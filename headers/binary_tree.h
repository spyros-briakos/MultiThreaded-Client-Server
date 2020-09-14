#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <iostream>
#include <string>
#include "record_list.h"

using namespace std;

typedef class Summary_Statistic_Node
{
    public:
        string name;
        int agecategory1;
        int agecategory2;
        int agecategory3;
        int agecategory4;
        Summary_Statistic_Node* next;

        Summary_Statistic_Node(string _name)
        :name(_name),next(NULL),agecategory1(0),agecategory2(0),agecategory3(0),agecategory4(0)
        {}
}SS_Node;
void SS_Insert(SS_Node** ,string, int );  
void SS_Delete_List(SS_Node* );
void SS_Print_List(SS_Node* );
int SS_Length(SS_Node* );

class SameDateNode;
typedef class SameDateNode SDNode;

class SameDateNode
{
    public:
        SDNode* next;
        RecordNode* record;

        SameDateNode(RecordNode* rec)
        : next(NULL),record(rec)
        {};
        ~SameDateNode()
        {
            next=NULL;
            record=NULL;
        } 
};

class TreeNode
{
    public:
        string date;
        TreeNode* left;
        TreeNode* right;
        RecordNode* record;
        SDNode* next; 
        int height; 
        
        TreeNode(string _date,RecordNode* rec)
        : date(_date),left(NULL),right(NULL),next(NULL),height(1),record(rec)
        {}
        ~TreeNode()
        {
            left=NULL;
            right=NULL;
            record=NULL;
            next=NULL;
        };
        
};

bool Destroy_SDNodes(TreeNode* );

TreeNode* createTreeNode(string );
TreeNode* rightRotate(TreeNode* );
TreeNode* leftRotate(TreeNode* );
TreeNode* BST_Insert(TreeNode* ,string, RecordNode* );

int getBalance(TreeNode* );
int Compare(string , string );
int height(TreeNode* );
int max(int , int );

void BST_Print_Still_Patients(TreeNode* );
void BST_Check_For_Exit_Date(TreeNode* );
void createSDNode(TreeNode* ,RecordNode* );
void preOrder(TreeNode *);  
void inOrder(TreeNode *);  
void BST_Destroy(TreeNode* );
void Print_SDNodes(TreeNode* );
void CountSuspectsNoRestriction(TreeNode* );
void CountSuspectsWithRestriction(TreeNode* ,string ,string );
void CountSuspectsWithDoubleRestriction(TreeNode* ,string ,string ,string );
void CountSuspectsForaDisease(TreeNode* ,string ,string ,string );
void NumOfPatientsOfaDisease(TreeNode* ,string );
void NumOfPatientsOfaCountry(TreeNode* ,string );

void Summary_Stats_Of_Dir(TreeNode* ,string* ,string );
void CalculateAgeRanges(TreeNode* ,string ,string ,string ,SS_Node** );
void CountSuspectsOfDifferentCountries(TreeNode* ,string ,string ,SS_Node** );
void CountDischargesOfDifferentCountries(TreeNode* ,string ,string ,SS_Node** );
void CountDischargesWithDoubleRestriction(TreeNode* ,string ,string ,string );

#endif