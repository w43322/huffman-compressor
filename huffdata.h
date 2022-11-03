#ifndef HUFFDATA_H
#define HUFFDATA_H

#include"hufftree.h"

unordered_map<char,int>CountEachByte(const char*,int);
unordered_map<char,int>CountEachByte(QByteArray&);
unordered_map<char,string>GetHuffCode(unordered_map<char,int>&);
int Encode(QByteArray&,unordered_map<char,string>&,const char*,int);
QString Decode(const char*,int,int);
QByteArray DeCompress(QString&,std::unordered_map<char,string>);

#endif // HUFFDATA_H
