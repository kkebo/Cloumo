/*
Created by: PiMaster
*/

#include <pistring.h>

char stroob=0;

//I couldn't use the ones defined in the string class, so I made an obscurely named namespace to hide them
bool strexternalfuncs::isequal(const char* str1,const char* str2,unsigned len1,unsigned len2){
	if(len1!=len2){
		return false;}
	unsigned pos=0;
	while(pos<len1){
		if(str1[pos]!=str2[pos]){
			return false;}
		++pos;}
	return true;}
unsigned strexternalfuncs::strlen(const char* str){
	unsigned x = 0;
	while(str[x]!=0){
		++x;}
	return x;}

//equal
bool operator==(const string& str1,const string& str2){
	return (str1.comparestr(str2.data,0,str1.datalen,0,str2.datalen)==0);}

bool operator==(const string& str1,const char* str2){
	return (str1.comparestr(str2,0,str1.datalen,0,str1.strlen(str2))==0);}

bool operator==(const char* str1,const string& str2){
	return (str2.comparestr(str1,0,str2.datalen,0,str2.strlen(str1))==0);}

//not equal
bool operator!=(const string& str1,const string& str2){
	return (str1.comparestr(str2.data,0,str1.datalen,0,str2.datalen)!=0);}

bool operator!=(const string& str1,const char* str2){
	return (str1.comparestr(str2,0,str1.datalen,0,str1.strlen(str2))!=0);}

bool operator!=(const char* str1,const string& str2){
	return (str2.comparestr(str1,0,str2.datalen,0,str2.strlen(str1))!=0);}

//greater than
bool operator>(const string& str1,const string& str2){
	return (str1.comparestr(str2.data,0,str1.datalen,0,str2.datalen)>0);}

bool operator>(const string& str1,const char* str2){
	return (str1.comparestr(str2,0,str1.datalen,0,str1.strlen(str2))>0);}

bool operator>(const char* str1,const string& str2){
	return (str2.comparestr(str1,0,str2.datalen,0,str2.strlen(str1))>0);}

//less than
bool operator<(const string& str1,const string& str2){
	return (str1.comparestr(str2.data,0,str1.datalen,0,str2.datalen)<0);}

bool operator<(const string& str1,const char* str2){
	return (str1.comparestr(str2,0,str1.datalen,0,str1.strlen(str2))<0);}

bool operator<(const char* str1,const string& str2){
	return (str2.comparestr(str1,0,str2.datalen,0,str2.strlen(str1))<0);}

//greater than or equal to
bool operator>=(const string& str1,const string& str2){
	return (str1.comparestr(str2.data,0,str1.datalen,0,str2.datalen)>=0);}

bool operator>=(const string& str1,const char* str2){
	return (str1.comparestr(str2,0,str1.datalen,0,str1.strlen(str2))>=0);}

bool operator>=(const char* str1,const string& str2){
	return (str2.comparestr(str1,0,str2.datalen,0,str2.strlen(str1))>=0);}

//less than or equal to
bool operator<=(const string& str1,const string& str2){
	return (str1.comparestr(str2.data,0,str1.datalen,0,str2.datalen)<=0);}

bool operator<=(const string& str1,const char* str2){
	return (str1.comparestr(str2,0,str1.datalen,0,str1.strlen(str2))<=0);}

bool operator<=(const char* str1,const string& str2){
	return (str2.comparestr(str1,0,str2.datalen,0,str2.strlen(str1))<=0);}
