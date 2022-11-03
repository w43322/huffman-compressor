#ifndef TREENODE_H
#define TREENODE_H

#include <utility>

//定义结点类
class treeNode
{
public:
    int data,freq;
    treeNode *lchild,*rchild;
    treeNode(int DATA,int FREQ)
    {
        this->data=DATA;
        this->freq=FREQ;
        this->lchild=NULL;
        this->rchild=NULL;
    }
};

#endif // TREENODE_H
