#include <iostream>
#include <string>
#include "../headers/record_list.h"
#include "../headers/binary_tree.h"

using namespace std;

int Check_Dates(string entry_date,string exit_date)
{
    if(Check_Date_Format(entry_date,"entry")==-1 || Check_Date_Format(exit_date,"exit")==-1) return -1;
    if(Compare(entry_date,exit_date)==1) return -1;
    return 0; //Dates are just fine.
}

int Check_Date_Format(string date,string kindofdate)
{
    if(kindofdate=="exit" && date=="--") return 0;
    
    if(date.size()!=10) return INVALID_DATE; //Error -1: Invalid Date, Date's Format: DD-YY-RRRR 
    for(int i=0;i<date.size();i++)
        if(i!=2 && i!=5)
            if(!isdigit(date[i])) 
                return INVALID_DATE; //Error -1: Invalid Date

    if((date.substr(2,1) != "-") || (date.substr(5,1) != "-")) return INVALID_DATE; //Error -1: Invalid Date

    int day = stoi(date.substr(0,2));
    int month = stoi(date.substr(3,2));
    int year = stoi(date.substr(6,4));
    if(day>31 || day<=0 || month>12 || month<=0 || year>2050 || year<1900 ) return INVALID_DATE; //Error -1: Invalid Date
    
    return 0; //Date is correct
}

int Check_Virus(string virus)
{
    //Check for diseasedID.
    for(int i=0;i<virus.size();i++)
    {
        if(isspace(virus[i])) return DISEASEID_HAS_SPACE; //Error -5: DiseaseID has at least one space.
        if(!((virus[i]>='a' && virus[i]<='z') || (virus[i]>='A' && virus[i]<='Z') || (virus[i]>= '0' && virus[i]<= '9') || (virus[i]=='-')))
            return INVALID_DISEASE; //Error -4: Invalid DiseaseID.
    }
    return 0;
}

int Check_Country(string c)
{
    //Check for country.
    for(int i=0;i<c.size();i++)
    {
        if(isspace(c[i])) return COUNTRY_HAS_SPACE; //Error -3: Country has at least one space.
        if(!isalpha(c[i])) return COUNTRY_HAS_NONALPHABETIC; //Error -2: Country has a character which isn't alphabetic.
    }
    return 0;
}

int Check_Record(string rID)
{
    //Check if rID is a string with letters or digits.
    for(int i=0;i<rID.size();i++)
        if(!isalpha(rID[i]) && !isdigit(rID[i])) return -11;
    
    return 0;
}

int Check_Age(string age)
{
    int _age = stoi(age);
    if(_age<=0 || _age>120) return INVALID_AGE;
    return 0;
}

int Check_k(string k)
{
    if(k.size()==1 && isdigit(k[0]))    return 0;
    else return INVALID_K;
}

//Checking for Validity of our parameters//
int Is_Valid(RecordNode** head,string rID,string pFN,string pLN,string dID,string c,string age,string enD,string exD)
{
    RecordNode* temp = *head;
    
    //Check for rID.
    int err = Check_Record(rID);
    if(err!=0) return err;

    //Check if record exists.
    while(temp!=NULL)
    {
        if(temp->recordID==rID) return RECORD_ALREADY_EXISTS; //Error -10: This record exists.  
        temp = temp->next;
    }
   
    //Check for First Name.
    for(int i=0;i<pFN.size();i++)
    {
        if(isspace(pFN[i])) return FIRST_NAME_HAS_SPACE; //Error -9: First Name has at least one space.
        if(!isalpha(pFN[i])) return FIRST_NAME_HAS_NONALPHABETIC; //Error -7: First Name has a character which isn't alphabetic.
    }

    //Check for Last Name.
    for(int i=0;i<pLN.size();i++)
    {
        if(isspace(pLN[i])) return LAST_NAME_HAS_SPACE; //Error -8: Last Name has at least one space.
        if(!isalpha(pLN[i])) return LAST_NAME_HAS_NONALPHABETIC; //Error -6: Last Name has a character which isn't alphabetic.
    }
    
    err = Check_Age(age);
    if(err!=0) return err;

    err = Check_Virus(dID);
    if(err!=0) return err; 
    
    err = Check_Country(c);
    if(err!=0) return err; 
        
    //Check for entry and exit date.
    err = Check_Dates(enD,exD);
    if(err!=0) return err;

    return 0; //Everything was fine.
}

//Ιnsert our record into list...
RecordNode* RL_Insert(RecordNode** head,string rID,string pFN,string pLN,string dID,string c,string age,string enD,string exD)
{
    // if(exD=="") exD="-";
    RecordNode* node = new RecordNode(rID,pFN,pLN,dID,c,age,enD,exD);
    RecordNode* temp = *head;

    if(*head==NULL) 
    {
        *head = node;
    }
    else 
    {
        while(temp->next!=NULL) temp = temp->next;
        temp->next=node;
    }
    return node;
}

//Print list's RecordNodes...
void RL_Print(RecordNode* head)
{
    while(head!=NULL)
    {
        cout << head->recordID << " " << head->patientFirstName << " " << head->patientLastName << " " << head->diseaseID << " " << head->country << " " << head->age << " " << head->entryDate << " " << head->exitDate << "\n"; 
        head = head->next;
    }
}

void Put_Record_Exit(RecordNode* head,string rID,string exDate)
{
    if(CALL_ERROR(Check_Record(rID))!=0) return;
 
    while(head!=NULL)
    {
        if(head->recordID==rID) 
        {
            if(Check_Date_Format(exDate,"exit")!=0) 
            {
                cout << "Invalid Exit Date!\n"; 
                return;
            }
            if(Compare(head->entryDate,exDate)==1) 
            {
                cout << "Entry date is greater than Exit date, try again!" << endl;
                return;
            }

            head->exitDate = exDate;
            // cout << "Exit date of Record " << rID << " is updated!" << endl;       
            cout << "Record updated" << endl;       
            return;
        }
        head = head->next;
    }
    cout << "Couldn't update exit date of Record " << rID << " because it doesn't exist!" << endl;
}

//Length of RList
int RL_Length(RecordNode* head)
{
    int length = 0;
    while(head!=NULL)
    {
        length++;
        head = head->next;
    }
    return length;
}

//Function for printing any kind of error in RL_Insert...
void Print_Errors(int error)
{
    switch (error)
    {
        case -12:
            cout << "Age should be between (0,120]!\n";
            break;

        case -11:
            cout << "RecordID should be a string!\n";
            break;
        
        case -10:
            cout << "Record already exists!\n";
            break;
        
        case -9:
            cout << "First name has at least one space character!\n";
            break;
        
        case -8:
            cout << "Last name has at least one space character!\n";
            break;
        
        case -7:
            cout << "First name has at least one nonalphabetic character!\n";
            break;
        
        case -6:
            cout << "Last name has at least one nonalphabetic character!\n";
            break;
        
        case -5:
            cout << "DiseaseID has at least one space character!\n";
            break;
        
        case -4:
            cout << "Invalid Input of DiseaseID!\n";
            break;
        
        case -3:
            cout << "Country has at least one space character!\n";
            break;
        
        case -2:
            cout << "Country has at least one nonalphabetic character!\n";
            break;
        
        case -1:
            cout << "Invalid Input of Date!\n";
            break;
        
        default:
            break;
    }
}

int CALL_ERROR(int err)
{
    //Case: Something went wrong and Insertion failed.
    if(err!=0)
    {
        // Print_Errors(err);
        // cout << "ERROR" << endl;
        return 1;
    }
    return 0;
}

void Destroy_RList(RecordNode** head)
{
    RecordNode* temp = *head;
    RecordNode* next;

    while(temp!=NULL)
    {
        next = temp->next;
        delete(temp);
        temp = next;
    }
    *head = NULL;
}   


