/*Created by: PiMaster

Member Functions:
	inline const char* c_str() const - return the data of the string as a const c string
	clear() - clear the contents of the string (no return)
	insert(const char c,const unsigned pos) - inserts a character at pos (returns new length)
	insert(const char* str,const unsigned pos) - inserts a c-string at pos (returns new length)
	insert(const string& str,const unsigned pos) - inserts a string at pos (returns new length)
	count(const char c) - returns the number of occurrences of c
	count(const char* str) - returns the number of occurrences of str
	count(const string& str) - returns the number of occurrences of str
	replace(const char* str1,const char* str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	replace(const string& str1,const char* str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	replace(const char* str1,const string& str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	replace(const string& str1,const string& str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	erase(const unsigned start,const unsigned end) - erase data between start and end (returns new length)
	find(const char c,unsigned n,unsigned start=0) - returns the position of the nth occurrence of c starting at start
	find(const char* str,unsigned n,unsigned start=0) - returns the position of the nth occurrence of str starting at start
	find(const string& str,unsigned n,unsigned start=0) - returns the position of the nth occurrence of str starting at start
	rfind(const char c,unsigned n,unsigned start) - returns the position of the nth occurrence of c going backwards starting at start
	rfind(const char* str,unsigned n,unsigned start) - returns the position of the nth occurrence of str going backwards starting at start
	rfind(const string& str,unsigned n,unsigned start) - returns the position of the nth occurrence of str going backwards starting at start
	reserve(const unsigned newsize) - request a new maximum size for the string (returns new size)
		NOTE: If newsize is less than the current size, the reallocation will NOT happen
	upper() - make all letters in the string uppercase (no return)
	lower() - make all letters in the string lowercase (no return)
	length() - returns the length of the string
	capacity() - returns the capacity of the string (size of the buffer)

Member Operators:
	inline operator char*() - converts the string to a c string
	inline operator const char*() - converts the string to a const c string
	inline char& operator[](const int x) - returns the character at index x
		NOTE: If x is greater than the string length, it will return a reference to stroob
	inline const char& operator[](const int x) const - returns the const character at index x
		NOTE: If x is greater than the string length, it will return a reference to stroob
	string& operator=(const char c) - sets the string equal to c (NULL terminated) (returns the string)
	string& operator=(const char* str) - sets the string equal to str (returns the string)
	string& operator=(const string& str) - sets the string equal to str (returns the string)
	string& operator+=(const char c) - appends c to the string (returns the string)
	string& operator+=(const char* str) - appends str to the string (returns the string)
	string& operator+=(cosnt string& str) - appends str to the string (returns the string)

Global Operators:
	bool operator==(const string& str1,const string& str2) - returns if str1 is equal to str2
	bool operator==(const string& str1,const char* str2) - returns if str1 is equal to str2
	bool operator==(const char* str1,const string& str2) - returns if str1 is equal to str2
	bool operator!=(const string& str1,const string& str2) - returns if str1 is not equal to str2
	bool operator!=(const string& str1,const char* str2) - returns if str1 is not equal to str2
	bool operator!=(const char* str1,const string& str2) - returns if str1 is not equal to str2
	bool operator>(const string& str1,const string& str2) - returns if the length of str1 is greater than the length of str2
	bool operator>(const string& str1,const char* str2) - returns if the length of str1 is greater than the length of str2
	bool operator>(const char* str1,const string& str2) - returns if the length of str1 is greater than the length of str2
	bool operator<(const string& str1,const string& str2) - returns if the length of str1 is less than the length of str2
	bool operator<(const string& str1,const char* str2) - returns if the length of str1 is less than the length of str2
	bool operator<(const char* str1,const string& str2) - returns if the length of str1 is less than the length of str2
	bool operator>=(const string& str1,const string& str2) - returns if the length of str1 is greater than or equal to the length of str2
	bool operator>=(const string& str1,const char* str2) - returns if the length of str1 is greater than or equal to the length of str2
	bool operator>=(const char* str1,const string& str2) - returns if the length of str1 is greater than or equal to the length of str2
	bool operator<=(const string& str1,const string& str2) - returns if the length of str1 is less than or equal to the length of str2
	bool operator<=(const string& str1,const char* str2) - returns if the length of str1 is less than or equal to the length of str2
	bool operator<=(const char* str1,const string& str2) - returns if the length of str1 is less than or equal to the length of str2

*/

#ifndef _PISTRING_H_
#define _PISTRING_H_

#include <stddef.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

extern char stroob;

struct string{
	private:
	//memory related functions
		//resize the string data array
		void resizedata(unsigned newsize){
			//minimum buffer size
			if(newsize<minbuffsize){
				newsize=minbuffsize;}
			char* tmp=new char[datasize+newsize];
			if(data!=NULL){
				memcpy(tmp,data,datalen+1);
				delete[] data;}
			data=tmp;
			datasize+=newsize;}
		//doesn't check for memory overlapping
		void* memcpy(void* dst,const void* src,const unsigned len){
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
			return dst;}
		//does check for memory overlapping
		void* memmove(void* dst,const void* src,unsigned len){
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
			return dst;}
		unsigned strlen(const char* str) const{
			unsigned x=0;
			while(str[x]!=0){
				++x;}
			return x;}
	//functions that are called by other functions
		//count the number of occurrences of str
		unsigned countstr(const char* str,const unsigned size) const{
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
			return n;}
		//replace every occurrence of str1 with str2 a maximum of num times
		unsigned replacestr(const char* str1,const char* str2,const unsigned size1,const unsigned size2,const unsigned num){
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
			return n;}
		//find the nth occurrence of str starting at x
		unsigned findstr(const char* str,const unsigned s,unsigned x,const unsigned n) const{
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
			return datalen+1;}
		//find the nth occurrence of str starting at x going backwards
		unsigned rfindstr(const char* str,const unsigned s,unsigned x,const unsigned n) const{
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
			return datalen+1;}
	//constants
		//maximum unsigned integer
		const static unsigned maxint=~0;
		//difference between 'a' and 'A' (used in upper() and lower() functions)
		const static unsigned aA_diff='a'-'A';
		//the maximum buffer size
		const static unsigned minbuffsize=32;
	//friends
		//operators
		//equal
		friend bool operator==(const string&,const string&);
		friend bool operator==(const string&,const char*);
		friend bool operator==(const char*,const string&);
		//not equal
		friend bool operator!=(const string&,const string&);
		friend bool operator!=(const string&,const char*);
		friend bool operator!=(const char*,const string&);
		//greater than
		friend bool operator>(const string&,const string&);
		friend bool operator>(const string&,const char*);
		friend bool operator>(const char*,const string&);
		//less than
		friend bool operator<(const string&,const string&);
		friend bool operator<(const string&,const char*);
		friend bool operator<(const char*,const string&);
		//greater than or equal to
		friend bool operator>=(const string&,const string&);
		friend bool operator>=(const string&,const char*);
		friend bool operator>=(const char*,const string&);
		//less than or equal to
		friend bool operator<=(const string&,const string&);
		friend bool operator<=(const string&,const char*);
		friend bool operator<=(const char*,const string&);
	protected:
		//data
		char* data;
		unsigned datasize,datalen;
	public:
		string(){
			data=new char[32];
			data[0]=0;
			datasize=32;
			datalen=0;}
		string(const char c) {
			data = new char[32];
			data[0] = c;
			data[1] = 0;
			datasize = 32;
			datalen = 1;
		}
		string(const char* str){
			unsigned s=strlen(str);
			data=new char[s+minbuffsize];
			memcpy(data,str,s+1);
			datasize=s+minbuffsize;
			datalen=s;}
		string(const char *str, size_t len) {
			data = new char[len + minbuffsize];
			memcpy(data, str, len + 1);
			datasize = len + minbuffsize;
			datalen = len;
		}
		string(const string& str){
			data=new char[str.datasize];
			memcpy(data,str.data,str.datalen+1);
			datasize=str.datasize;
			datalen=str.datalen;}
		~string(){
			delete[] data;}
		inline operator const char*() const{
			return data;}
		inline const char* c_str() {
			if (data[datalen] != 0) *this += '\0';
			return data;}
		inline char& operator[](const unsigned x){
			if(x>datalen){
				return stroob;}
			return data[x];}
		inline const char& operator[](const unsigned x) const{
			if(x>datalen){
				return stroob;}
			return data[x];}
		string& operator=(const char c){
			if(2>datasize){
				resizedata(minbuffsize+1-datasize);}
			data[0]=c;
			data[1]=0;
			datalen=1;
			return *this;}
		string& operator=(const char* str){
			unsigned s=strlen(str);
			if(s+1>datasize){
				resizedata((s<<1)-datasize);}
			memcpy(data,str,s+1);
			datalen=s;
			return *this;}
		string& operator=(const string& str){
			datalen=str.datalen;
			if(datalen+1>datasize){
				resizedata((datalen<<1)-datasize);}
			memcpy(data,str.data,datalen+1);
			return *this;}
		string& operator+=(const char c){
			if(datalen+2>datasize){
				resizedata(minbuffsize);}
			data[datalen]=c;
			data[++datalen]=0;
			return *this;}
		string& operator+=(const char* str){
			unsigned s=strlen(str);
			if(datalen+s+1>datasize){
				resizedata(s<<1);}
			memcpy(data+datalen,str,s+1);
			datalen+=s;
			return *this;}
		string& operator+=(const string& str){
			if(datalen+str.datalen+1>datasize){
				resizedata(str.datalen<<2);}
			memcpy(data+datalen,str.data,str.datalen+1);
			datalen+=str.datalen;
			return *this;}
		string& operator<<(const char c) {
			return *this += c;
		}
		string& operator<<(const char *str) {
			return *this += str;
		}
		string& operator<<(const string& str) {
			return *this += str;
		}
		template <typename T>
		string& operator<<(T n) {
			string tmp;
			if (n < 0) {
				*this += '-';
				n = -n;
			}
			while (n > 0) {
				tmp += char(n % 10 + '0');
				n /= 10;
			}
			size_t i = tmp.datalen;
			while (i > 0) {
				*this += tmp[--i];
			};
			return *this;
		}
		void clear(){
			data[0]=0;}
		unsigned insert(const char c,const unsigned pos){
			if(datalen+2>datasize){
				resizedata(minbuffsize);}
			memmove(data+pos+1,data+pos,datalen-pos);
			data[pos]=c;
			++datalen;
			return datalen;}
		unsigned insert(const char* str,const unsigned pos){
			unsigned s=strlen(str);
			if(datalen+s+1>datasize){
				resizedata(s<<2);}
			memmove(data+pos+s,data+pos,datalen-pos);
			memcpy(data+pos,str,s);
			datalen+=s;
			return datalen;}
		unsigned insert(const string& str,const unsigned pos){
			if(datalen+str.datalen+1>datasize){
				resizedata(str.datalen<<1);}
			memmove(data+pos+str.datalen,data+pos,datalen-pos);
			memcpy(data+pos,str.data,str.datalen);
			datalen+=str.datalen;
			return datalen;}
		//count
		unsigned count(const char c) const{
			return countstr(&c,1);}
		unsigned count(const char* str) const{
			return countstr(str,strlen(str));}
		unsigned count(const string& str) const{
			return countstr(str.data,str.datalen);}
		//replace
		unsigned replace(const char* str1,const char* str2,unsigned num=maxint){
			return replacestr(str1,str2,strlen(str1),strlen(str2),num);}
		unsigned replace(const string& str1,const char* str2,unsigned num=maxint){
			return replacestr(str1.data,str2,str1.datalen,strlen(str2),num);}
		unsigned replace(const char* str1,const string& str2,unsigned num=maxint){
			return replacestr(str1,str2.data,strlen(str1),str2.datalen,num);}
		unsigned replace(const string& str1,const string& str2,unsigned num=maxint){
			return replacestr(str1.data,str2.data,str1.datalen,str2.datalen,num);}
		//erase
		unsigned erase(const unsigned start,const unsigned end){
			memmove(data+start,data+end,datalen-end+1);
			datalen-=end-start;
			return datalen;}
		//compare
		int compare(const char *str) {
			return strcmp(this->c_str(), str);
		}
		int compare(string &str) {
			return strcmp(this->c_str(), str.c_str());
		}
		int compare(unsigned index, size_t len, const char *str) {
			return strncmp(this->c_str() + index, str, len);
		}
		int compare(unsigned index, size_t len, string &str) {
			return strncmp(this->c_str() + index, str.c_str(), len);
		}
		//find
		unsigned find(const char c,unsigned n=0,unsigned start=0) const{
			return findstr(&c,1,start,n);}
		unsigned find(const char* str,unsigned n=0,unsigned start=0) const{
			return findstr(str,strlen(str),start,n);}
		unsigned find(const string& str,unsigned n=0,unsigned start=0) const{
			return findstr(str.data,str.datalen,start,n);}
		//rfind (reverse find)
		unsigned rfind(const char c,unsigned n=0,unsigned start=maxint) const{
			if(start==maxint){
				start=datalen;}
			return rfindstr(&c,1,start,n);}
		unsigned rfind(const char* str,unsigned n=0,unsigned start=maxint) const{
			if(start==maxint){
				start=datalen;}
			return rfindstr(str,strlen(str),start,n);}
		unsigned rfind(const string& str,unsigned n=0,unsigned start=maxint) const{
			if(start==maxint){
				start=datalen;}
			return rfindstr(str,strlen(str),start,n);}
		//reserve
		int reserve(const unsigned newsize){
			int size=(newsize>datasize?newsize:datasize);
			char* tmp=new char[size];
			memcpy(tmp,data,datalen);
			delete[] data;
			data=tmp;
			datasize=size;
			return size;}
		void upper(){
			unsigned pos=0;
			while(pos<=datalen){
				if(data[pos]>='a' && data[pos]<='z'){
					data[pos]-=aA_diff;}
				++pos;}}
		void lower(){
			unsigned pos=0;
			while(pos<=datalen){
				if(data[pos]>='A' && data[pos]<='Z'){
					data[pos]+=aA_diff;}
				++pos;}}
		unsigned length() const{
			return datalen;}
		unsigned capacity() const{
			return datasize;}};

//I couldn't use the ones defined in the string class, so I made an obscurely named namespace to hide them
//(this isn't bad practice, is it?)
namespace strexternalfuncs{
	bool isequal(const char* str1,const char* str2,const unsigned len1,const unsigned len2);
	unsigned strlen(const char* str);};

//global operators
//add
string operator + (const char c, const string &str);
string operator + (const string &str, const char c);
string operator + (const char *str1, const string &str2);
string operator + (const string &str1, const char *str2);
string operator + (const string &str1, const string &str2);

//equal
bool operator==(const string& str1,const string& str2);
bool operator==(const string& str1,const char* str2);
bool operator==(const char* str1,const string& str2);

//not equal
bool operator!=(const string& str1,const string& str2);
bool operator!=(const string& str1,const char* str2);
bool operator!=(const char* str1,const string& str2);

//greater than
bool operator>(const string& str1,const string& str2);
bool operator>(const string& str1,const char* str2);
bool operator>(const char* str1,const string& str2);

//less than
bool operator<(const string& str1,const string& str2);
bool operator<(const string& str1,const char* str2);
bool operator<(const char* str1,const string& str2);

//greater than or equal to
bool operator>=(const string& str1,const string& str2);
bool operator>=(const string& str1,const char* str2);
bool operator>=(const char* str1,const string& str2);

//less than or equal to
bool operator<=(const string& str1,const string& str2);
bool operator<=(const string& str1,const char* str2);
bool operator<=(const char* str1,const string& str2);

#endif