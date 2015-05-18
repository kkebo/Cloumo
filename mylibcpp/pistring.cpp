/*
Created by: PiMaster
*/

#include <pistring.h>

char stroob=0;

//I couldn't use the ones defined in the string class, so I made an obscurely named namespace to hide them
//(this isn't bad practice, is it?)
namespace strexternalfuncs{
	bool isequal(const char* str1,const char* str2,const unsigned len1,const unsigned len2){
		if(len1!=len2){
			return false;}
		unsigned pos=0;
		while(pos<len1){
			if(str1[pos]!=str2[pos]){
				return false;}
			++pos;}
		return true;}
	unsigned strlen(const char* str){
		unsigned x = 0;
		while(str[x]!=0){
			++x;}
		return x;}};

//global operators
//add
string operator + (const char c, const string &str) {
	return string(c) += str;
}
string operator + (const string &str, const char c) {
	return string(str) += c;
}
string operator + (const char *str1, const string &str2) {
	return string(str1) += str2;
}
string operator + (const string &str1, const char *str2) {
	return string(str1) += str2;
}
string operator + (const string &str1, const string &str2) {
	return string(str1) += str2;
}

//equal
bool operator==(const string& str1,const string& str2){
	return strexternalfuncs::isequal(str1.data,str2.data,str1.datalen,str2.datalen);}

bool operator==(const string& str1,const char* str2){
	return strexternalfuncs::isequal(str1.data,str2,str1.datalen,strexternalfuncs::strlen(str2));}

bool operator==(const char* str1,const string& str2){
	return strexternalfuncs::isequal(str1,str2.data,strexternalfuncs::strlen(str1),str2.datalen);}

//not equal
bool operator!=(const string& str1,const string& str2){
	return !strexternalfuncs::isequal(str1.data,str2.data,str1.datalen,str2.datalen);}

bool operator!=(const string& str1,const char* str2){
	return !strexternalfuncs::isequal(str1.data,str2,str1.datalen,strexternalfuncs::strlen(str2));}

bool operator!=(const char* str1,const string& str2){
	return !strexternalfuncs::isequal(str1,str2.data,strexternalfuncs::strlen(str1),str2.datalen);}

//greater than
bool operator>(const string& str1,const string& str2){
	return str1.datalen>str2.datalen;}

bool operator>(const string& str1,const char* str2){
	return str1.datalen>strexternalfuncs::strlen(str2);}

bool operator>(const char* str1,const string& str2){
	return strexternalfuncs::strlen(str1)>str2.datalen;}

//less than
bool operator<(const string& str1,const string& str2){
	return str1.datalen<str2.datalen;}

bool operator<(const string& str1,const char* str2){
	return str1.datalen<strexternalfuncs::strlen(str2);}

bool operator<(const char* str1,const string& str2){
	return strexternalfuncs::strlen(str1)<str2.datalen;}

//greater than or equal to
bool operator>=(const string& str1,const string& str2){
	return str1.datalen>=str2.datalen;}

bool operator>=(const string& str1,const char* str2){
	return str1.datalen>=strexternalfuncs::strlen(str2);}

bool operator>=(const char* str1,const string& str2){
	return strexternalfuncs::strlen(str1)>=str2.datalen;}

//less than or equal to
bool operator<=(const string& str1,const string& str2){
	return str1.datalen<=str2.datalen;}

bool operator<=(const string& str1,const char* str2){
	return str1.datalen<=strexternalfuncs::strlen(str2);}

bool operator<=(const char* str1,const string& str2){
	return strexternalfuncs::strlen(str1)<=str2.datalen;}
