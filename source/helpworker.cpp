#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <cstdlib>
#include <fcntl.h>   
#include <unistd.h> 
#include <sys/wait.h>
#include <dirent.h> 
#include <fstream>
#include <signal.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include "../headers/helpworker.h"

using namespace std;

extern volatile sig_atomic_t sigintorquit;
extern volatile sig_atomic_t sigusr1;

void sigintorquithandler(int sig, siginfo_t *siginfo, void *context)
{
	sigintorquit=true;
}

void sigusr1handler(int sig, siginfo_t *siginfo, void *context)
{
	sigusr1=true;
}

//Function, which fills significat params of struct sockaddr_in.
int bind_on_port(struct sockaddr_in* server,int sock ,short port, const char* serverIP) 
{
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(serverIP);
    server->sin_port = htons(port);
    return bind(sock,(struct sockaddr*)server,sizeof(*server));
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

void WorkerHasNewDateFiles(int num_of_dirs,char*** countries_array,char* input_dir,string** latest_dates,RecordNode** head,int diseases,BucketNode** diseaseHashTable,BucketNode** countryHashTable)
{
    char path_to_dir[30],path_to_date_file[50];
    strcpy(path_to_dir,"");strcpy(path_to_date_file,"");
    int d=0,i=0,j=0,z=0,q=0,new_date_files=0,count=0,err=0,disease_bucket=0,country_bucket=0,bucket_size=20,num_of_all_records=0;
    DIR *dir;
    struct dirent *entry;
    fstream newfile;
    string line,parameters[8];

    //Firstly calculate number of new date files.
    for(i=0;i<num_of_dirs;i++)
    {
        strcpy(path_to_dir,input_dir);
        strncat(path_to_dir,"/",1);
        strncat(path_to_dir,(*(countries_array))[i],strlen((*(countries_array))[i]));
        dir = opendir(path_to_dir);
        if(dir==0)  cout << "The parameter input_dir that you gave doesn't exist.Try again!" << endl;
        while ((entry = readdir(dir)) != NULL)  
        {
            if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)  
            {
                string current_date_file(entry->d_name);  
                
                //Check if this date file is the new one.
                if(Compare(current_date_file,(*(latest_dates))[i])==1)
                {
                    //Increase num of new date files and update maximum date into this dir.
                    new_date_files++;

                    strcpy(path_to_date_file,path_to_dir);
                    strncat(path_to_date_file,"/",1);
                    strncat(path_to_date_file,entry->d_name,strlen(entry->d_name));
                    newfile.open(path_to_date_file,ios::in); 
                    if(!newfile.is_open())  cout << "Cannot open date file " << path_to_date_file << endl;

                    while(getline(newfile,line))    num_of_all_records++;    
                    
                    newfile.close();
                }
            } 
        }
        closedir(dir);
    }

    string* date_files = new string[new_date_files];
    string* countries_of_new_date_files = new string[new_date_files];
    string** records_array = new string*[num_of_all_records];

    //Loop through directories so as to store names of new date files.
    for(i=0;i<num_of_dirs;i++)
    {
        //Prepare path_to_dir so as to open each subdir...
        strcpy(path_to_dir,input_dir);
        strncat(path_to_dir,"/",1);
        strncat(path_to_dir,(*(countries_array))[i],strlen((*(countries_array))[i]));
        
        //Firstly store names of date files so as to sort them.
        //Open each subdirectory of Worker.
        dir = opendir(path_to_dir);
        if(dir==0)  cout << "The parameter input_dir that you gave doesn't exist.Try again!" << endl;
        //Take each date file of subdirectory of Worker.
        while((entry = readdir(dir)) != NULL)  
        {
            if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)
            {
                string current_date_file(entry->d_name);
                if(Compare(current_date_file,(*(latest_dates))[i])==1)
                {
                    //Store new date file's name and in which dir belongs to.
                    date_files[count]=entry->d_name;
                    countries_of_new_date_files[count]=(*(countries_array))[i];
                    count++;
                }
            }
        }    
        //Close each subdirectory of Worker.
        closedir(dir);
    }  
    
    //Update latest date of each dir with new date file.
    for(i=0;i<new_date_files;i++)
    {
        for(d=0;d<num_of_dirs;d++)
        {
            if(countries_of_new_date_files[i]==(*(countries_array))[d]) break;
        }

        if(Compare(date_files[i],(*(latest_dates))[d])==1)
        {
            (*(latest_dates))[d] = date_files[i];
        }
    }

    //Now Worker is ready to open date files with ascending order.
    for(i=0;i<new_date_files;i++)
    {
        //Prepare path_to_date to give is as argument to fstream.open().
        strcpy(path_to_dir,input_dir);
        strncat(path_to_dir,"/",1);
        strncat(path_to_dir,(countries_of_new_date_files[i]).c_str(),strlen((countries_of_new_date_files[i]).c_str()));
        strcpy(path_to_date_file,path_to_dir);
        strncat(path_to_date_file,"/",1);
        strncat(path_to_date_file,date_files[i].c_str(),strlen(date_files[i].c_str()));
    
        //Open each date file of this subdirectory and take its contents,records.
        newfile.open(path_to_date_file,ios::in); 
        if(!newfile.is_open())  cout << "Cannot open date file " << path_to_date_file << endl;

        //Get line by line each record and store temporarily each contents to array of strings,parameters[7].
        while(getline(newfile, line))
        { 
            stringstream iss(line);j=0;
            while (iss >> parameters[j])    j++;

            //Case: Record has exactly 6 words so it has the right format.
            if(j==6)
            {
                //Allocate memory for array of 8 strings so as to put inside contents of a full record.
                records_array[q] = new string[8];
                
                //Initialization of contents of record to records_array.
                records_array[q][0] = parameters[0]; //rID
                records_array[q][1] = parameters[2]; //first name
                records_array[q][2] = parameters[3]; //last name
                records_array[q][3] = parameters[4]; //disease
                records_array[q][4] = countries_of_new_date_files[i]; //country
                records_array[q][5] = parameters[5]; //age
            
                //Case: ENTER record
                if(parameters[1]=="ENTER")
                {
                    records_array[q][6] = date_files[i]; //entry date
                    records_array[q][7] = "--"; //exit date
                }
                //Case: EXIT record
                else if(parameters[1]=="EXIT")
                {
                    records_array[q][6] = "--"; //entry date
                    records_array[q][7] = date_files[i]; //exit date
                }
                //Case: Error so fill record with "--".
                else
                {
                    records_array[q][0] = "--";
                    cerr << "ERROR" << endl;
                }
            }
            //Case: This half-Record hasn't the appropriate number of parameters,so reject it.
            else
            {
                cerr << "ERROR" << endl;
            }
            q++;
        }
        //Close each date file so as to continue with next.
        newfile.close(); 
    } 
    
    //Now Worker is ready to manipulate half records who holds them in records_array so as to create a full record...
    for(q=0;q<num_of_all_records;q++)
    {
        //Case: ENTER record.
        if(records_array[q][6]!="--")
        {
            //Loop through array to find EXIT record with same rID.
            for(j=0;j<num_of_all_records;j++)
            {
                //If ENTER record and EXIT record have the rID continue to update exit_date of our full record.
                if(records_array[q][0]==records_array[j][0] && records_array[j][7]!="--" && records_array[j][0]!="--")
                {
                    records_array[q][7]=records_array[j][7];
                    records_array[j][0]="--";
                    break;
                }
            }
        }
    }
    
    //So here we will take from records_array the active full records and insert them to our structures. 
    for(q=0;q<num_of_all_records;q++)
    {
        //If record is active...
        if(records_array[q][0]!="--")
        {
            //Check if Record is Valid.
            err = Is_Valid(head,records_array[q][0],records_array[q][1],records_array[q][2],records_array[q][3],records_array[q][4],records_array[q][5],records_array[q][6],records_array[q][7]);

            //Case: Found a problem so reject the record.
            if(CALL_ERROR(err)==1)
            {
                cerr << "ERROR" << endl;
            } 
            //Record is Valid, so keep on...
            else
            {
                RecordNode* record = RL_Insert(head,records_array[q][0],records_array[q][1],records_array[q][2],records_array[q][3],records_array[q][4],records_array[q][5],records_array[q][6],records_array[q][7]);

                disease_bucket = hash_function2(records_array[q][3],diseases);
                country_bucket = hash_function2(records_array[q][4],num_of_dirs);

                //For diseases...
                //First time in this bucket.
                if(diseaseHashTable[disease_bucket]==NULL)
                {    
                    diseaseHashTable[disease_bucket] = CreateBucketNode(bucket_size);
                    InsertBucketEntry(diseaseHashTable[disease_bucket],records_array[q][3],records_array[q][6],bucket_size,record);
                }
                //Case: We have already some entries in this bucket...
                else
                {
                    InsertBucketEntry(diseaseHashTable[disease_bucket],records_array[q][3],records_array[q][6],bucket_size,record);
                }
                
                //For countries...
                //First time in this bucket.
                if(countryHashTable[country_bucket]==NULL)
                {    
                    countryHashTable[country_bucket] = CreateBucketNode(bucket_size);
                    InsertBucketEntry(countryHashTable[country_bucket],records_array[q][4],records_array[q][6],bucket_size,record);
                }
                //Case: We have already some entries in this bucket...
                else
                {
                    InsertBucketEntry(countryHashTable[country_bucket],records_array[q][4],records_array[q][6],bucket_size,record);
                }  
            }
        }
    }
   
    //Deallocate memory for string arrays
    for(z=0;z<num_of_all_records;z++)  delete[] records_array[z];
    delete[] records_array;
    delete[] date_files;
    delete[] countries_of_new_date_files;
}

void Worker_Write(char* CPfifo,string message,int buffersize)
{
    char sum_stats[message.size()+1];
    strcpy(sum_stats,message.c_str());
    sum_stats[message.size()] = '\0';
    int last_bytes=(strlen(sum_stats)+1)%buffersize,CPfd,k;
    int chunk_times=(strlen(sum_stats)+1)/buffersize;

    //Worker opens CPfifo in order to write sum_stats.
    if((CPfd = open(CPfifo, O_WRONLY))<0)   perror("Worker cannot open CPfifo!");
    
    //Case: Send chunk_times same-bytes messages.
    if(last_bytes==0)
    {
        for(k=0;k<chunk_times;k++)  write(CPfd,&sum_stats[k*buffersize],buffersize);
    }
    //Case: Send chunk_times-1 same-bytes messages and last message with last_bytes.
    else
    {
        for(k=0;k<=chunk_times;k++)
        {
            if(k==chunk_times)  write(CPfd,&sum_stats[k*buffersize],last_bytes);
            else    write(CPfd,&sum_stats[k*buffersize],buffersize); 
        }
    }
    
    //Worker closes CPfifo.
    close(CPfd);
}


string Worker_Read(char* PCfifo,int buffersize)
{
    char buff[buffersize];buff[buffersize]='\0';strcpy(buff,"");int PCfd;string message="";
    
    //Worker opens named pipe so as to read.
    if((PCfd = open(PCfifo, O_RDONLY ))<0)   
    {
        if(!sigintorquit)   perror("Worker cannot open PCfifo!");
        //Case: Worker received SIGINT or SIGQUIT so just return an empty string.
        else    return "";
    }

    //Worker reads buffersize bytes each time so as to collect summary stats and print them.
    while((read(PCfd,buff,buffersize))>0)
    {
        string buffer(buff);
        strcpy(buff,"");        
        message.append(buffer);
    }

    //Workes closes CP_pipe.
    close(PCfd);

    //Return what Worker read.
    return message;
}

void insert(Node** head, string disease)  
{  
    Node *new_node,*temp = *head;
    
    if(*head == NULL)  
    {  
        new_node = new Node(disease); 
        *head = new_node;  
        return;  
    }  
    
    while(temp->next != NULL)  
    {
        if(temp->name==disease) return;
        temp = temp->next;  
    }

    if(temp->name==disease) return;

    new_node = new Node(disease); 
    temp->next = new_node;  
}  

void delete_list(Node* head)
{
    Node* temp = head;
    Node* next;

    while(temp!=NULL)
    {
        next = temp->next;
        delete(temp); 
        temp = next;
    }
    head = NULL;
}

void print_list(Node* head)
{
    Node* temp=head;
    while(temp!=NULL)
    {
        cout << temp->name << endl;
        temp = temp->next;
    }
}

int length(Node* head)
{
    Node* temp=head;
    int length=0;
    while(temp!=NULL)
    {
        length++;
        temp = temp->next;
    }
    return length;
}

void SS_Insert(SS_Node** head,string disease,int age)
{
    SS_Node *new_node,*temp = *head;
    
    if(*head == NULL)  
    {  
        new_node = new SS_Node(disease);
        if(age<=20) new_node->agecategory1++; 
        if(age>=21 && age<=40) new_node->agecategory2++; 
        if(age>=41 && age<=60) new_node->agecategory3++; 
        if(age>=61) new_node->agecategory4++; 
        *head = new_node;  
        return;  
    }  
    
    while(temp->next != NULL)  
    {
        if(temp->name==disease) 
        {
            if(age<=20) temp->agecategory1++; 
            if(age>=21 && age<=40) temp->agecategory2++; 
            if(age>=41 && age<=60) temp->agecategory3++; 
            if(age>=61) temp->agecategory4++; 
            return;
        }
        temp = temp->next;  
    }

    if(temp->name==disease) 
    {
        if(age<=20) temp->agecategory1++; 
        if(age>=21 && age<=40) temp->agecategory2++; 
        if(age>=41 && age<=60) temp->agecategory3++; 
        if(age>=61) temp->agecategory4++; 
        return;
    }

    new_node = new SS_Node(disease); 
    if(age<=20) new_node->agecategory1++; 
    if(age>=21 && age<=40) new_node->agecategory2++; 
    if(age>=41 && age<=60) new_node->agecategory3++; 
    if(age>=61) new_node->agecategory4++; 
    temp->next = new_node;  
}

void SS_Delete_List(SS_Node* head)
{
    SS_Node* temp = head;
    SS_Node* next;

    while(temp!=NULL)
    {
        next = temp->next;
        delete(temp); 
        temp = next;
    }
    head = NULL;
}

void SS_Print_List(SS_Node* head)
{
    SS_Node* temp=head;
    while(temp!=NULL)
    {
        cout << temp->name << endl << temp->agecategory1 << endl << temp->agecategory2 << endl << temp->agecategory3 << endl << temp->agecategory4 << endl;
        temp = temp->next;
    }
}

int SS_Length(SS_Node* head)
{
    SS_Node* temp=head;
    int length=0;
    while(temp!=NULL)
    {
        length++;
        temp = temp->next;
    }
    return length;
}

void swap(string *xp, string *yp) 
{ 
	string temp = *xp; 
	*xp = *yp; 
	*yp = temp; 
} 

void selectionsort(string* arr, int n) 
{ 
	int i, j, min_idx; 

	for (i = 0; i < n-1; i++) 
	{ 
		min_idx = i; 
		for (j = i+1; j < n; j++) 
		if (Compare(arr[j],arr[min_idx])==-1) 
			min_idx = j; 

		swap(&arr[min_idx], &arr[i]); 
	} 
} 

void printArray(string arr[], int size) 
{ 
	int i; 
	for (i=0; i < size; i++) 
		cout << arr[i] << " "; 
	cout << endl; 
} 

// int main() 
// { 
// 	string arr[] = {"10-10-2020", "10-10-2021", "10-10-2022", "10-10-2023", "10-10-2024"}; 
// 	int n = sizeof(arr)/sizeof(arr[0]); 
// 	selectionSort(arr, n); 
// 	printArray(arr, n); 
// 	return 0; 
// } 

// int main(void)
// {
//     SS_Node* head = NULL;
//     for(int i=0;i<2000;i++)
//     {
//         SS_Insert(&head,"COVID-2019",61);
//         // SS_Insert(&head,"COVID-2019",25);
//         // SS_Insert(&head,"COVID-2019",50);
//         // SS_Insert(&head,"COVID-2019",81);
//     }

//     SS_Print_List(head);
//     cout << "Size: " << SS_Length(head) << endl;
//     SS_Delete_List(head);
// }
 