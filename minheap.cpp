#include "minheap.h"
void MinHeap::MinimizeHeap(int i)
{
    int l=i<<1;
    int r=(i<<1)+1;
    int smallest=i;
    if(l<=siz&&heap[l]->freq<heap[smallest]->freq)smallest=l;
    if(r<=siz&&heap[r]->freq<heap[smallest]->freq)smallest=r;
    if(smallest!=i)
    {
        swap(heap[i],heap[smallest]);
        MinimizeHeap(smallest);
    }
}
void MinHeap::MaintainHeap(int i)
{
    int p=i>>1;
    if(p==0)return;
    if(heap[p]->freq>heap[i]->freq)
    {
        swap(heap[i],heap[p]);
        MaintainHeap(p);
    }
}
void MinHeap::push(treeNode *nodeptr)
{
    heap[++siz]=nodeptr;
    MaintainHeap(siz);
}
treeNode *MinHeap::pop()
{
    treeNode* res=heap[1];
    heap[1]=heap[siz--];
    MinimizeHeap(1);
    return res;
}
int MinHeap::size()
{
    return siz;
}
