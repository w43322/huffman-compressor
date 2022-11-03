#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "minheap.h"
#include "treenode.h"
#include<unordered_map>

#include<QDebug>
#include<QByteArray>
using namespace std;

//定义哈夫曼树
class HuffTree
{
public:
    treeNode *root;

    QByteArray toByteArray(QString&);
    HuffTree(unordered_map<char,string>&);
    HuffTree(unordered_map<char,int>&);
    ~HuffTree();
    unordered_map<char,string>toHuffCode();

private:
    void FreeMem(treeNode*);
    void DFS(unordered_map<char,string>&,treeNode*,string);
    treeNode *join(treeNode*,treeNode*);
};

#endif // HUFFTREE_H
