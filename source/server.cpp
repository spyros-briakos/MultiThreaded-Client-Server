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
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../headers/helpserver.h"

using namespace std;

//Global Variables
pthread_mutex_t mtx,print_mtx,begin_mtx;
pthread_cond_t cond_nonempty,cond_nonfull,cond_begin;
Buffer buffer;
bool sum_stats_done=false;
volatile sig_atomic_t sigint = false;
int server_threads_are_ready=0;
extern Worker_Info* worker_info_array;
extern int server_thread_count_sum_stats,num_of_workers;
extern bool create_info_array;
char* WorkersIP = NULL;

int main(int argc,char* argv[])
{
    int one=1,i=0,j=0,k=0,q=0,numThreads=0,num_of_queries=0,inactive_threads=0,div=0,mod=0,err=0,newsock,socket_descriptor,buffersize,client_sock,workers_sock,statistic_sock,count_workers=0;
    string queryPortNum,statisticsPortNum;
    char* serverIP = (char* )malloc((strlen(getIP())+1)*sizeof(char));
    strcpy(serverIP,getIP());
    struct sockaddr_in client_server,workers_server;
    socklen_t clientlen = sizeof(client_server);
    socklen_t workerslen = sizeof(workers_server);

    if(argc!=9)
    {
        cout << "Incorrect Input!Please try the above format:" <<  endl << "./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize" << endl;
        return -1;
    }
    for(i=1;i<=7;i+=2)
    {            
        if(strcmp(argv[i],"-w")==0) numThreads=atoi(argv[i+1]);
        else if(strcmp(argv[i],"-q")==0) queryPortNum = argv[i+1];
        else if(strcmp(argv[i],"-s")==0) statisticsPortNum = argv[i+1];
        else if(strcmp(argv[i],"-b")==0) buffersize = atoi(argv[i+1]);
        else if(strcmp(argv[i],"-b")==0) buffersize = atoi(argv[i+1]);
        else return -1;
    }

    //Check for positive values of parameters.
    if(numThreads<=0)
    {
        cout << "numThreads must be a positive number!" << endl;
        return -1;
    }
    if(buffersize<=0)
    {
        cout << "bufferSize must be a positive number!" << endl;
        return -1;
    }

    //Initialization of mutexes and condition variables.
    pthread_mutex_init(&mtx,0);
    pthread_mutex_init(&print_mtx,0);
    pthread_mutex_init(&begin_mtx,0);
    pthread_cond_init(&cond_begin,0);
    pthread_cond_init(&cond_nonempty,0);
    pthread_cond_init(&cond_nonfull,0);
    buffer.fds = new int[buffersize];

    //Sigaction for SIGINT signal...
    struct sigaction act1;
	memset (&act1, 0, sizeof(act1)); 
	act1.sa_sigaction = siginthandler;
 	act1.sa_flags = SA_SIGINFO;
	if(sigaction(SIGINT, &act1, NULL) < 0)  print_error("SERVER SIGINT ERROR!");

    //Create a statisticsPort socket.
    if((workers_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)  print_error("server worker socket creation failed");
    //Make port reusable.
    if (setsockopt(workers_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0)   print_error("setsockopt(SO_REUSEADDR) failed");
    //Bind socket to address.
    if(bind_on_port(&workers_server,workers_sock,stoi(statisticsPortNum),serverIP)<0)  print_error("bind for workers failed");
    //Listen for connections.
    if(listen(workers_sock,1000) < 0) print_error("listen workers_sock failed");

    //Create a queryPort socket.
    if((client_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)  print_error("server client socket creation failed");
    //Make port reusable.
    if (setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0)   print_error("setsockopt(SO_REUSEADDR) failed");
    //Bind socket to address.
    if(bind_on_port(&client_server,client_sock,stoi(queryPortNum),serverIP)<0)  print_error("bind for client failed");
    //Listen for connections.
    if(listen(client_sock,1000) < 0) print_error("listen client_sock failed");
    
    //Allocate and store info which will be useful for threads.
    Thread_Info* info = new Thread_Info(buffersize,client_sock,workers_sock,serverIP);
    server_threads_are_ready = numThreads;

    //Allocate space for threads_array and each thread is getting ready to do his work.
    pthread_t* threads_array = new pthread_t[numThreads];
    for(j=0;j<numThreads;j++)
        if(err = pthread_create(threads_array+j,NULL,server_thread,info))
            print_error("server pthread_create failed");

    //Main thread is started getting statistics from Workers and at the same time each worker's port.
    if(pthread_mutex_lock(&print_mtx))  print_error("mutex locked failed");
    cout << endl << "Listening for worker's connections to port:" << statisticsPortNum << "  IP:" << serverIP << endl;
    if(pthread_mutex_unlock(&print_mtx))    print_error("mutex unlocked failed");    
    
    //Until all workers have tried to connect to server loop.
    while(count_workers<num_of_workers)
    {
        //Accept connection of a worker.
        if((statistic_sock = accept(workers_sock,(struct sockaddr*)&workers_server,&workerslen)) < 0)   cout << "Accept of worker connection failed!" << endl;

        //Get Worker's IP only once.
        if(WorkersIP==NULL)  
        {
            WorkersIP = (char*)malloc((strlen(inet_ntoa(workers_server.sin_addr))+1)*sizeof(char));
            strcpy(WorkersIP,inet_ntoa(workers_server.sin_addr));
        }
        
        //Increase counter of workers.
        count_workers++;
        cout << "Accepted connection of worker " << count_workers  << " " << statistic_sock  << " " << num_of_workers << " WIP " << WorkersIP << endl;
        
        //Main thread places socket descriptor to cycle buffer and updates with signal cond_nonempty.
        place(&buffer,statistic_sock,buffersize);
        //Main thread alerts that buffer isn't empty.
        pthread_cond_signal(&cond_nonempty);
        //Only for the first server-thread wait until worker_info_array has been created.
        while(!create_info_array);
    }
    
    //Main thread wait all threads finish their job with sum stats and 
    //then alerts them to unblock from condnonempty     so as to each thread decrease server_threads_are_ready.
    while(server_thread_count_sum_stats<num_of_workers);
    sum_stats_done=true;
    string all_sum_stats;
    for(q=0;q<num_of_workers;q++)   all_sum_stats.append(worker_info_array[q].sum_stats);
    cout << endl << all_sum_stats;
    pthread_cond_broadcast(&cond_nonempty);

    //Main thread wait all threads reach a point before while(1) of client services.
    while(server_threads_are_ready>0);
    if(pthread_mutex_lock(&begin_mtx))  print_error("mutex locked failed"); 
    // Update variable so as to not play with this. 
    sum_stats_done=false;
    pthread_cond_broadcast(&cond_begin);
    if(pthread_mutex_unlock(&begin_mtx))  print_error("mutex unlocked failed");


    //Main thread is started getting requests from client_threads...
    if(pthread_mutex_lock(&print_mtx))  print_error("mutex locked failed");
    cout << "Listening for client's connections to port:" << queryPortNum << "  IP:" << serverIP << endl << endl;
    if(pthread_mutex_unlock(&print_mtx))    print_error("mutex unlocked failed");    
    
    while(1)
    {        
        //Accept connection.
        if((newsock = accept(client_sock,(struct sockaddr*)&client_server,&clientlen)) < 0) 
        {
            //Case: Accept failed but not due to SIGINT.
            if(!sigint) print_error("server accept failed");
            //Case: Accept failed and the reason is SIGINT signal.
            else
            {
                if(pthread_mutex_lock(&print_mtx))  print_error("mutex locked failed");
                cout << endl << "Server has been terminated due to SIGINT signal!" << endl;
                if(pthread_mutex_unlock(&print_mtx))    print_error("mutex unlocked failed");    
                //Unblock all threads so as to finish.
                pthread_cond_broadcast(&cond_nonempty);
                //Close socket descriptors of buffer.
                for(q=0;q<buffer.count;q++) close(buffer.fds[q]);
                //Main thread wait for threads to finish and then finishes.
                for(i=0;i<numThreads;i++)   pthread_join(threads_array[i],NULL); 
                break;
            }
        }

        //Main thread places socket descriptor to cycle buffer and updates with signal cond_nonempty.
        place(&buffer,newsock,buffersize);
        pthread_cond_signal(&cond_nonempty);
    }
    
    //Server close port which used to listen client's connenctions.
    close(client_sock);
    //Server close port which used to listen worker's connenctions.
    close(workers_sock);
    //Destroy condition variables.
    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_cond_destroy(&cond_begin);
    //Destroy mutexes.
    pthread_mutex_destroy(&begin_mtx);
    pthread_mutex_destroy(&mtx);
    pthread_mutex_destroy(&print_mtx);
    //Deallocation of memory.
    delete info;
    delete[] threads_array;
    delete[] worker_info_array;
    free(WorkersIP);
    free(serverIP);

    return 0;
}