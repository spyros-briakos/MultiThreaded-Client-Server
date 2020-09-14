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
#include "../headers/helpmaster.h"

using namespace std;

volatile sig_atomic_t died_pid=-1;
volatile sig_atomic_t sigintorquit = false;

int main(int argc,char* argv[])
{
    int i=0,j=0,k=0,q=0,countries=0,numWorkers,buffersize,pid,num_of_forks,PCfd,CPfd,last_bytes=0,chunk_times=0,cases=0;
    string input_dir,myText,all_sum_stats="",message ="",f="",serverIP,serverPost;
    Worker_Info** info_array;
    char space[1] = {' '},last_char[1] = {'\0'},new_line[1] = {'\n'},num_of_dirs[20];
    struct dirent *entry;
    DIR *dir;

    if(argc!=11)
    {
        cout << "Incorrect Input!Please try the above format:" <<  endl << "./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir" << endl;
        return -1;
    }
    for(i=1;i<=9;i+=2)
    {            
            if(strcmp(argv[i],"-w")==0) numWorkers=atoi(argv[i+1]);
            else if(strcmp(argv[i],"-i")==0) input_dir = argv[i+1];
            else if(strcmp(argv[i],"-b")==0) buffersize = atoi(argv[i+1]);
            else if(strcmp(argv[i],"-s")==0) serverIP = argv[i+1];
            else if(strcmp(argv[i],"-p")==0) serverPost = argv[i+1];
            else return -1;
    }

    //Check for positive values of parameters.
    if(buffersize<=0)
    {
        cout << "BufferSize must be a positive number!" << endl;
        return -1;
    }
    if(numWorkers<=0)
    {
        cout << "numWorkers must be a positive number!" << endl;
        return -1;
    }

    //Convert serverIP,serverPost from string to char*.
    char* serverIP_ = new char[serverIP.length()+1];
    strcpy(serverIP_,serverIP.c_str());
    char* serverPost_ = new char[serverPost.length()+1];
    strcpy(serverPost_,serverPost.c_str());

    //Convert input_dir from string to char*.
    char* path = new char[input_dir.length()+1];
    strcpy(path,input_dir.c_str());

    //Calculate num of countries.
    dir = opendir(path);
    if(dir==0) 
    {
        cout << "The parameter input_dir that you gave doesn't exist.Try again!" << endl;
        return -1;
    }
    while ((entry = readdir(dir)) != NULL)  
        if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)   
            countries++;
    closedir(dir);

    //Store countries from input_dir to countries_array.
    char** countries_array = new char*[countries];
    dir = opendir(path);
    while ((entry = readdir(dir)) != NULL) 
    {
        if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)
        {
            countries_array[k] = new char[strlen(entry->d_name)+1];
            strcpy(countries_array[k],entry->d_name);
            k++;
        }
    }
    closedir(dir);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Start to do some real work for father//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Case: Some Workers will manipulate more than one dirs.
    if(countries >= numWorkers)
    {
        //Calculate how many forks are going to happen and then allocate memory for info_array
        num_of_forks = numWorkers;
        info_array = new Worker_Info*[num_of_forks];

        //Calculate some values so as to split dirs uniformly.
        int dirs_per_worker = countries/numWorkers;
        int mod = countries%numWorkers;

        //Case: Some Workers are going to have more dirs than others.
        if(mod!=0)
        {
            for(i=0;i<numWorkers;i++)
            {
                //Allocate memory for each Worker_Info struct.
                info_array[i] = new Worker_Info;

                //First mod Workers will have more dirs_per_worker+mod directories.
                if(i<mod)
                {
                    //Allocate memory for directories of Worker.
                    info_array[i]->dirs = new char* [dirs_per_worker+1];
                    info_array[i]->num_of_dirs = dirs_per_worker+1;
                    for(int q=0;q<(dirs_per_worker+1);q++)
                    {
                        info_array[i]->dirs[q] = new char[strlen(countries_array[j])+1];
                        strcpy(info_array[i]->dirs[q],countries_array[j]);
                        j++;
                    }
                }
                //The rest Workers will have dirs_per_worker directories.
                else
                {
                    //Allocate memory for directories of Worker.
                    info_array[i]->dirs = new char* [dirs_per_worker];
                    info_array[i]->num_of_dirs = dirs_per_worker;
                    for(int q=0;q<(dirs_per_worker);q++)
                    {
                        info_array[i]->dirs[q] = new char[strlen(countries_array[j])+1];
                        strcpy(info_array[i]->dirs[q],countries_array[j]);
                        j++;
                    }
                }
            }
        }
        //Case: All Workers will have the same number of dirs.
        else
        {
            for(i=0;i<numWorkers;i++)
            {
                //Allocate memory for each Worker_Info struct.
                info_array[i] = new Worker_Info;

                //Allocate memory for directories of Worker.
                info_array[i]->dirs = new char* [dirs_per_worker];
                info_array[i]->num_of_dirs = dirs_per_worker;
                for(int q=0;q<(dirs_per_worker);q++)
                {
                    info_array[i]->dirs[q] = new char[strlen(countries_array[j])+1];
                    strcpy(info_array[i]->dirs[q],countries_array[j]);
                    j++;
                }
            }
        }  
    }
    else
    {    
        num_of_forks = countries;
        info_array = new Worker_Info*[num_of_forks];
        
        for(i=0;i<num_of_forks;i++)
        {
            //Allocate memory for each Active Worker_Info struct.
            info_array[i] = new Worker_Info;

            //Allocate memory for directory of Active Worker.
            info_array[i]->dirs = new char*[1];
            info_array[i]->num_of_dirs = 1;
            info_array[i]->dirs[0] = new char[strlen(countries_array[j])+1];
            strcpy(info_array[i]->dirs[0],countries_array[j]);
            j++;  
        }
    }
    
    //Store names of pair of pipes of each Worker into struct Worker_Info.
    char PCfifo[20];
    char CPfifo[20];
    for(i=0;i<(num_of_forks);i++)
    {
        sprintf(PCfifo,"pipes/PCfifo%d",i);
        strcpy(info_array[i]->PCfifo,PCfifo);
        if (mkfifo(PCfifo, 0666) < 0) perror("Cannot create named pipe!");
    }
    
    //Create num_of_forks Workers with fork and each Worker run worker.cpp.
    for(i=0;i<numWorkers; i++)
    { 
        pid = fork();

        //Fork failed
        if(pid==-1)
        { 
            perror("Failed to fork");
            return 1;
        }
        //Workers can continue.
        else if(pid==0)
        {
            //Active Workers. 
            if(i<num_of_forks)
            {   
                char p[40];
                char buff[10],forks_[10];
                sprintf(buff, "%d", buffersize);
                sprintf(forks_, "%d", num_of_forks);
                sprintf(p,"./executable/worker");
                char* arr[] = { p, info_array[i]->PCfifo, buff,forks_, NULL};
                execv("./executable/worker", arr);
            }
            //Inactive Workers.
            //Case: numWorkers>countries.
            else
            {
                char p[40];
                sprintf(p,"./executable/inactive_worker");
                char* arr[] = {p, NULL};
                execv("./executable/inactive_worker", arr);
            } 
        }
        //Father stores pids of his active children.
        else    
            if(i<num_of_forks)  info_array[i]->pid=pid;
    }

    cout << endl << "Master " << getpid() << endl;

    //Father passes some significant information to each Worker through named pipes.
    for(i=0;i<(num_of_forks);i++)
    {
        //Father opens named pipe so as to write.
        if((PCfd = open(info_array[i]->PCfifo, O_WRONLY ))<0)   perror("Father cannot open PCfifo!");
        
        //Write number of dirs of Worker!
        sprintf(num_of_dirs,"%d",info_array[i]->num_of_dirs);
        write(PCfd,num_of_dirs,strlen(num_of_dirs));
        write(PCfd,new_line,1);

        //Father informs Worker which dirs is going manipulate.
        for(q=0;q<info_array[i]->num_of_dirs;q++)
        {
            //Calculate how many splits are we going to do for each country and last_bytes of country.
            chunk_times = strlen(info_array[i]->dirs[q])/buffersize;
            last_bytes = strlen(info_array[i]->dirs[q])%buffersize;

            //Case: Send chunk_times same-bytes messages.
            if(last_bytes==0)
            {
                for(k=0;k<chunk_times;k++)
                    write(PCfd,&info_array[i]->dirs[q][k*buffersize],buffersize);
            }
            //Case: Send chunk_times-1 same-bytes messages and last message with last_bytes.
            else
            {
                for(k=0;k<=chunk_times;k++)
                {
                    if(k==chunk_times)  
                        write(PCfd,&info_array[i]->dirs[q][k*buffersize],last_bytes);
                    else    
                        write(PCfd,&info_array[i]->dirs[q][k*buffersize],buffersize); 
                }
            }
            //Write a space between countries.
            if(q!=(info_array[i]->num_of_dirs-1))   write(PCfd,space,1);
        }
        //Write character which will indicate that we have next info and specifically the name of input_dir!
        write(PCfd,new_line,1);
        write(PCfd,path,strlen(path));

        //Write character which will indicate that we have next info and specifically the name of serverIP!
        write(PCfd,new_line,1);
        write(PCfd,serverIP_,strlen(serverIP_));
        
        //Write character which will indicate that we have next info and specifically the name of serverIP!
        write(PCfd,new_line,1);
        write(PCfd,serverPost_,strlen(serverPost_));

        //Write character which will indicate that message is over!
        write(PCfd,last_char,1);
        //Father close named pipe so as to Worker understand that he can read now...
        close(PCfd);
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Father is ready to make requests to Workers.../////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int SUCCESS=0,FAIL=0;
    
    //Sigaction for SIGCHLD signal...
    struct sigaction act;
	memset (&act, 0, sizeof(act)); 
	act.sa_sigaction = sigchildhandler;
 	act.sa_flags = SA_RESTART | SA_SIGINFO;
	if(sigaction(SIGCHLD, &act, NULL) < 0) 
	{
		perror("Father SIGCHLD ERROR!");
		return 1;
	}
    
    //Sigaction for SIGINT signal...
    struct sigaction act1;
	memset (&act1, 0, sizeof(act1)); 
	act1.sa_sigaction = sigintorquithandler;
 	act1.sa_flags = SA_SIGINFO;
	if(sigaction(SIGINT, &act1, NULL) < 0) 
	{
		perror("Father SIGINT ERROR!");
		return 1;
	}

    //Sigaction for SIGQUIT signal...
    struct sigaction act2;
	memset (&act2, 0, sizeof(act2)); 
	act2.sa_sigaction = sigintorquithandler;
 	act2.sa_flags = SA_SIGINFO;
    if(sigaction(SIGQUIT, &act2, NULL) < 0) 
	{
		perror("Father SIGQUIT ERROR!");
		return 1;
	}

    string input;
    while(1) 
    {           
        //Case: Father got SIGINT or SIGQUIT... 
        if(sigintorquit)
        {
            //Father is waiting for Workers...
            pid_t wpid;
            int status;
            while((wpid = wait(&status)) > 0);
            cout << endl << "Master waited Workers to die and now he dies due to SIGINT signal!" << endl;
            break;
        }
    }
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////End of program,after is deallocation of memory//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Delete all named pipes.
    for(i=0;i<num_of_forks;i++)
    {
        if(remove(info_array[i]->PCfifo) != 0) perror("Cannot remove named pipe!\n");
    }

    //Deallocation of memory...
    for(i=0;i<num_of_forks;i++)
    {
        for(k=0;k<info_array[i]->num_of_dirs;k++)
        {
            //For each country name.
            delete[] info_array[i]->dirs[k];
        }
        //For each array of countries(dirs).
        delete[] info_array[i]->dirs;
        //For each Worker_Info.
        delete info_array[i];
    }
    delete[] info_array;
    for(i=0;i<countries;i++)    delete[] countries_array[i];
    delete[] countries_array;
    delete[] path;
    delete[] serverIP_;
    delete[] serverPost_;

    return 0;
}
