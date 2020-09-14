#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>   
#include <unistd.h> 
#include <dirent.h> 
#include <fstream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <ctype.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include "../headers/helpserver.h"

using namespace std;

int server_thread_count_sum_stats=0,num_of_workers=100;
extern pthread_mutex_t mtx,print_mtx,begin_mtx;
extern pthread_cond_t cond_nonempty,cond_nonfull,cond_begin;
extern Buffer buffer;  
extern volatile sig_atomic_t sigint;
extern bool sum_stats_done;
extern int server_threads_are_ready;
Worker_Info* worker_info_array;
bool create_info_array=false;
extern char* WorkersIP;

//Handler of SIGINT
void siginthandler(int sig, siginfo_t *siginfo, void *context)
{
	sigint=true;
}

//Function for printing errors...
void print_error(string err)
{
    perror(err.c_str());
    exit(-1);
}


void socket_read(int sd,string* answer)
{
    char buf[1];strcpy(buf,"");
    (*answer)="";

    while(1)  
    {
        buf[1]='\0';
        read(sd,buf,1);
        if(strcmp(buf,"\0")==0) break;     
        (*answer).append(buf);
    }
}

//Function which places a fd inside cycle buffer.
void place(Buffer* buffer,int fd,int bufferSize) 
{
    //Firstly lock mutex in order to place only one thread per time.
    if(pthread_mutex_lock(&mtx))  print_error("mutex locked failed");
    while(buffer->count>=bufferSize) 
    {
        //Wait until (signal come) buffer is nonfull, do not check while condition many times.
        pthread_cond_wait(&cond_nonfull,&mtx);
    }
    //Chanfe buffer end.
    buffer->end = (buffer->end+1) % bufferSize;
    //Obtain the 'last' fd of cycle buffer.
    buffer->fds[buffer->end]=fd;
    //We have one more fd into our cycle buffer.
    buffer->count++;
    //Finally unlock mutex, next thread please.
    if(pthread_mutex_unlock(&mtx))  print_error("mutex unlocked failed");
}

//Function which obtains a fd inside cycle buffer.
int obtain(Buffer* buffer,int bufferSize) 
{
    //Firstly lock mutex in order to obtain only one thread per time.
    if(pthread_mutex_lock(&mtx))  print_error("mutex locked failed");
    while(buffer->count<=0) 
    {    
        //Wait until (signal come) buffer is nonempty, do not check while condition many times.
        pthread_cond_wait(&cond_nonempty,&mtx);
        //Case SIGINT signal came...
        if(sigint)  
        {
            if(pthread_mutex_unlock(&mtx))  print_error("mutex unlocked failed");
            return -10;
        }
        //Case summary statistics are over and now is client's turn.
        if(sum_stats_done)
        {
            if(pthread_mutex_unlock(&mtx))  print_error("mutex unlocked failed");
            return -12;
        }
    }
    //Obtain the 'first' fd of cycle buffer.
    int fd = buffer->fds[buffer->start];
    //Chanfe buffer start.
    buffer->start=(buffer->start+1) % bufferSize;
    //We have one less fd into our cycle buffer.
    buffer->count--;
    //Finally unlock mutex, next thread please.
    if(pthread_mutex_unlock(&mtx))  print_error("mutex unlocked failed");
    //Return file descriptor.
    return fd;
}

int client_params(struct sockaddr_in* server ,short port, const char* serverIP) 
{
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(serverIP);
    server->sin_port = htons(port);
}

//Function, which fills significat params of struct sockaddr_in.
int bind_on_port(struct sockaddr_in* server,int sock ,short port, const char* serverIP) 
{
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(serverIP);
    server->sin_port = htons(port);
    return bind(sock,(struct sockaddr*)server,sizeof(*server));
}

//Return IP of linux machine.
const char *getIP()
{
    char host[256];
    char *IP;
    struct hostent *host_entry;
    int hostname;
    hostname = gethostname(host, sizeof(host)); //find the host name
    if (hostname == -1) 
    {
      perror("gethostname");
      exit(1);
    }
    host_entry = gethostbyname(host); //find host information
    if (host_entry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
    return (const char* )IP;
}

//Function of each thread of server.
void* server_thread(void* info_)
{
    //Get some significant infos from main_thread.
    Thread_Info* info = (Thread_Info*)info_;
    string worker_port,stats,request,workers,final_answer;
    int socket_descriptor;
    char buf[1];

    //Server_Thread is getting summary statistics and prints them to stdout.
    while(1)
    {
        //Server_Thread obtains socket descriptor from buffer so as to begin its communication with a single client_thread.
        socket_descriptor = obtain(&buffer,info->buffersize);
        
        //This case means that all workers have sent their sum stats.
        if(socket_descriptor==-12)  break;
        if(socket_descriptor==-10)  pthread_exit(NULL);
        
        //Server_thread alerts that buffer isn't full.
        pthread_cond_signal(&cond_nonfull);
        
        //Server_Thread reads num_of_workers.
        socket_read(socket_descriptor,&workers); 
        
        //Server_Thread reads worker_port.
        socket_read(socket_descriptor,&worker_port);
        
        //Server_Thread reads sum_stats.
        socket_read(socket_descriptor,&stats);
        
        //Store num_of_workers so as to main_thread see it.
        num_of_workers=stoi(workers);

        //Lock mutex.
        if(pthread_mutex_lock(&print_mtx))  print_error("mutex locked failed");

        //First thread will create the info array and update num_of_workers so as main_server_thread knows it.
        if(!create_info_array)
        {
            //Allocate space for global Worker's Info array
            worker_info_array = new Worker_Info[num_of_workers];
            create_info_array=true;
        }
        //Store to array port and sum stats of this worker.
        worker_info_array[server_thread_count_sum_stats].port=stoi(worker_port);
        worker_info_array[server_thread_count_sum_stats].sum_stats=stats;
        //Increase counter so as to main_server_thread when all threads have finished with sum stats.
        server_thread_count_sum_stats++;
        //Unlock mutex.
        if(pthread_mutex_unlock(&print_mtx))    print_error("mutex unlocked failed");  
        
        //Server_Thread closes socket.
        close(socket_descriptor);  
    }
    
    //Thread is waiting signal from main thread so as to cond_begin.
    if(pthread_mutex_lock(&begin_mtx))  print_error("mutex locked failed"); 
    server_threads_are_ready--;
    pthread_cond_wait(&cond_begin,&begin_mtx);
    if(pthread_mutex_unlock(&begin_mtx))  print_error("mutex unlocked failed");
    
    //Server_Thread is doing its work, communication (clients&workers).
    while(1)
    {
        //Server_Thread obtains socket descriptor from buffer so as to begin its communication with a single client_thread.
        socket_descriptor = obtain(&buffer,info->buffersize);

        //Case: obtain caught SIGINT signal so thread finishes.
        if(socket_descriptor==-10)  break;

        //Server_thread alerts that buffer isn't full.
        pthread_cond_signal(&cond_nonfull);

        //Server_thread reads request from client.
        socket_read(socket_descriptor,&request);

        //Call function to do the job.
        final_answer = server_coordinate(request,info);
        
        //Server_Thread is responding to client thread about its request's answer.
        if(write(socket_descriptor,final_answer.c_str(),final_answer.length()+1)<0) print_error("thread write ");  

        //Server_Thread is safely printing client's request and answer of client's request, which obtained from Workers.
        if(pthread_mutex_lock(&print_mtx))  print_error("mutex locked failed");
        cout << request << endl << final_answer << endl << endl;
        if(pthread_mutex_unlock(&print_mtx))    print_error("mutex unlocked failed");  

        //Server_Thread closes client-server socket.
        close(socket_descriptor);  
    }
}

//Function which sees what query do we have and call the appropriate function.
string server_coordinate(string request,Thread_Info* info)
{
    stringstream rt(request);
    string words[9];
    int i=0;
    while(rt >> words[i]) i++;   
    
    if(words[0]=="/diseaseFrequency" && (i==4 || i==5))
        return get_answer(request,info); 
    else if((words[0]=="/searchPatientRecord" && i==2) || (words[0]=="/topk-AgeRanges" && i==6))
        return get_answer2(request,info); 
    else if((words[0]=="/numPatientAdmissions" && (i==4 || i==5)) || (words[0]=="/numPatientDischarges" && (i==4 || i==5)))
    {
        if(i==4)    return get_answer3(request,info,false); 
        if(i==5)    return get_answer3(request,info,true); 
    }
    else
        return "ERROR";
}

//Function for diseaseFrequency.
string get_answer(string request,Thread_Info* info)
{
    int errors=0,j,cases=0;
    string f;

    for(j=0;j<(num_of_workers);j++)  
    {
        f = server_question_worker(request,info,worker_info_array[j].port);
        if(f!="ERROR")  cases+=stoi(f);
        else    errors++;
    }
    if(errors!=num_of_workers)  return to_string(cases);
    else    return "ERROR";
}

//Function for searchPatientRecord or Topk-AgeRanges.
string get_answer2(string request,Thread_Info* info)
{
    int errors=0,j,cases=0;
    string f="";

    for(j=0;j<(num_of_workers);j++)  
    {
        f = server_question_worker(request,info,worker_info_array[j].port);
        if(f!="ERROR")  
        {
            //Remove \n
            f = f.substr(0, f.size()-1);
            return f;
        }
        else    errors++;
    }
    if(errors==num_of_workers)  return "ERROR";
}

//Function for numPatientAdmissions or numPatientDischarges.
string get_answer3(string request,Thread_Info* info,bool with_country)
{
    string admissions_or_discharges,f;
    int errors=0,j,cases=0;

    //Case: without country filter.
    if(!with_country)
    {
        for(j=0;j<num_of_workers;j++)   
        {
            f = server_question_worker(request,info,worker_info_array[j].port);
            if(f!="ERROR")  admissions_or_discharges.append(f);
            else    errors++;
        }
        if(errors!=num_of_workers)  
        {
            //Remove \n
            admissions_or_discharges = admissions_or_discharges.substr(0, admissions_or_discharges.size()-1);
            return admissions_or_discharges;
        }
        else    return "ERROR";  
    }
    //Case: with country filter.
    else
    {
        int cases=0;
        string country;
        for(j=0;j<num_of_workers;j++)   
        {
            f = server_question_worker(request,info,worker_info_array[j].port);
            if(f!="ERROR")  
            {
                stringstream rt(f);
                string params[2];
                int q=0;
                while(rt >> params[q]) q++; 
                country=params[0];
                cases+=stoi(params[1]);
            }
            else    errors++;
        }
        if(errors!=num_of_workers)  
        {
            admissions_or_discharges.append(country);
            admissions_or_discharges.append(" ");
            admissions_or_discharges.append(to_string(cases));
            return admissions_or_discharges;
        }
        else    return "ERROR";  
    }
     
}

//Function which connects to a specific worker writes a query and returns the answer.
string server_question_worker(string request,Thread_Info* info,int worker_port)
{
    struct sockaddr_in server_worker;
    int server_worker_sock;
    string answer;

    //Create a server-worker socket.
    if((server_worker_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)  print_error("client socket creation failed");
    
    //Client fills some serious members of struct sockaddr_in server.
    client_params(&server_worker,worker_port,WorkersIP);

    //Server_thread tries to connect to each worker.
    if(connect(server_worker_sock,(struct sockaddr*)&server_worker,sizeof(server_worker)) < 0)  print_error("server-worker connect failed") ;

    //Server_thread question worker for client's request.
    if(write(server_worker_sock,request.c_str(),request.length()+1)<0) print_error("thread write ");  
    
    //Server_thread reads the answer of each worker.
    socket_read(server_worker_sock,&answer);

    //Server_thread closes server-worker socket.
    close(server_worker_sock);

    return answer;
}
