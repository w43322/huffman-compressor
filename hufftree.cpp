#include"hufftree.h"


QByteArray HuffTree::toByteArray(QString &HuffString)
{
    QByteArray res;
    treeNode *nownode=root;
    for(int i=0;i<HuffString.length();++i)
    {
        if(HuffString[i]=='0')
            nownode=nownode->lchild;
        else
            nownode=nownode->rchild;
        if(nownode->lchild==NULL&&nownode->rchild==NULL)
        {
            res.push_back(nownode->data);
            nownode=root;
        }
    }
    return res;
}
HuffTree::HuffTree(unordered_map<char,string>&HuffCode)
{
    root=new treeNode(0,0);
    for(auto &&it:HuffCode)
    {
        //qDebug("%4d|%20s|%c",it.first,it.second.c_str(),(char)it.first);
        string str=it.second;
        treeNode *nownode=root;
        for(unsigned i=0;i<str.length();++i)
        {
            if(str[i]=='0')
            {
                if(!nownode->lchild)
                    nownode->lchild=new treeNode(it.first,0);
                nownode=nownode->lchild;
            }
            else
            {
                if(!nownode->rchild)
                    nownode->rchild=new treeNode(it.first,0);
                nownode=nownode->rchild;
            }
        }
    }
}
HuffTree::HuffTree(unordered_map<char,int>&ByteCnt)
{
    MinHeap *pq=new MinHeap;
    //generate huffman tree
    for(auto i:ByteCnt)
    {
        treeNode *newnode=new treeNode(i.first,i.second);
        pq->push(newnode);
    }
    while(pq->size()>1)
    {
        treeNode *fir=pq->pop();
        treeNode *sec=pq->pop();
        treeNode *newnode=join(fir,sec);
        pq->push(newnode);
    }
    root=pq->pop();
    delete pq;
}
HuffTree::~HuffTree()
{
    FreeMem(root);
}
//返回哈夫曼编码
unordered_map<char,string>HuffTree::toHuffCode()
{
    unordered_map<char,string>res;
    DFS(res,root,"");
    return res;
}
void HuffTree::FreeMem(treeNode *nownode)
{
    if(nownode==NULL)return;
    if(nownode->lchild)
        FreeMem(nownode->lchild);
    if(nownode->rchild)
        FreeMem(nownode->rchild);
    free(nownode);
}
void HuffTree::DFS(unordered_map<char,string>&HuffmanCode,treeNode *nownode,string nowstr)
{
    if(!(nownode->lchild||nownode->rchild))
    {
        if(nownode==root)
            HuffmanCode[nownode->data]="0";
        else
            HuffmanCode[nownode->data]=nowstr;
        return;
    }
    if(nownode->lchild)
        DFS(HuffmanCode,nownode->lchild,nowstr+"0");
    if(nownode->rchild)
        DFS(HuffmanCode,nownode->rchild,nowstr+"1");
}
treeNode *HuffTree::join(treeNode *A,treeNode *B)
{
    treeNode *res=new treeNode(0,A->freq+B->freq);
    res->lchild=A;
    res->rchild=B;
    return res;
}
