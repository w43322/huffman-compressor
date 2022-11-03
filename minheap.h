#ifndef MINHEAP_H
#define MINHEAP_H

#define MIN_HEAP_SIZE 1024

#include "treenode.h"
#include <utility>
using std::swap;

class MinHeap
{
public:
    void push(treeNode *nodeptr);
    treeNode *pop();
    int size();
private:
    int siz=0;
    treeNode *heap[MIN_HEAP_SIZE];
    void MinimizeHeap(int i);
    void MaintainHeap(int i);
};



#undef MIN_HEAP_SIZE

#endif // MINHEAP_H
