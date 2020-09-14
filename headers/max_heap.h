#include <iostream>

using namespace std;

class HeapNode;
typedef HeapNode* HeapNodeptr;
class Data;
typedef Data* Dataptr;

class Data
{
    public:
        float num;
        string name;

        Data(string _name,float _num)
        : name(_name),num(_num)
        {}
        ~Data()
        {}
};

class HeapNode
{
    public:
        HeapNodeptr left;
        HeapNodeptr right;
        HeapNodeptr parent;
        Dataptr data;
        
        HeapNode(string _name,float _num)
        :left(NULL),right(NULL),parent(NULL)
        {
            data = new Data(_name,_num);
        }
        ~HeapNode()
        {
            delete data;
            parent=NULL;
            right=NULL;
            left=NULL;
        }
};

class Max_Heap
{
    public:
        HeapNodeptr root;
        HeapNodeptr last;
        int heap_nodes;

        Max_Heap()
        : root(NULL),last(NULL),heap_nodes(0)
        {}
        ~Max_Heap()
        {}

        Dataptr ExtractRoot();
        void Insert(string, float);
        void Heapify();
        void Swap(Dataptr ,Dataptr );
        void Delete(HeapNodeptr );
        void printLevelOrder(HeapNodeptr ,int );
        void printGivenLevel(HeapNodeptr , int ); 
        string Topk(int );
};