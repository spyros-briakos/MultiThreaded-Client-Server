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

using namespace std;

class Thread_Info
{
    public:
        int num_of_queries;
        string* queries;
        string serverIP,serverPort;
        
        Thread_Info(int num,string* q,string IP,string port)
        : num_of_queries(num),queries(q),serverIP(IP),serverPort(port)
        {}
};

void * client_thread(void* );
void print_error(string );
int client_params (struct sockaddr_in* , short ,const char*);
void socket_read(int ,string* );