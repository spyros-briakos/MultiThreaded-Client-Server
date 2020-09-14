#include <iostream>
#include <string>
#include <signal.h>

using namespace std;

struct Worker_Info
{
    char** dirs;
    char PCfifo[15];
    int num_of_dirs;
    pid_t pid;
};

int countCountries(const char* );
void print_array(Worker_Info** ,int );
void Father_Write(char* ,string ,int );
string Father_Read(char* ,int );
void sigchildhandler (int , siginfo_t *, void *);
void sigintorquithandler (int , siginfo_t *, void *);
void FatherReplaceDiedChild(Worker_Info*, int,char**,char*,char*);