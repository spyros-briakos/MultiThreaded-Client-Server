#include <bits/stdc++.h> 
#include "../headers/binary_tree.h"

using namespace std; 

extern int still_patients;
extern int suspects;

//Inline functions
int height(TreeNode* N)  
{  
    if (N != NULL) return N->height;  
    else return 0;  
}  
int max(int a, int b)  
{  
    if(a > b) return a;
    else return b;
}  

//Function which compares two dates and return the appropriate number which symbolizes the right equality or inequality.
int Compare(string date1, string date2)
{   
    if(date2=="--") return -1;
    if(date1=="--") return 1;

    int day1 = stoi(date1.substr(0,2));
    int month1 = stoi(date1.substr(3,2));
    int year1 = stoi(date1.substr(6,4));
    
    int day2 = stoi(date2.substr(0,2));
    int month2 = stoi(date2.substr(3,2));
    int year2 = stoi(date2.substr(6,4));

    if(year1<year2) return -1;
    else if(year1>year2) return 1;
    else //year1==year2
    {
        if(month1<month2) return -1;
        else if(month1>month2) return 1;
        else //month1==month2
        {
            if(day1<day2) return -1;
            else if(day1>day2) return 1;
            else //day1==day2 
                return 0;
        }   
    }
}

void createSDNode(TreeNode* t,RecordNode* record)
{
    SDNode* temp = new SDNode(record);
    if(t->next==NULL) t->next=temp;
    else
    {
        SDNode* r = t->next;
        while(r->next != NULL) r = r->next;
        r->next = temp;
    }
}

TreeNode* createTreeNode(string date,RecordNode* record)  
{  
    TreeNode* temp = new TreeNode(date,record); 
    return temp;  
}  
  
TreeNode* rightRotate(TreeNode* temp)  
{  
    TreeNode *lchild = temp->left;  
    TreeNode *rlchild = lchild->right;  
  
    lchild->right = temp;  
    temp->left = rlchild;  
  
    temp->height = max(height(temp->left),height(temp->right)) + 1;  
    lchild->height = max(height(lchild->left),height(lchild->right)) + 1;  
  
    return lchild;  
}  
  
TreeNode* leftRotate(TreeNode* temp)  
{  
    TreeNode *rchild = temp->right;  
    TreeNode *lrchild = rchild->left;  
  
    rchild->left = temp;  
    temp->right = lrchild;  
  
    temp->height = max(height(temp->left),height(temp->right)) + 1;  
    rchild->height = max(height(rchild->left),height(rchild->right)) + 1;  
  
    return rchild;  
}  
  
//Function which help us understand the situation about children of a specific node.
int getBalance(TreeNode* temp)  
{  
    if (temp != NULL)  
        return height(temp->left) - height(temp->right);  
    else    
        return 0;  
}  

//Insert a TreeNode into AVL Tree...
TreeNode* BST_Insert(TreeNode* temp, string key, RecordNode* record)  
{  
    if (temp == NULL)  
        return(createTreeNode(key,record));  
    
    //Case: key < temp->date
    if (Compare(key,temp->date)==-1) 
        temp->left = BST_Insert(temp->left,key,record);  
    //Case: key > temp->date
    else if (Compare(key,temp->date)==1)  
        temp->right = BST_Insert(temp->right,key,record);  
    //Case: Equality (key==temp->date)
    else
    {  
        createSDNode(temp,record);
        return temp;  
    }
    temp->height = 1 + max(height(temp->left),height(temp->right));  
  
    int balance = getBalance(temp);  
  
    //Case: Left left child
    if (balance < -1 && Compare(key,temp->right->date)==1)  
        return leftRotate(temp);  

    //Case: Right right child
    if (balance > 1 && Compare(key,temp->left->date)==-1)  
        return rightRotate(temp);  
  
    //Case: Right left child
    if (balance < -1 && Compare(key,temp->right->date)==-1)  
    {  
        temp->right = rightRotate(temp->right);  
        return leftRotate(temp);  
    }  
    
    //Case:Left right child
    if (balance > 1 && Compare(key,temp->left->date)==1)  
    {  
        temp->left = leftRotate(temp->left);  
        return rightRotate(temp);  
    }

    return temp;  
}  

//Print TreeNodes with preOrder traversal...  
void preOrder(TreeNode *root)  
{  
    if(root != NULL)  
    {  
        cout << root->date;
        Print_SDNodes(root);  
        preOrder(root->left);  
        preOrder(root->right);  
    }  
}  

//Print TreeNodes with inOrder traversal...  
void inOrder(TreeNode* root)
{
    if(root != NULL)
    {
        inOrder(root->left);
        cout << root->record->entryDate << " "  << root->record->exitDate << " " << root->record->country << " " << root->record->patientFirstName << endl;
        Print_SDNodes(root);
        inOrder(root->right);
    }
}

//Calculate Summary Stats for each date file of directory and add it to big string summary_stats.
void Summary_Stats_Of_Dir(TreeNode* root,string* summary_stats,string country)
{
    if(root != NULL)
    {
        Summary_Stats_Of_Dir(root->left,summary_stats,country);
        
        //Go to all nodes of AVL_tree inorderly and for each node traverse the Same_Date_Node_List so as to calculate cases of diseases in a specific date.
        //With the help of function SS_Insert, which simply create nodes of unique diseases,which keep infos about cases!
        SS_Node* head = NULL,*temp=NULL;
        SDNode* tmp = root->next;
        
        //Fill SS_Nodes with infos about cases...
        SS_Insert(&head,root->record->diseaseID,stoi(root->record->age));
        while(tmp!=NULL) 
        {
            SS_Insert(&head,tmp->record->diseaseID,stoi(tmp->record->age));
            tmp=tmp->next;
        }

        //Create the right output format for summary stats of this date!
        temp=head;
        (*summary_stats).append(root->date);
        (*summary_stats).append("\n");
        (*summary_stats).append(country);
        (*summary_stats).append("\n");
        while(temp!=NULL)
        {
            (*summary_stats).append(temp->name);
            (*summary_stats).append("\n");
            (*summary_stats).append("Age range 0-20 years: ");
            (*summary_stats).append(to_string(temp->agecategory1));
            (*summary_stats).append(" cases");
            (*summary_stats).append("\n");
            (*summary_stats).append("Age range 21-40 years: ");
            (*summary_stats).append(to_string(temp->agecategory2));
            (*summary_stats).append(" cases");
            (*summary_stats).append("\n");
            (*summary_stats).append("Age range 41-60 years: ");
            (*summary_stats).append(to_string(temp->agecategory3));
            (*summary_stats).append(" cases");
            (*summary_stats).append("\n");
            (*summary_stats).append("Age range 60+ years: ");
            (*summary_stats).append(to_string(temp->agecategory4));
            (*summary_stats).append(" cases");
            (*summary_stats).append("\n\n");        
            temp = temp->next;
        }

        //Deallocate memory for SS_Nodes.
        SS_Delete_List(head);
        
        Summary_Stats_Of_Dir(root->right,summary_stats,country);
    }
}

//Print SameDateNodes...
void Print_SDNodes(TreeNode* root)
{
    SDNode* tmp = root->next;
    int counter=1;
    while(tmp!=NULL) 
    {
        cout << tmp->record->entryDate << " " << tmp->record->exitDate << " " << tmp->record->country << " " << tmp->record->patientFirstName << "(S)" << endl;
        counter++;
        tmp=tmp->next;
    }
}

//Function which deallocates memory for all TreeNodes of AVL Tree
void BST_Destroy(TreeNode* temp)
{
    if(temp==NULL) return;
    
    BST_Destroy(temp->left);
    BST_Destroy(temp->right);
    
    Destroy_SDNodes(temp);
    delete temp;
}

//Function which deallocates memory for list of a TreeNode(with same dates)
bool Destroy_SDNodes(TreeNode* temp)
{
    SDNode* tmp = temp->next;
    SDNode* next;

    while(tmp!=NULL)
    {
        next = tmp->next;
        delete(tmp);
        tmp = next;
    }
    temp->next = NULL;
}

void BST_Check_For_Exit_Date(TreeNode* root)
{
    if(root==NULL) return;

    BST_Check_For_Exit_Date(root->left);
    BST_Check_For_Exit_Date(root->right); 

    if(root->record->exitDate=="-") still_patients++;
    
    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        if(temp->record->exitDate=="-") still_patients++;
        temp = temp->next;
    }
}

void BST_Print_Still_Patients(TreeNode* root)
{
    if(root==NULL) return;

    BST_Print_Still_Patients(root->left);
    BST_Print_Still_Patients(root->right);

    if(root->record->exitDate=="-") still_patients++;

    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        if(temp->record->exitDate=="-") still_patients++;
        temp = temp->next;
    }
}                       
    
//Count suspects of virus.
void CountSuspectsNoRestriction(TreeNode* root)
{
    if(root==NULL) return;

    CountSuspectsNoRestriction(root->left);
    CountSuspectsNoRestriction(root->right); 

    suspects++;
    
    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        suspects++;
        temp = temp->next;
    }
}

//Count suspects of virus who are inside [date1,date2].
void CountSuspectsWithRestriction(TreeNode* root,string date1,string date2)
{
    if(root==NULL) return;
    
    if(Compare(root->date,date1)!=-1) //root->date>=date1 
    {        
        if(Compare(root->date,date2)!=1) //root->date<=date2
        {
            suspects++;
            
            SDNode* temp = root->next;
            while(temp!=NULL)
            {
                if(Compare(temp->record->entryDate,date2)!=1) suspects++; //temp->record->entryDate<=date2
                temp = temp->next;
            }
        }
        CountSuspectsWithRestriction(root->right,date1,date2); 
        CountSuspectsWithRestriction(root->left,date1,date2); 
    }
    if(Compare(root->date,date1)==-1) CountSuspectsWithRestriction(root->right,date1,date2);
}

//Count suspects of virus who are inside [date1,date2] and their nationality is from a specific country.
void CountSuspectsWithDoubleRestriction(TreeNode* root,string date1,string date2,string country)
{
    if(root==NULL) return;
    
    if(Compare(root->date,date1)!=-1) //root->date>=date1 
    {        
        if(Compare(root->date,date2)!=1) //root->date<=date2
        {
            if(root->record->country==country) suspects++;
            
            SDNode* temp = root->next;
            while(temp!=NULL)
            {
                if(Compare(temp->record->entryDate,date2)!=1 && temp->record->country==country) suspects++; //temp->record->entryDate<=date2
                temp = temp->next;
            }
        }
        CountSuspectsWithDoubleRestriction(root->right,date1,date2,country); 
        CountSuspectsWithDoubleRestriction(root->left,date1,date2,country); 
    }
    if(Compare(root->date,date1)==-1) CountSuspectsWithDoubleRestriction(root->right,date1,date2,country);
}

//Count suspects of virus and their nationality is from a specific country.
void NumOfPatientsOfaDisease(TreeNode* root,string country)
{
    if(root==NULL) return;

    NumOfPatientsOfaDisease(root->left,country);
    NumOfPatientsOfaDisease(root->right,country); 

    if(root->record->country==country) suspects++;
            
    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        if(temp->record->country==country) suspects++;
        temp = temp->next;
    }
}

//Count suspects of country and their virus is a specific illness.
void NumOfPatientsOfaCountry(TreeNode* root,string disease)
{
    if(root==NULL) return;

    NumOfPatientsOfaCountry(root->left,disease);
    NumOfPatientsOfaCountry(root->right,disease); 

    if(root->record->diseaseID==disease) suspects++;
            
    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        if(temp->record->diseaseID==disease) suspects++;
        temp = temp->next;
    }
}

//Count suspects of country who are inside [date1,date2] and their illness is a specific disease.
void CountSuspectsForaDisease(TreeNode* root,string date1,string date2,string disease)
{
    if(root==NULL) return;
    
    if(Compare(root->date,date1)!=-1) //root->date>=date1 
    {        
        if(Compare(root->date,date2)!=1) //root->date<=date2
        {
            if(root->record->diseaseID==disease) suspects++;
            
            SDNode* temp = root->next;
            while(temp!=NULL)
            {
                if(Compare(temp->record->entryDate,date2)!=1 && temp->record->diseaseID==disease) suspects++; //temp->record->entryDate<=date2
                temp = temp->next;
            }
        }
        CountSuspectsForaDisease(root->right,date1,date2,disease); 
        CountSuspectsForaDisease(root->left,date1,date2,disease); 
    }
    if(Compare(root->date,date1)==-1) CountSuspectsForaDisease(root->right,date1,date2,disease);
}

void CalculateAgeRanges(TreeNode* root,string date1,string date2,string disease,SS_Node** head)
{
    if(root==NULL) return;
    
    if(Compare(root->date,date1)!=-1) //root->date>=date1 
    {        
        if(Compare(root->date,date2)!=1) //root->date<=date2
        {
            if(root->record->diseaseID==disease)    SS_Insert(head,root->record->diseaseID,stoi(root->record->age));
            
            SDNode* temp = root->next;
            while(temp!=NULL)
            {
                if(Compare(temp->record->entryDate,date2)!=1 && temp->record->diseaseID==disease) SS_Insert(head,temp->record->diseaseID,stoi(temp->record->age)); //temp->record->entryDate<=date2
                temp = temp->next;
            }
        }
        CalculateAgeRanges(root->right,date1,date2,disease,head); 
        CalculateAgeRanges(root->left,date1,date2,disease,head); 
    }
    if(Compare(root->date,date1)==-1) CalculateAgeRanges(root->right,date1,date2,disease,head);
}

void CountSuspectsOfDifferentCountries(TreeNode* root,string date1,string date2,SS_Node** head)
{
    if(root==NULL) return;
    
    if(Compare(root->date,date1)!=-1) //root->date>=date1 
    {        
        if(Compare(root->date,date2)!=1) //root->date<=date2
        {
            SS_Insert(head,root->record->country,1);
            
            SDNode* temp = root->next;
            while(temp!=NULL)
            {
                if(Compare(temp->record->entryDate,date2)!=1)   SS_Insert(head,temp->record->country,1); //temp->record->entryDate<=date2
                temp = temp->next;
            }
        }
        CountSuspectsOfDifferentCountries(root->right,date1,date2,head); 
        CountSuspectsOfDifferentCountries(root->left,date1,date2,head); 
    }
    if(Compare(root->date,date1)==-1) CountSuspectsOfDifferentCountries(root->right,date1,date2,head);
}

void CountDischargesOfDifferentCountries(TreeNode* root,string date1,string date2,SS_Node** head)
{
    if(root==NULL) return;

    CountDischargesOfDifferentCountries(root->left,date1,date2,head); 
    CountDischargesOfDifferentCountries(root->right,date1,date2,head); 

    if(Compare(root->record->exitDate,date1)!=-1 && Compare(root->record->exitDate,date2)!=1) //date1<=root->exit_date<=date2
        SS_Insert(head,root->record->country,1);

    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        if(Compare(temp->record->exitDate,date1)!=-1 && Compare(temp->record->exitDate,date2)!=1) //date1<=temp->exit_date<=date2
            SS_Insert(head,temp->record->country,1);
        temp = temp->next;
    }
}

void CountDischargesWithDoubleRestriction(TreeNode* root,string date1,string date2,string country)
{
    if(root==NULL) return;

    CountDischargesWithDoubleRestriction(root->left,date1,date2,country); 
    CountDischargesWithDoubleRestriction(root->right,date1,date2,country); 

    if(Compare(root->record->exitDate,date1)!=-1 && Compare(root->record->exitDate,date2)!=1 && root->record->country==country) //date1<=root->exit_date<=date2
        suspects++;

    SDNode* temp = root->next;
    while(temp!=NULL)
    {
        if(Compare(temp->record->exitDate,date1)!=-1 && Compare(temp->record->exitDate,date2)!=1 && temp->record->country==country) //date1<=temp->exit_date<=date2
            suspects++;
        temp = temp->next;
    }
}