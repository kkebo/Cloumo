/*
Created by: PiMaster
*/

#include <pistring.h>

char stroob=0;

//resize the string data array
void string::resizedata(unsigned newsize) {
	//minimum buffer size
	if(newsize<minbuffsize){
		newsize=minbuffsize;}
	char* tmp=new char[datasize+newsize];
	if(data!=NULL){
		memcpy(tmp,data,datalen+1);
		delete[] data;}
	data=tmp;
	datasize+=newsize;
}

//doesn't check for memory overlapping
void* string::memcpy(void* dst,const void* src,const unsigned len) {
	if(dst==NULL || src==NULL || len==0){
		return NULL;}
	char* d=(char*)dst;
	char* s=(char*)src;
	unsigned n=0;
	while(n<len){
		*d=*s;
		++d;
		++s;
		++n;}
	return dst;
}

//does check for memory overlapping
void* string::memmove(void* dst,const void* src,unsigned len) {
	if(dst==NULL || src==NULL || len==0){
		return NULL;}
	char* d=(char*)dst;
	char* s=(char*)src;
	unsigned n=0;
	//if dst is potentially overlapping src
	if(d>s){
		d+=len;
		s+=len;
		while(n<len){
			*d=*s;
			++d;
			++s;
			++n;}}
	else{
		while(n<len){
			*d=*s;
			++d;
			++s;
			++n;}}
	return dst;
}

unsigned string::strlen(const char* str) const {
	unsigned x=0;
	while(str[x]!=0){
		++x;}
	return x;
}

//count the number of occurrences of str
unsigned string::countstr(const char* str,const unsigned size) const {
	unsigned x,pos=0,n=0;
	while(pos<=datalen){
		//check if we found the string
		for(x=0;x<size && pos<=datalen;++x,++pos){
			if(data[pos]!=str[x]){
				break;}}
		if(x==size){
			++n;}
		else{
			++pos;}}
	return n;
}

//replace every occurrence of str1 with str2 a maximum of num times
unsigned string::replacestr(const char* str1,const char* str2,const unsigned size1,const unsigned size2,const unsigned num) {
	unsigned c=countstr(str1,size1);
	if(datalen+(size2-size1)*c>datasize){
		resizedata(((size2-size1)*c)<<1);}
	unsigned x,pos=0,n=0;
	while(pos<=datalen && n<num){
		for(x=0;x<size1 && pos<=datalen;++x,++pos){
			if(data[pos]!=str1[x]){
				break;}}
		if(x==size1){
			memmove(data+pos+size2-size1,data+pos,datalen+size2-size1);
			memcpy(data+pos-size1,str2,size2);
			++n;}
		else{
			++pos;}}
	datalen+=(size2-size1)*n;
	return n;
}

//find the nth occurrence of str starting at x
unsigned string::findstr(const char* str,const unsigned s,unsigned x,const unsigned n) const {
	unsigned pos,c=0;
	for(x=0;x<datalen;++x){
		for(pos=0;pos<s;++pos){
			if(data[x+pos]!=str[pos]){
				break;}}
		if(pos==s){
			if(c==n){
				return x;}
			else{
				++c;}}}
	return datalen+1;
}

//find the nth occurrence of str starting at x going backwards
unsigned string::rfindstr(const char* str,const unsigned s,unsigned x,const unsigned n) const{
	unsigned pos,c=0;
	for(;x<datalen;--x){
		for(pos=0;pos<s;++pos){
			if(data[x+pos]!=str[pos]){
				break;}}
		if(pos==s){
			if(c==n){
				return x;}
			else{
				++c;}}
		else{
			x-=pos;}}
	return datalen+1;
}

string::string(){
	data=new char[32];
	data[0]=0;
	datasize=32;
	datalen=0;
}

string::string(const char c) {
	data = new char[32];
	data[0] = c;
	data[1] = 0;
	datasize = 32;
	datalen = 1;
}

string::string(const char* str){
	unsigned s=strlen(str);
	data=new char[s+minbuffsize];
	memcpy(data,str,s+1);
	datasize=s+minbuffsize;
	datalen=s;
}

string::string(const char *str, size_t len) {
	data = new char[len + minbuffsize];
	memcpy(data, str, len + 1);
	datasize = len + minbuffsize;
	datalen = len;
}

string::string(const string& str){
	data=new char[str.datasize];
	memcpy(data,str.data,str.datalen+1);
	datasize=str.datasize;
	datalen=str.datalen;
}

string::~string(){
	delete[] data;
}

string& string::operator=(const char c){
	if(2>datasize){
		resizedata(minbuffsize+1-datasize);}
	data[0]=c;
	data[1]=0;
	datalen=1;
	return *this;
}

string& string::operator=(const char* str){
	unsigned s=strlen(str);
	if(s+1>datasize){
		resizedata((s<<1)-datasize);}
	memcpy(data,str,s+1);
	datalen=s;
	return *this;
}

string& string::operator=(const string& str){
	datalen=str.datalen;
	if(datalen+1>datasize){
		resizedata((datalen<<1)-datasize);}
	memcpy(data,str.data,datalen+1);
	return *this;
}

string& string::operator+=(const char c){
	if(datalen+2>datasize){
		resizedata(minbuffsize);}
	data[datalen]=c;
	data[++datalen]=0;
	return *this;
}

string& string::operator+=(const char* str){
	unsigned s=strlen(str);
	if(datalen+s+1>datasize){
		resizedata(s<<1);}
	memcpy(data+datalen,str,s+1);
	datalen+=s;
	return *this;
}

string& string::operator+=(const string& str){
	if(datalen+str.datalen+1>datasize){
		resizedata(str.datalen<<2);}
	memcpy(data+datalen,str.data,str.datalen+1);
	datalen+=str.datalen;
	return *this;
}

string& string::operator<<(const char c) {
	return *this += c;
}

string& string::operator<<(const char *str) {
	return *this += str;
}

string& string::operator<<(const string& str) {
	return *this += str;
}

void string::clear(){
	data[0]=0;
}

unsigned string::insert(const char c,const unsigned pos){
	if(datalen+2>datasize){
		resizedata(minbuffsize);}
	memmove(data+pos+1,data+pos,datalen-pos);
	data[pos]=c;
	++datalen;
	return datalen;
}

unsigned string::insert(const char* str,const unsigned pos){
	unsigned s=strlen(str);
	if(datalen+s+1>datasize){
		resizedata(s<<2);}
	memmove(data+pos+s,data+pos,datalen-pos);
	memcpy(data+pos,str,s);
	datalen+=s;
	return datalen;
}

unsigned string::insert(const string& str,const unsigned pos){
	if(datalen+str.datalen+1>datasize){
		resizedata(str.datalen<<1);}
	memmove(data+pos+str.datalen,data+pos,datalen-pos);
	memcpy(data+pos,str.data,str.datalen);
	datalen+=str.datalen;
	return datalen;
}

//count
unsigned string::count(const char c) const{
	return countstr(&c,1);
}
unsigned string::count(const char* str) const{
	return countstr(str,strlen(str));
}
unsigned string::count(const string& str) const{
	return countstr(str.data,str.datalen);
}

//replace
unsigned string::replace(const char* str1,const char* str2,unsigned num=maxint){
	return replacestr(str1,str2,strlen(str1),strlen(str2),num);
}
unsigned string::replace(const string& str1,const char* str2,unsigned num=maxint){
	return replacestr(str1.data,str2,str1.datalen,strlen(str2),num);
}
unsigned string::replace(const char* str1,const string& str2,unsigned num=maxint){
	return replacestr(str1,str2.data,strlen(str1),str2.datalen,num);
}
unsigned string::replace(const string& str1,const string& str2,unsigned num=maxint){
	return replacestr(str1.data,str2.data,str1.datalen,str2.datalen,num);
}

//erase
unsigned string::erase(const unsigned start,const unsigned end){
	memmove(data+start,data+end,datalen-end+1);
	datalen-=end-start;
	return datalen;
}

//compare
int string::compare(const char *str) {
	return strcmp(this->c_str(), str);
}
int string::compare(string &str) {
	return strcmp(this->c_str(), str.c_str());
}
int string::compare(unsigned index, size_t len, const char *str) {
	return strncmp(this->c_str() + index, str, len);
}
int string::compare(unsigned index, size_t len, string &str) {
	return strncmp(this->c_str() + index, str.c_str(), len);
}

//find
unsigned string::find(const char c,unsigned n=0,unsigned start=0) const{
	return findstr(&c,1,start,n);
}
unsigned string::find(const char* str,unsigned n=0,unsigned start=0) const{
	return findstr(str,strlen(str),start,n);
}
unsigned string::find(const string& str,unsigned n=0,unsigned start=0) const{
	return findstr(str.data,str.datalen,start,n);
}

//rfind (reverse find)
unsigned string::rfind(const char c,unsigned n=0,unsigned start=maxint) const{
	if(start==maxint){
		start=datalen;}
	return rfindstr(&c,1,start,n);
}
unsigned string::rfind(const char* str,unsigned n=0,unsigned start=maxint) const{
	if(start==maxint){
		start=datalen;}
	return rfindstr(str,strlen(str),start,n);
}
unsigned string::rfind(const string& str,unsigned n=0,unsigned start=maxint) const{
	if(start==maxint){
		start=datalen;}
	return rfindstr(str,strlen(str),start,n);
}

//reserve
int string::reserve(const unsigned newsize){
	int size=(newsize>datasize?newsize:datasize);
	char* tmp=new char[size];
	memcpy(tmp,data,datalen);
	delete[] data;
	data=tmp;
	datasize=size;
	return size;
}

void string::upper(){
	unsigned pos=0;
	while(pos<=datalen){
		if(data[pos]>='a' && data[pos]<='z'){
			data[pos]-=aA_diff;
		}
		++pos;
	}
}

void string::lower(){
	unsigned pos=0;
	while(pos<=datalen){
		if(data[pos]>='A' && data[pos]<='Z'){
			data[pos]+=aA_diff;
		}
		++pos;
	}
}

unsigned string::length() const{
	return datalen;
}

unsigned string::capacity() const{
	return datasize;
}

//I couldn't use the ones defined in the string class, so I made an obscurely named namespace to hide them
//(this isn't bad practice, is it?)
namespace strexternalfuncs {
	bool isequal(const char* str1,const char* str2,const unsigned len1,const unsigned len2) {
		if(len1!=len2){
			return false;}
		unsigned pos=0;
		while(pos<len1){
			if(str1[pos]!=str2[pos]){
				return false;}
			++pos;}
		return true;
	}
	unsigned strlen(const char* str) {
		unsigned x = 0;
		while(str[x]!=0){
			++x;}
		return x;
	}
};

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
bool operator==(const string& str1,const string& str2) {
	return strexternalfuncs::isequal(str1.data,str2.data,str1.datalen,str2.datalen);
}

bool operator==(const string& str1,const char* str2) {
	return strexternalfuncs::isequal(str1.data,str2,str1.datalen,strexternalfuncs::strlen(str2));
}

bool operator==(const char* str1,const string& str2) {
	return strexternalfuncs::isequal(str1,str2.data,strexternalfuncs::strlen(str1),str2.datalen);
}

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
