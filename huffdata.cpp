#include"huffdata.h"

//统计各个字节的出现次数
unordered_map<char,int>CountEachByte(const char* buf,int SIZ)
{
    unordered_map<char,int>res;
    for(int i=0;i<SIZ;++i)
        res[buf[i]]++;
    return res;
}
unordered_map<char,int>CountEachByte(QByteArray &arr)
{
    unordered_map<char,int>res;
    for(auto i:arr)
        res[i]++;
    return res;
}

//根据字节数量统计建立哈夫曼树并返回每个字符的哈夫曼编码
unordered_map<char,string>GetHuffCode(unordered_map<char,int>&ByteCnt)
{
    HuffTree TREE(ByteCnt);
    unordered_map<char,string>res=TREE.toHuffCode();
    return res;
}

//根据已经生成的哈夫曼树把数据编码为哈夫曼编码
//把哈夫曼编码以二进制形式储存并返回
//返回编码长度
int Encode(QByteArray &res,unordered_map<char,string>&HuffCode,const char* buf,int SIZ)
{
    res.clear();
    int totBITcnt=0;
    char BYTE=0;
    int BITcnt=0;
    for(int i=0;i<SIZ;++i)
    {
        for(unsigned j=0;j<HuffCode[buf[i]].length();++j)
        {
            char BIT=HuffCode[buf[i]][j]=='0'?0:1;
            BYTE<<=1;
            BYTE|=BIT;
            ++BITcnt;
            ++totBITcnt;
            if(BITcnt==8)
            {
                res.push_back(BYTE);
                BYTE=0;
                BITcnt=0;
            }
        }
    }
    if(BITcnt)
    {
        while(BITcnt<8)
        {
            BYTE<<=1;
            ++BITcnt;
        }
        res.push_back(BYTE);
    }
    return totBITcnt;
}

//把二进制方式储存的哈夫曼编码转换为“0,1”的字符串
QString Decode(const char* buf,int SIZ,int BitCnt)
{
    QString res;
    for(auto it=0;it<SIZ;++it)
    {
        auto i=buf[it];
        for(int j=7;j>=0;--j)
        {
            if(i>>j&1)
                res+="1";
            else
                res+="0";
            if(res.length()==BitCnt)
                break;
        }
    }
    return res;
}

//把“0,1”字符串形式的哈夫曼编码转换回原来的文件并返回
QByteArray DeCompress(QString &HuffString,unordered_map<char,string>HuffCode)
{
    HuffTree TREE(HuffCode);
    QByteArray res=TREE.toByteArray(HuffString);
    return res;
}
