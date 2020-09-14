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
#include "../headers/helpclient.h"

using namespace std;

pthread_mutex_t count_lock,print_mtx,begin_mtx;
pthread_cond_t cond_begin,print_sth;
int count_line=0,client_threads_are_ready=0;
string* queries = NULL;

int main(int argc,char* argv[])
{
    int i=0,j=0,k=0,q=0,numThreads=0,err=0,port=0,number_of_queries=0;
    string queryFile="",serverIP="",serverPort="",line="";
    fstream newfile;
    
    if(argc!=9)
    {
        cout << "Incorrect Input!Please try the above format:" <<  endl << "./whoClient –q queryFile -w numThreads –sp servPort –sip servIP" << endl;
        return -1;
    }
    for(i=1;i<=7;i+=2)
    {            
        if(strcmp(argv[i],"-w")==0) numThreads=atoi(argv[i+1]);
        else if(strcmp(argv[i],"-q")==0) queryFile = argv[i+1];
        else if(strcmp(argv[i],"-sp")==0) serverPort = argv[i+1];
        else if(strcmp(argv[i],"-sip")==0) serverIP = argv[i+1];
        else return -1;
    }

    //Check for positive values of parameters.
    if(numThreads<=0)
    {
        cout << "numThreads must be a positive number!" << endl;
        return -1;
    }

    //Count how many queries do we have...
    newfile.open(queryFile,ios::in); 
    if(!newfile.is_open())  
    {
        cout << "Cannot open date file " << queryFile << endl;
        return -1;
    }
    while(getline(newfile,line))    number_of_queries++;
    newfile.close();
    
    //Client is starting reading line by line queryFile and stores into string array queries.
    queries = new string[number_of_queries];
    newfile.open(queryFile,ios::in); 
    if(!newfile.is_open())  
    {
        cout << "Cannot open date file " << queryFile << endl;
        return -1;
    }
    while(getline(newfile,line))    queries[q++]=line;
    newfile.close();
    
    //Initialization of mutexes and condition variables.
    pthread_mutex_init(&count_lock,0);
    pthread_mutex_init(&print_mtx,0);
    pthread_mutex_init(&begin_mtx,0);
    pthread_cond_init(&cond_begin,0);

    //Allocate and store info which will be useful for threads.
    Thread_Info* info = new Thread_Info(number_of_queries,queries,serverIP,serverPort);
    client_threads_are_ready=numThreads;
                                    
    //Client is starting creating threads.
    pthread_t* threads_array = new pthread_t[numThreads];
    for(i=0;i<numThreads;i++)
        if(err = pthread_create(threads_array+i,NULL,client_thread,info))
            print_error("client pthread_create failed");
    
    //Main thread alerts threads to begin their jobs.
    while(client_threads_are_ready>0);
    if(pthread_mutex_lock(&begin_mtx))  print_error("mutex locked failed"); 
    pthread_cond_broadcast(&cond_begin);
    if(pthread_mutex_unlock(&begin_mtx))  print_error("mutex unlocked failed");

    //Main thread wait for active threads to finish...
    for(i=0;i<numThreads;i++)   pthread_join(threads_array[i],NULL);    
    
    //Print end-message...
    cout << "Client has been served!" << endl;
    
    //Deallocation of memory...
    pthread_cond_destroy(&cond_begin);
    pthread_mutex_destroy(&begin_mtx);
    pthread_mutex_destroy(&count_lock);
    pthread_mutex_destroy(&print_mtx);
    delete[] queries;
    delete info;
    delete[] threads_array;
    return 0;
}