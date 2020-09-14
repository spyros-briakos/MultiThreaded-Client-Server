#include "../headers/bucket_list.h"
#include "../headers/max_heap.h"

extern int still_patients;
extern int suspects;

using namespace std;

BucketNode* CreateBucketNode(int bucket_size)
{
    BucketNode* ptr = new BucketNode(bucket_size);
    return ptr;
}

void DeleteBucketNode(BucketNode* temp,int bucket_size)
{   
    if(temp==NULL) return;
    //Deallocate memory for each Bucket_Entry*...
    for(int j=0;j<((bucket_size-12)/8);j++) 
        if(temp->array[j]!=NULL) delete(temp->array[j]);
    //Deallocate memory for array which is made up of Bucket_Entry*... 
    if(temp->array!=NULL) delete [] temp->array;
    //Deallocate memory of BucketNode
    delete temp;
}

void InsertBucketEntry(BucketNode* head, string name, string date, int bucket_size, RecordNode* record)
{
    int max_entries = (bucket_size-12)/8;
    BucketNode* temp = head;
    
    //Traverse BucketNodes...so as to reach last BucketNode.
    while(temp->next!=NULL)
    {
        for(int j=0;j<max_entries;j++)
        {
            //Case: Found Bucket Entry with same name (country or disease)
            if(temp->array[j]->name==name)
            {
                temp->array[j]->root = BST_Insert(temp->array[j]->root,date,record); //Insert our date into BST.
                return;
            }
        }    
        temp = temp->next; //Go to next Bucket Node.
    }

    //Case: We have some Bucket Entries inside last Bucket Node, but it isn't full.
    if(temp->counter<max_entries)
    {
        for(int j=0;j<temp->counter;j++)
        {
            //Case: Found Bucket Entry with same name (country or disease).
            if(temp->array[j]->name==name)
            {
                temp->array[j]->root = BST_Insert(temp->array[j]->root,date,record); //Insert our date into BST.
                return;
            }
        }
        //Didn't found same name so this is a new Bucket Entry.
        temp->array[temp->counter] = new BucketEntry(name); //Allocate memory for new Bucket Entry
        temp->array[temp->counter]->root = BST_Insert(temp->array[temp->counter]->root,date,record); //Insert our date into BST.
        (temp->counter)++;
    }
    //Case: head->counter==max_entries, so last Bucket Node is full.
    else
    {
        for(int j=0;j<max_entries;j++)
        {
            //Case: Found Bucket Entry with same name (country or disease)
            if(temp->array[j]->name==name)
            {
                temp->array[j]->root = BST_Insert(temp->array[j]->root,date,record); //Insert our date into BST.
                return;
            }
        }
        //Case: Last BucketNode is full and didn't found Bucket Entry with same name, so we have to allocate a new one.
        BucketNode* new_bucket = CreateBucketNode(bucket_size);
        new_bucket->array[0] = new BucketEntry(name); //Allocate memory for new Bucket Entry
        new_bucket->array[0]->root = BST_Insert(new_bucket->array[0]->root,date,record); //Insert our date into BST.
        new_bucket->counter=1;
        temp->next = new_bucket; //Link last Bucket Node with new one.
    }
}

//Length of BList
int BL_Length(BucketNode* head)
{
    int length = 0;
    while(head!=NULL)
    {
        length++;
        head = head->next;
    }
    return length;
}

//Print bucket_list's BucketNodes...
void BL_Print(BucketNode* head,bool with_tree)
{
    while(head!=NULL)
    {
        //cout << "Num of Bucket Entries: " << head->counter << endl;
        for(int i=0;i<head->counter;i++)
        {
            cout << head->array[i]->name << "\n"; 
            if(with_tree)
            {
                inOrder(head->array[i]->root);
                cout << endl;
            }
        }
        head = head->next;
    }
}

void Print_Hash_Tables(int disease_entries, int country_entries, BucketNode** diseaseHashTable, BucketNode** countryHashTable,bool with_tree)
{
    cout << "\n/////Disease's Buckets/////////\n";
    for(int j=0;j<disease_entries;j++) 
    {
        if(BL_Length(diseaseHashTable[j])!=0)
        {
            cout << "---------------------\nBucket " << j << " has " << BL_Length(diseaseHashTable[j]) << " BNodes" << endl;
            if(with_tree) BL_Print(diseaseHashTable[j],true);
            else BL_Print(diseaseHashTable[j],false);
        }
    }
    cout << "---------------------\n///////////////////////////////\n";

    cout << "\n/////Country's Buckets/////////\n";
    for(int j=0;j<country_entries;j++) 
    {
        if(BL_Length(countryHashTable[j])!=0)
        {
            cout << "---------------------\nBucket " << j << " has " << BL_Length(countryHashTable[j]) << " BNodes" << endl;
            if(with_tree) BL_Print(countryHashTable[j],true);
            else BL_Print(countryHashTable[j],false);
        }
    }
    cout << "---------------------\n///////////////////////////////\n";
}

void Destroy_BList(BucketNode** head)
{
    BucketNode* temp = *head;
    BucketNode* next;

    while(temp!=NULL)
    {
        next = temp->next;
        for(int j=0;j<temp->counter;j++)
        {
            //Deallocate memory of AVL tree.
            BST_Destroy(temp->array[j]->root); 
            //Deallocate memory for BucketEntry*.
            delete temp->array[j]; 
        }
        //Deallocate memory for array of BucketEntries*.
        delete [] temp->array; 
        //Deallocate memory for BucketNode.
        delete(temp); 
        temp = next;
    }
    //Head of list point to nothing.
    *head = NULL;
}

void FindBucketEntryOfdID(BucketNode* head, string name)
{
    while(head!=NULL)
    {
        for(int j=0;j<head->counter;j++)
        {
            if(head->array[j]->name==name) 
            {
                still_patients=0;
                BST_Check_For_Exit_Date(head->array[j]->root);
                if(head->array[j]->root!=NULL) cout << name << ": " << still_patients << endl;
                return;
            }
        }
        head = head->next;
    }
    cout << "Didn't found this disease into our buckets" << endl;
    still_patients=0;
}

void PrintBucketEntriesOfdID(BucketNode* head)
{
    while(head!=NULL)
    {
        for(int j=0;j<head->counter;j++) 
        {
            still_patients=0;
            BST_Print_Still_Patients(head->array[j]->root);
            cout << head->array[j]->name << ": " << still_patients << endl;
        }
        head = head->next;
    }
}

void GlobalDiseaseStats(BucketNode** hashtable,int disease_entries,string date1,string date2)
{
    BucketNode* temp;
    for(int j=0;j<disease_entries;j++)
    {
        temp=hashtable[j];
        while(temp!=NULL)
        {
            for(int j=0;j<temp->counter;j++)
            {
                suspects=0;
                //Case: We search from date1...date2.
                if(date1!="empty" && date2!="empty")
                {
                    CountSuspectsWithRestriction(temp->array[j]->root,date1,date2);
                    cout << temp->array[j]->name << " " << suspects << endl;
                }
                //Case: No date restriction
                else
                {
                    CountSuspectsNoRestriction(temp->array[j]->root);
                    cout << temp->array[j]->name << " " << suspects << endl;
                }
            }
            temp = temp->next;
        }
    }
    suspects=0;
}

void DiseaseFrequency(BucketNode** hashtable,int disease_entries,string disease,string date1,string date2,string country,string* disease_message)
{
    BucketNode* temp;
    for(int j=0;j<disease_entries;j++)
    {
        temp=hashtable[j];
        while(temp!=NULL)
        {
            for(int j=0;j<temp->counter;j++)
            {
                if(temp->array[j]->name==disease)
                {
                    suspects=0;
                    //Case: We search from date1...date2.
                    if(country=="empty")
                    {
                        CountSuspectsWithRestriction(temp->array[j]->root,date1,date2);
                    }
                    //Case: We search from date1...date2, with country's filter
                    else
                    {
                        CountSuspectsWithDoubleRestriction(temp->array[j]->root,date1,date2,country);
                    }
                    (*disease_message).append(to_string(suspects));
                    return;
                }
            }
            temp = temp->next;
        }
    }
    suspects=0;
    (*disease_message).append("ERROR");
}

void TopkDiseases(BucketNode** hashtable,int disease_entries,string country,string date1,string date2,int k)
{
    BucketNode* temp;
    Max_Heap* p = new Max_Heap(); //Create an empty Binary Heap.
    
    for(int j=0;j<disease_entries;j++)
    {
        temp = hashtable[j];

        while(temp!=NULL)
        {
            for(int j=0;j<temp->counter;j++)
            {
                suspects = 0;
                
                //The only restriction is country.
                if(date1=="empty" && date2=="empty")
                {
                    NumOfPatientsOfaDisease(temp->array[j]->root,country);
                }
                //Case: We have date restriction.
                else
                {
                    CountSuspectsWithDoubleRestriction(temp->array[j]->root,date1,date2,country);
                }  

                p->Insert(temp->array[j]->name,suspects);
            }
            temp = temp->next;
        }
    }
    p->Topk(k);
    //Deallocate memory for on the fly binary heap.
    p->Delete(p->root);
    delete p;
}

void TopkCountries(BucketNode** countryhashtable,int country_entries,string virus,string date1,string date2,int k)
{
    BucketNode* temp;
    Max_Heap* p = new Max_Heap(); //Create an empty Binary Heap.
    
    for(int j=0;j<country_entries;j++)
    {
        temp = countryhashtable[j];
        
        while(temp!=NULL)
        {
            for(int j=0;j<temp->counter;j++)
            {
                suspects = 0;

                //The only restriction is virus.
                if(date1=="empty" && date2=="empty")
                {
                    NumOfPatientsOfaCountry(temp->array[j]->root,virus);
                }
                //Case: We have date restriction.
                else
                {
                    CountSuspectsForaDisease(temp->array[j]->root,date1,date2,virus);
                }  

                p->Insert(temp->array[j]->name,suspects);
            }
            temp = temp->next;
        }
    }
    p->Topk(k);
    //Deallocate memory for on the fly binary heap.
    p->Delete(p->root);
    delete p;
}

string TopkAgeRanges(BucketNode** countryhashtable,int country_entries,string country,string virus,string date1,string date2,int k)
{
    BucketNode* temp;
    Max_Heap* p = new Max_Heap(); //Create an empty Binary Heap.
    
    for(int j=0;j<country_entries;j++)
    {
        temp = countryhashtable[j];
        
        while(temp!=NULL)
        {
            for(int j=0;j<temp->counter;j++)
            {
                if(temp->array[j]->name==country)
                {
                    SS_Node* head = NULL;
                    CalculateAgeRanges(temp->array[j]->root,date1,date2,virus,&head);
                
                    //Didn't found the specific disease.
                    if(head==NULL)  
                    {
                        delete p;
                        return "ERROR";
                    }

                    //Calculate the percentages and insert them into binary heap.
                    float f1 = (float)(head->agecategory1)/(float)(head->agecategory1+head->agecategory2+head->agecategory3+head->agecategory4);
                    float f2 = (float)(head->agecategory2)/(float)(head->agecategory1+head->agecategory2+head->agecategory3+head->agecategory4);
                    float f3 = (float)(head->agecategory3)/(float)(head->agecategory1+head->agecategory2+head->agecategory3+head->agecategory4);
                    float f4 = (float)(head->agecategory4)/(float)(head->agecategory1+head->agecategory2+head->agecategory3+head->agecategory4);
                    p->Insert("0-20:",f1);
                    p->Insert("21-40:",f2);
                    p->Insert("41-60:",f3);
                    p->Insert("60+:",f4);

                    string topk = p->Topk(k);
                    p->Delete(p->root);
                    SS_Delete_List(head);
                    delete p;
                    return topk;
                }
            }
            temp = temp->next;
        }
    }
    delete p;
    return "ERROR";
}

string NumPatientsAdmissionsDischarges(BucketNode** hashtable,int disease_entries,string disease,string date1,string date2,string country,bool on_off)
{ 
    BucketNode* temp;
    for(int j=0;j<disease_entries;j++)
    {
        temp=hashtable[j];
        while(temp!=NULL)
        {
            for(int j=0;j<temp->counter;j++)
            {
                if(temp->array[j]->name==disease)
                {
                    string info;
                    
                    //Case: NumPatientAdmissions
                    if(on_off)
                    {
                        //Case: No country filter...
                        if(country=="empty")
                        {
                            SS_Node* head = NULL,*current;
                            CountSuspectsOfDifferentCountries(temp->array[j]->root,date1,date2,&head);
                            
                            if(head==NULL)  return "ERROR";
                            current=head;
                            while(current!=NULL)
                            {
                                info.append(current->name);                            
                                info.append(" ");                            
                                info.append(to_string(current->agecategory1));
                                info.append("\n");
                                current = current->next;
                            }
                            SS_Delete_List(head);
                        }
                        //Case: We have country filter...
                        else
                        {
                            suspects=0;
                            CountSuspectsWithDoubleRestriction(temp->array[j]->root,date1,date2,country);
                            info.append(country);                            
                            info.append(" ");                            
                            info.append(to_string(suspects));                            
                        }
                    }
                    //Case: NumPatientDischarges
                    else
                    {
                        //Case: No country filter...
                        if(country=="empty")
                        {
                            SS_Node* head = NULL,*current;
                            CountDischargesOfDifferentCountries(temp->array[j]->root,date1,date2,&head);
                            
                            if(head==NULL)  return "ERROR";
                            current=head;
                            while(current!=NULL)
                            {
                                info.append(current->name);                            
                                info.append(" ");                            
                                info.append(to_string(current->agecategory1));
                                info.append("\n");
                                current = current->next;
                            }
                            SS_Delete_List(head);
                        }
                        //Case: We have country filter...
                        else
                        {
                            suspects=0;
                            CountDischargesWithDoubleRestriction(temp->array[j]->root,date1,date2,country);
                            info.append(country);                            
                            info.append(" ");                            
                            info.append(to_string(suspects));                            
                        }
                    }
                    return info;
                }
            }
            temp = temp->next;
        }
    }
    return "ERROR";
}  


//////////////////////////Hash Functions/////////////////////////////////

int hash_function(string s, int M) 
{
    int intLength = s.length() / 4;
    long sum = 0;
    for (int j = 0; j < intLength; j++) {
    string c = s.substr(j * 4, (j * 4) + 4).c_str();
    long mult = 1;
    for (int k = 0; k < c.length(); k++) 
    {
        sum += c[k] * mult;
        mult *= 256;
    }
    }

    string c = s.substr(intLength * 4).c_str();
    long mult = 1;
    for (int k = 0; k < c.length(); k++) 
    {
        sum += c[k] * mult;
        mult *= 256;
    }

    return(abs(sum) % M);
}

int hash_function2(string x, int M) 
{
    int i, sum;
    for (sum=0, i=0; i < x.length(); i++) sum += x[i];
    return sum % M;
}

unsigned long hash_function3(string str, int M)
{
    unsigned long hash = 5381;

    for(int i=0;i<str.size();i++)
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */

    return hash%M;
}
