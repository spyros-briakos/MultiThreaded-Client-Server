#include <iostream>
#include "../headers/max_heap.h"

using namespace std;

void Max_Heap::Insert(string name,float num)
{
    HeapNodeptr new_node = new HeapNode(name,num);
    this->heap_nodes++;

    //Case: Empty heap.
    if(root==NULL)
    {
        this->root=new_node;
        this->last=new_node;
        return;
    }

    //Initialize temp with last heapnode.
    HeapNodeptr temp = this->last;
    //Go up until heapnode is right child of its parent or we reached root. 
    while(temp->parent!=NULL && temp == temp->parent->right) temp = temp->parent;

    //Case A: temp isn't root.
    if(temp->parent!=NULL)
    {
        //Case A1: temp's parent has a right child.
        if(temp->parent->right!=NULL)
        {
            temp = temp->parent->right;
            //Go as much down left as possible.
            while(temp->left!=NULL) temp = temp->left;
        }
        //Case A2: temp's parent hasn't a right child.
        else temp = temp->parent;
    }
    //Case B: temp==root, last was the rightmost heapnode,so temp will be leftmost of new level.
    else
    {
        while(temp->left!=NULL) temp = temp->left;
    }

    this->last = new_node; //Update last heapnode.
    if(temp->left!=NULL) temp->right = new_node; //Case A2.
    else temp->left = new_node; //Case B.
    new_node->parent = temp;
    
    //Make it again Max_Heap
    while(new_node->parent!=NULL && new_node->parent->data->num < new_node->data->num)
    {
        this->Swap(new_node->parent->data,new_node->data);
        new_node = new_node->parent;
    } 
} 

//Function for swapping data
void Max_Heap::Swap(Dataptr d1,Dataptr d2)
{
    //Swapping number of people who are ill.
    float temp_num = d1->num;
    d1->num = d2->num;
    d2->num = temp_num;
    //Swapping name of disease or country.
    string temp_name = d1->name;
    d1->name = d2->name;
    d2->name = temp_name;
}

//Deallocating memory for heapnodes
void Max_Heap::Delete(HeapNodeptr root)
{
    if(root==NULL) return;
    Delete(root->right);
    Delete(root->left);
    delete root;
}

//Make binary heap again have the property of heap
void Max_Heap::Heapify()
{   
    HeapNodeptr temp = this->root;
    while(1)
    {
        if(temp->left!=NULL && temp->right!=NULL)
        {
            if(temp->left->data->num<=temp->right->data->num) 
            {
                if(temp->data->num <= temp->right->data->num)
                    this->Swap(temp->right->data,temp->data);
                temp = temp->right;
            }
            else
            {
                if(temp->data->num <= temp->left->data->num)
                    this->Swap(temp->left->data,temp->data);
                temp = temp->left;
            }
        }
        else if(temp->left!=NULL && temp->right==NULL)
        {
            if(temp->data->num <= temp->left->data->num)
                this->Swap(temp->left->data,temp->data);    
            temp = temp->left;
        }
        else if(temp->left==NULL && temp->right!=NULL)
        {
            if(temp->data->num <= temp->right->data->num)
                this->Swap(temp->right->data,temp->data);
            temp = temp->right;
        }
        else
        {
            break;
        }
    }
}

//Pop the root.
Dataptr Max_Heap::ExtractRoot()
{
    //Store root, so as to return it.
    Dataptr temp_data = new Data(this->root->data->name,this->root->data->num);

    this->heap_nodes--;

    //Case: Max_Heap is empty.
    if(this->root==NULL || this->last==NULL) return NULL;

    //Case: Max_Heap has only one heapnode.
    if(this->last==this->root) 
    {
        delete this->root;
        this->root = NULL;
        this->last = NULL;
        return temp_data;
    }
    HeapNodeptr temp = this->last;
    while(temp->parent!=NULL && temp==temp->parent->left) temp = temp->parent;

    //Case: Root wasn't reached, so go to left sibling.
    if(temp->parent!=NULL) temp = temp->parent->left;

    //Process to reach to the second last heapnode. (temp will be new last heapnode)
    while(temp->right!=NULL) temp = temp->right;

    //Swap data of last heapnode and root.
    this->Swap(this->last->data,this->root->data);
    
    //Make parent of last node point to NULL wherever last node was...
    if(this->last==this->last->parent->right) this->last->parent->right=NULL;
    else if(this->last==this->last->parent->left) this->last->parent->left=NULL;
    
    //Deallocate memory for last heapnode.
    delete this->last;

    //Update last node.
    this->last = temp;

    //Make it again Heap.
    this->Heapify();

    return temp_data;
}

void Max_Heap::printLevelOrder(HeapNodeptr root,int h) 
{ 
    int i; 
    for (i=1; i<=h; i++) 
    { 
        printGivenLevel(root, i); 
        cout<< endl;
    } 
    cout<< endl;
} 
  
void Max_Heap::printGivenLevel(HeapNodeptr root, int level) 
{ 
    if (root == NULL) 
        return; 
    if (level == 1) cout << root->data->num << " "; 
    else if (level > 1) 
    { 
        printGivenLevel(root->left, level-1); 
        printGivenLevel(root->right, level-1); 
    } 
} 

//Function which prints Topk diseases or countries.
string Max_Heap::Topk(int k)
{
    Dataptr rt;
    string topk;

    if(k>=this->heap_nodes)
    {
        int limit = this->heap_nodes;
        for(int j=0;j<limit;j++)
        {
            // this->printLevelOrder(this->root,3);
            rt = this->ExtractRoot();
            topk.append(rt->name);
            topk.append(" ");
            topk.append(to_string(int(rt->num*100)));
            topk.append("%\n");
            delete rt;
        }
    }
    else
    {
        for(int j=0;j<k;j++)
        {
            // this->printLevelOrder(this->root,3);
            rt = this->ExtractRoot();
            topk.append(rt->name);
            topk.append(" ");
            topk.append(to_string(int(rt->num*100)));
            topk.append("%\n");
            delete rt;
        }  
    }
    return topk;
}

// int main(void)
// {
//     Max_Heap* p = new Max_Heap(); //Create an empty Binary Heap.
//     int k=5;
    
//     p->Insert("0-20:",float(10)/float(100));
//     p->Insert("21-40:",float(204)/float(100));
//     p->Insert("41-60:",float(304)/float(100));
//     p->Insert("60+:",float(402)/float(100));

//     p->Topk(k);
//     //Deallocate memory for on the fly binary heap.
//     p->Delete(p->root);
//     delete p;
// }