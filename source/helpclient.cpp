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
#include "../headers/helpclient.h"

using namespace std;

extern int count_line,client_threads_are_ready;
extern pthread_mutex_t count_lock,print_mtx,begin_mtx;
extern pthread_cond_t cond_begin;

void* client_thread(void* info_)
{
    //Get some significant infos from main_thread.
    Thread_Info* info = (Thread_Info*)info_;
    string query,answer;
    int thread_sock;
    char buf[1];
    bool terminated=false;
    struct sockaddr_in server;
    
    //Thread is waiting signal from main thread so as to cond_begin.
    if(pthread_mutex_lock(&begin_mtx))  print_error("mutex locked failed"); 
    client_threads_are_ready--;
    pthread_cond_wait(&cond_begin,&begin_mtx);
    if(pthread_mutex_unlock(&begin_mtx))  print_error("mutex unlocked failed");
    
    //Continue until all requests have been sent to server.
    while(1)
    {   
        //Create a thread_client-server socket.
        if((thread_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)  print_error("client socket creation failed");
        //Client fills some serious members of struct sockaddr_in server.
        client_params(&server,stoi(info->serverPort),info->serverIP.c_str());

        //Thread locks mutex so as to protect count_line.
        if(pthread_mutex_lock(&count_lock))  print_error("mutex locked failed");
        
        //Increase count_line if we have yet queries.
        if(count_line < info->num_of_queries)   
        {
            //Client tries to connect to server.
            if(connect(thread_sock,(struct sockaddr*)&server,sizeof(server)) < 0)  print_error("client connect failed") ;
        
            //Thread stores his query into a string.
            query = info->queries[count_line];
        
            //Increase line of queries, because we're sure that we are inside limits of array.
            count_line++;
        }
        //All queries have been sent so thread can finish.
        else terminated=true;
        
        //Thread unlocks mutex count_lock.
        if(pthread_mutex_unlock(&count_lock))  print_error("mutex unlocked failed");
        
        //Thread finishes cause terminated is true.
        if(terminated)  
        {
            //Thread closes socket.
            close(thread_sock);   
            break;
        }

        //Thread is writing to queryPort (server's socket) the request of client.
        if(write(thread_sock,query.c_str(),query.length()+1)<0) print_error("thread write ");  
        
        socket_read(thread_sock,&answer);

        //Thread is safely printing server's answer of its request.
        if(pthread_mutex_lock(&print_mtx))  print_error("mutex locked failed");
        cout << query << endl << answer << endl << endl;
        if(pthread_mutex_unlock(&print_mtx))    print_error("mutex unlocked failed");
    
        //Thread closes socket.
        close(thread_sock);
    }
}

int client_params(struct sockaddr_in* server ,short port, const char* serverIP) 
{
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(serverIP);
    server->sin_port = htons(port);
}

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