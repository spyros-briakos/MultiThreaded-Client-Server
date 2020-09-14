#ifndef BUCKET_LIST_H
#define BUCKET_LIST_H

#include <iostream>
#include "binary_tree.h"

using namespace std;

class BucketEntry
{
    public:
        string name;//name of Disease or Country.
        TreeNode* root;//pointer to binary search tree's root.
        
        BucketEntry()
        : name("NULL"),root(NULL)
        {}
        BucketEntry(string _name)
        : name(_name),root(NULL)
        {}
};

class BucketNode 
{
    public:
        BucketEntry** array; //array of pointers to BucketEntries.
        BucketNode* next; //pointer to next BucketNode.
        unsigned int counter; //how many bucket entries 
        
        BucketNode(int bucket_size)
        : next(NULL),counter(0)
        {
            array = new BucketEntry*[(bucket_size-12)/8];
            for(int j=0;j<((bucket_size-12)/8);j++) array[j]=NULL;
        }
        ~BucketNode()
        {
            array=NULL;
            next=NULL;
        }
};

BucketNode* CreateBucketNode(int );

int BL_Length(BucketNode* );

void DeleteBucketNode(BucketNode* ,int );
void InsertBucketEntry(BucketNode* ,string ,string , int, RecordNode* );
void FindBucketEntryOfdID(BucketNode* ,string );
void PrintBucketEntriesOfdID(BucketNode* );
void BL_Print(BucketNode* ,bool);
void Destroy_BList(BucketNode** );
void Print_Hash_Tables(int , int , BucketNode** , BucketNode** ,bool);
void GlobalDiseaseStats(BucketNode** ,int ,string ,string );
void DiseaseFrequency(BucketNode** ,int ,string ,string ,string ,string,string* );
void TopkDiseases(BucketNode** ,int ,string ,string ,string ,int );
void TopkCountries(BucketNode** ,int ,string ,string ,string ,int );

string TopkAgeRanges(BucketNode** ,int ,string ,string ,string ,string ,int );
string NumPatientsAdmissionsDischarges(BucketNode** ,int ,string ,string ,string ,string ,bool );

/////////Hash Functions////////
int hash_function(string , int );
int hash_function2(string , int ); 
unsigned long hash_function3(string , int );

#endif