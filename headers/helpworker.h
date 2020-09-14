#ifndef HELPWORKER_H
#define HELPWORKER_H

#include <iostream>
#include <signal.h>
#include "bucket_list.h"

using namespace std;

class Node
{
    public:
        string name;
        Node* next;

        Node(string _name)
        :name(_name),next(NULL)
        {}
};


void swap(string *, string *);
void selectionsort(string* , int );
void printArray(string [], int );

string Worker_Read(char* ,int );
void Worker_Write(char* ,string ,int );


void insert(Node** ,string );  
void delete_list(Node* );
void print_list(Node* );
int length(Node* );

void sigintorquithandler(int , siginfo_t *, void *);
void sigusr1handler(int , siginfo_t *, void *);
void WorkerHasNewDateFiles(int ,char*** ,char* ,string**  ,RecordNode** ,int, BucketNode**, BucketNode**);

void print_error(string );
int client_params (struct sockaddr_in* , short ,const char*);
int bind_on_port(struct sockaddr_in* ,int  ,short , const char* );
void socket_read(int ,string* );
const char *getIP();
#endif