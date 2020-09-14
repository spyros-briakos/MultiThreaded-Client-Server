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

class Buffer
{
    public:
        int* fds;
        int start;
        int end;
        int count;

        Buffer()
        : start(0),end(-1),count(0)
        {}
        ~Buffer()
        {
            delete[] fds;
        }
};

class Thread_Info
{
    public:
        int buffersize,client_sock,workers_sock;  
        string server_IP;

        Thread_Info(int b,int c,int w,string IP)
        : buffersize(b),client_sock(c),workers_sock(w),server_IP(IP)
        {}
};

class Worker_Info
{
    public:
        int port;
        string sum_stats;

        Worker_Info()
        :port(0),sum_stats("")
        {}
};

void place(Buffer* ,int ,int ); 
int obtain(Buffer* ,int );
int bind_on_port (struct sockaddr_in*,int , short ,const char*);
void print_error(string );
void* server_thread(void* );
void siginthandler(int , siginfo_t *, void *);
void socket_read(int ,string* );
int client_params (struct sockaddr_in* , short ,const char*);
string server_question_worker(string ,Thread_Info* ,int );
string server_coordinate(string ,Thread_Info* );
string get_answer(string ,Thread_Info* );
string get_answer2(string ,Thread_Info* );
string get_answer3(string ,Thread_Info* ,bool);
const char* getIP();