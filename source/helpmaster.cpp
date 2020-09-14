#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <fcntl.h>   
#include <unistd.h> 
#include <sys/wait.h>
#include <dirent.h> 
#include <fstream>
#include <signal.h>
#include "../headers/helpmaster.h"

using namespace std;

extern volatile sig_atomic_t died_pid;
extern volatile sig_atomic_t sigintorquit;

void sigchildhandler(int sig, siginfo_t *siginfo, void *context)
{
	died_pid=(long)siginfo->si_pid;
}

void sigintorquithandler(int sig, siginfo_t *siginfo, void *context)
{
	sigintorquit=true;
}

void Father_Write(char* PCfifo,string message,int buffersize)
{
    char sum_stats[message.size()+1];
    strcpy(sum_stats,message.c_str());
    sum_stats[message.size()] = '\0';
    int last_bytes=(strlen(sum_stats)+1)%buffersize,PCfd,k;
    int chunk_times=(strlen(sum_stats)+1)/buffersize;

    //Father opens PCfifo in order to write sum_stats.
    if((PCfd = open(PCfifo, O_WRONLY))<0)   perror("Father cannot open PCfifo!");
    
    //Case: Send chunk_times same-bytes messages.
    if(last_bytes==0)
    {
        for(k=0;k<chunk_times;k++)  write(PCfd,&sum_stats[k*buffersize],buffersize);
    }
    //Case: Send chunk_times-1 same-bytes messages and last message with last_bytes.
    else
    {
        for(k=0;k<=chunk_times;k++)
        {
            if(k==chunk_times)  write(PCfd,&sum_stats[k*buffersize],last_bytes);
            else    write(PCfd,&sum_stats[k*buffersize],buffersize); 
        }
    }
    
    //Father closes PCfifo.
    close(PCfd);
}

string Father_Read(char* CPfifo,int buffersize)
{
    char buff[buffersize];buff[buffersize]='\0';strcpy(buff,"");int CPfd;string message="";
    
    //Father opens named pipe so as to read.
    if((CPfd = open(CPfifo, O_RDONLY ))<0)   perror("Father cannot open CPfifo!");

    //Father reads buffersize bytes each time so as to collect summary stats and print them.
    while((read(CPfd,buff,buffersize))>0)
    {
        string buffer(buff); 
        strcpy(buff,"");       
        message.append(buffer);
    }

    //Father closes CP_pipe.
    close(CPfd);

    //Return what Father read.
    return message;
}

int countCountries(const char* filename) 
{
    string myText;
    ifstream MyReadFile(filename);
    int temp=0;
    while (getline (MyReadFile, myText)) temp++;
    MyReadFile.close();
    return temp;
}

void print_array(Worker_Info** array,int num)
{
    for(int i=0;i<num;i++)
    {
        cout << "===================" << endl;
        cout << array[i]->PCfifo << endl << array[i]->pid << endl;
        for(int k=0;k<array[i]->num_of_dirs;k++)
        {
            cout << array[i]->dirs[k] << endl;
        }
    }
}

void FatherReplaceDiedChild(Worker_Info* child_info,int buffersize,char** path,char* serverIP,char* serverPost)
{
    int PCfd,q,k,chunk_times,last_bytes;
    char space[1] = {' '},last_char[1] = {'\0'},new_line[1] = {'\n'},num_of_dirs[20];

    //Father passes some significant information to new Worker through named pipe.
    if((PCfd = open(child_info->PCfifo, O_WRONLY ))<0)   perror("Father cannot open PCfifo!");
    
    //Write number of dirs of Worker!
    sprintf(num_of_dirs,"%d",child_info->num_of_dirs);
    write(PCfd,num_of_dirs,strlen(num_of_dirs));
    write(PCfd,new_line,1);

    //Father informs Worker which dirs is going manipulate.
    for(q=0;q<child_info->num_of_dirs;q++)
    {
        //Calculate how many splits are we going to do for each country and last_bytes of country.
        chunk_times = strlen(child_info->dirs[q])/buffersize;
        last_bytes = strlen(child_info->dirs[q])%buffersize;

        //Case: Send chunk_times same-bytes messages.
        if(last_bytes==0)
        {
            for(k=0;k<chunk_times;k++)
                write(PCfd,&child_info->dirs[q][k*buffersize],buffersize);
        }
        //Case: Send chunk_times-1 same-bytes messages and last message with last_bytes.
        else
        {
            for(k=0;k<=chunk_times;k++)
            {
                if(k==chunk_times)  
                    write(PCfd,&child_info->dirs[q][k*buffersize],last_bytes);
                else    
                    write(PCfd,&child_info->dirs[q][k*buffersize],buffersize); 
            }
        }
        //Write a space between countries.
        if(q!=(child_info->num_of_dirs-1))   write(PCfd,space,1);
    }
    //Write character which will indicate that we have next info and specifically the name of input_dir!
    write(PCfd,new_line,1);
    write(PCfd,*path,strlen(*path));

    //Write character which will indicate that we have next info and specifically the name of serverIP!
    write(PCfd,new_line,1);
    write(PCfd,serverIP,strlen(serverIP));
    
    //Write character which will indicate that we have next info and specifically the name of serverIP!
    write(PCfd,new_line,1);
    write(PCfd,serverPost,strlen(serverPost));


    //Write character which will indicate that message is over!
    write(PCfd,last_char,1);
    //Father close named pipe so as to Worker understand that he can read now...
    close(PCfd);

    //Father reads but he doesnt print again Summary Statistics from new Worker.
    // Father_Read(child_info->CPfifo,buffersize);
}