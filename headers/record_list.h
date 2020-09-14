#ifndef RECORD_LIST_H
#define RECORD_LIST_H

#include <iostream>
#include <string>

using namespace std;

//Enumeration of errors for printing the appropriate error messages.
enum Errors 
{
    INVALID_K = -13,
    INVALID_AGE,
    RECORD_ISNT_STRING,
    RECORD_ALREADY_EXISTS,
    FIRST_NAME_HAS_SPACE,
    LAST_NAME_HAS_SPACE,
    FIRST_NAME_HAS_NONALPHABETIC,
    LAST_NAME_HAS_NONALPHABETIC,
    DISEASEID_HAS_SPACE,
    INVALID_DISEASE,
    COUNTRY_HAS_SPACE,
    COUNTRY_HAS_NONALPHABETIC,
    INVALID_DATE
};

class RecordNode
{
    public:
        string recordID;
        string patientFirstName;
        string patientLastName;
        string diseaseID;
        string country;
        string age;
        string entryDate;
        string exitDate;
        RecordNode* next;

        RecordNode(string rID,string pFN,string pLN,string dID,string c,string _age,string enD,string exD)
        : recordID(rID),patientFirstName(pFN),patientLastName(pLN),diseaseID(dID),country(c),age(_age),entryDate(enD),exitDate(exD),next(NULL)
        {};
        ~RecordNode()
        {
            next=NULL;
        };
        
};

RecordNode* RL_Insert(RecordNode** ,string ,string ,string ,string ,string ,string ,string ,string );

int Is_Valid(RecordNode** ,string ,string ,string ,string ,string ,string ,string ,string );
int CALL_ERROR(int );
int RL_Length(RecordNode* );
int Check_Dates(string ,string );
int Check_Date_Format(string ,string );
int Check_Country(string );
int Check_Virus(string );
int Check_Age(string );
int Check_Record(string );
int Check_k(string );

void Print_Errors(int );
void RL_Print(RecordNode* );
void Destroy_RList(RecordNode** );
void Put_Record_Exit(RecordNode* ,string, string);


#endif
