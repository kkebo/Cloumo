/*
Created by: PiMaster
*/

#include <pistring.h>

char stroob=0;

//memory related functions
//resize the string data array
void string::resizedata(unsigned newsize){
	//minimum buffer size
	if(newsize<minbuffsize){
		newsize=minbuffsize;}
	char* tmp=new char[datasize+newsize];
	if(data!=nullptr){
		memcpy(tmp,data,datalen+1);
		delete[] data;}
	data=tmp;
	datasize+=newsize;}
//doesn't check for memory overlapping
void* string::memcpy(void* dst,const void* src,unsigned len){
	if(dst==nullptr || src==nullptr || len==0){
		return nullptr;}
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
void* string::memmove(void* dst,const void* src,unsigned len){
	if(dst==nullptr || src==nullptr || len==0){
		return nullptr;}
	char* d=(char*)dst;
	char* s=(char*)src;
	unsigned n=0;
	//if dst is potentially overlapping src
	if(d>s){
		d+=len-1;
		s+=len-1;
		while(n<len){
			*d=*s;
			--d;
			--s;
			++n;}}
	else{
		while(n<len){
			*d=*s;
			++d;
			++s;
			++n;}}
	return dst;}
unsigned string::strlen(const char* str) const{
	unsigned x=0;
	while(str[x]!=0){
		++x;}
	return x;}
//functions that are called by other functions
//count the number of occurrences of str
unsigned string::countstr(const char* str,unsigned size) const{
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
unsigned string::replacestr(const char* str1,const char* str2,unsigned size1,unsigned size2,unsigned num){
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
unsigned string::findstr(const char* str,unsigned s,unsigned x,unsigned n) const{
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
unsigned string::rfindstr(const char* str,unsigned s,unsigned x,unsigned n) const{
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
int string::comparestr(const char* str,unsigned first,unsigned last,unsigned start,unsigned end) const{
	unsigned pos=first;
	unsigned spos=start;
	while(pos<=last && spos<=end && data[pos]==str[spos]){
		++pos;
		++spos;}
	if(pos==last+1){
		return 0;}
	return data[pos]-str[spos];}
int string::compareistr(const char* str,unsigned first,unsigned last,unsigned start,unsigned end) const{
	unsigned pos=first;
	unsigned spos=start;
	char c1 = data[pos], c2 = str[spos];
	if ('A' <= c1 && c1 <= 'Z') c1 += 0x20;
	if ('A' <= c2 && c2 <= 'Z') c2 += 0x20;
	while(pos<=last && spos<=end && c1==c2){
		c1 = data[++pos];
		c2 = str[++spos];
		if ('A' <= c1 && c1 <= 'Z') c1 += 0x20;
		if ('A' <= c2 && c2 <= 'Z') c2 += 0x20;}
	if(pos==last+1){
		return 0;}
	return c1-c2;}
string::string(){
	data=new char[minbuffsize];
	data[0]=0;
	datasize=minbuffsize;
	datalen=0;}
string::string(const char* str){
	unsigned s=strlen(str);
	data=new char[s+minbuffsize];
	memcpy(data,str,s+1);
	datasize=s+minbuffsize;
	datalen=s;}
string::string(const char *str, size_t len) {
	data = new char[len + minbuffsize];
	memcpy(data, str, len + 1);
	datasize = len + minbuffsize;
	datalen = len;
}
string::string(size_t n, char c) {
	size_t i;
	data = new char[n + minbuffsize];
	for (i = 0; i < n; ++i) {
		data[i] = c;
	}
	data[i] = 0;
	datasize = n + minbuffsize;
	datalen = n;
}
string::string(const string& str){
	data=new char[str.datasize];
	memcpy(data,str.data,str.datalen+1);
	datasize=str.datasize;
	datalen=str.datalen;}
string::string(const string& str, unsigned pos, unsigned len){
	if (pos > str.datalen) {
		data = new char[minbuffsize];
		data[0] = 0;
		datasize = minbuffsize;
		datalen = 0;
	} else {
		if (pos + len > str.datalen) {
			len = str.datalen - pos;
		}
		
		data = new char[str.datasize - pos];
		memcpy(data, str.data + pos, len + 1);
		datasize = str.datasize - pos;
		datalen = len;
	}
}
string::~string(){
	delete[] data;}
string& string::operator=(const char c){
	if(2>datasize){
		resizedata(minbuffsize+1-datasize);}
	data[0]=c;
	data[1]=0;
	datalen=1;
	return *this;}
string& string::operator=(const char* str){
	unsigned s=strlen(str);
	if(s+1>datasize){
		resizedata((s<<1)-datasize);}
	memcpy(data,str,s+1);
	datalen=s;
	return *this;}
string& string::operator=(const string& str){
	datalen=str.datalen;
	if(datalen+1>datasize){
		resizedata((datalen<<1)-datasize);}
	memcpy(data,str.data,datalen+1);
	return *this;}
string& string::operator+=(const char c){
	if(datalen+2>datasize){
		resizedata(minbuffsize);}
	data[datalen]=c;
	data[datalen+1]=0;
	++datalen;
	return *this;}
string& string::operator+=(const char* str){
	unsigned s=strlen(str);
	if(datalen+s+1>datasize){
		resizedata(s<<1);}
	memcpy(data+datalen,str,s+1);
	datalen+=s;
	return *this;}
string& string::operator+=(const string& str){
	if(datalen+str.datalen+1>datasize){
		resizedata(str.datalen<<2);}
	memcpy(data+datalen,str.data,str.datalen+1);
	datalen+=str.datalen;
	return *this;}
void string::clear(){
	data[0]=0;}
unsigned string::insert(unsigned pos,unsigned n,const char c){
	if(datalen+n+1>datasize){
		resizedata(n<<1);}
	memmove(data+pos+n,data+pos,datalen-pos);
	for (unsigned i = 0; i < n; ++i) {
		data[pos+i]=c;
	}
	datalen+=n;
	return datalen;}
unsigned string::insert(unsigned pos,const char* str){
	unsigned s=strlen(str);
	if(datalen+s+1>datasize){
		resizedata(s<<1);}
	memmove(data+pos+s,data+pos,datalen-pos);
	memcpy(data+pos,str,s);
	datalen+=s;
	return datalen;}
unsigned string::insert(unsigned pos,const string& str){
	if(datalen+str.datalen+1>datasize){
		resizedata(str.datalen<<1);}
	memmove(data+pos+str.datalen,data+pos,datalen-pos);
	memcpy(data+pos,str.data,str.datalen);
	datalen+=str.datalen;
	return datalen;}
//count
unsigned string::count(const char c) const{
	return countstr(&c,1);}
unsigned string::count(const char* str) const{
	return countstr(str,strlen(str));}
unsigned string::count(const string& str) const{
	return countstr(str.data,str.datalen);}
//replace
unsigned string::replace(const char* str1,const char* str2,unsigned num){
	return replacestr(str1,str2,strlen(str1),strlen(str2),num);}
unsigned string::replace(const string& str1,const char* str2,unsigned num){
	return replacestr(str1.data,str2,str1.datalen,strlen(str2),num);}
unsigned string::replace(const char* str1,const string& str2,unsigned num){
	return replacestr(str1,str2.data,strlen(str1),str2.datalen,num);}
unsigned string::replace(const string& str1,const string& str2,unsigned num){
	return replacestr(str1.data,str2.data,str1.datalen,str2.datalen,num);}
//erase
unsigned string::erase(unsigned start,unsigned end){
	memmove(data+start,data+end,datalen-end+1);
	datalen-=end-start;
	return datalen;}
//find
unsigned string::find(const char c,unsigned n,unsigned start) const{
	return findstr(&c,1,start,n);}
unsigned string::find(const char* str,unsigned n,unsigned start) const{
	return findstr(str,strlen(str),start,n);}
unsigned string::find(const string& str,unsigned n,unsigned start) const{
	return findstr(str.data,str.datalen,start,n);}
//rfind (reverse find)
unsigned string::rfind(const char c,unsigned n,unsigned start) const{
	if(start==maxint){
		start=datalen;}
	return rfindstr(&c,1,start,n);}
unsigned string::rfind(const char* str,unsigned n,unsigned start) const{
	if(start==maxint){
		start=datalen;}
	return rfindstr(str,strlen(str),start,n);}
unsigned string::rfind(const string& str,unsigned n,unsigned start) const{
	if(start==maxint){
		start=datalen;}
	return rfindstr(str,strlen(str),start,n);}
//reserve
int string::reserve(const unsigned newsize){
	unsigned size=(newsize>datasize?newsize:datasize);
	char* tmp=new char[size];
	memcpy(tmp,data,datalen);
	delete[] data;
	data=tmp;
	datasize=size;
	return size;}
void string::upper(){
	unsigned pos=0;
	while(pos<=datalen){
		if(data[pos]>='a' && data[pos]<='z'){
			data[pos]-=aA_diff;}
		++pos;}}
void string::lower(){
	unsigned pos=0;
	while(pos<=datalen){
		if(data[pos]>='A' && data[pos]<='Z'){
			data[pos]+=aA_diff;}
		++pos;}}
unsigned string::length() const{
	return datalen;}
unsigned string::capacity() const{
	return datasize;}
void string::reverse(){
	if (datalen == 0) return;
	char* start=data;
	char* end=data+datalen-1;
	char buffer;
	while(end>start){
		buffer=*start;
		*start=*end;
		*end=buffer;
		++start;
		--end;}}
//compare
int string::compare(const char* str) const{
	return comparestr(str,0,datalen-1,0,strlen(str)-1);}
int string::compare(const string& str) const{
	return comparestr(str.data,0,datalen-1,0,str.datalen-1);}
int string::compare(unsigned pos1,unsigned n1,const string& str) const{
	return comparestr(str.data,pos1,pos1+n1-1,0,str.datalen-1);}
int string::compare(unsigned pos1,unsigned n1,const char* s) const{
	return comparestr(s,pos1,pos1+n1-1,0,strlen(s)-1);}
int string::compare(unsigned pos1,unsigned n1,const string& str,unsigned pos2,unsigned n2) const{
	return comparestr(str.data,pos1,pos1+n1-1,pos2,pos2+n2-1);}
int string::compare(unsigned pos1,unsigned n1,const char* s,unsigned n2) const{
	return comparestr(s,pos1,pos1+n1-1,0,n2-1);}
int string::comparei(const char* str) const{
	return compareistr(str,0,datalen-1,0,strlen(str)-1);}
int string::comparei(const string& str) const{
	return compareistr(str.data,0,datalen-1,0,str.datalen-1);}
int string::comparei(unsigned pos1,unsigned n1,const string& str) const{
	return compareistr(str.data,pos1,pos1+n1-1,0,str.datalen-1);}
int string::comparei(unsigned pos1,unsigned n1,const char* s) const{
	return compareistr(s,pos1,pos1+n1-1,0,strlen(s)-1);}
int string::comparei(unsigned pos1,unsigned n1,const string& str,unsigned pos2,unsigned n2) const{
	return compareistr(str.data,pos1,pos1+n1-1,pos2,pos2+n2-1);}
int string::comparei(unsigned pos1,unsigned n1,const char* s,unsigned n2) const{
	return compareistr(s,pos1,pos1+n1-1,0,n2-1);}
//iterator functions
string::iterator& string::iterator::operator++(){
	++x;
	if(x>str->datalen){
		x=str->datalen;}
	return *this;}
string::iterator& string::iterator::operator++(int){
	++x;
	if(x>str->datalen){
		x=str->datalen;}
	return *this;}
string::iterator& string::iterator::operator--(){
	--x;
	if(x==string::maxint){
		x=0;}
	return *this;}
string::iterator& string::iterator::operator--(int){
	--x;
	if(x==string::maxint){
		x=0;}
	return *this;}
bool string::iterator::operator==(const string::iterator &it) {
	return x == it.x;
}
bool string::iterator::operator!=(const string::iterator &it) {
	return x != it.x;
}
string::iterator& string::iterator::operator+=(unsigned n){
	x+=n;
	if(x>str->datalen){
		x=str->datalen;}
	return *this;}
string::iterator& string::iterator::operator-=(unsigned n){
	x-=n;
	if(x==string::maxint){
		x=0;}
	return *this;}
char& string::iterator::operator*(){
	return str->data[x];}
// const_iterator functions
string::const_iterator& string::const_iterator::operator++(){
	++x;
	if(x>str->datalen){
		x=str->datalen;}
	return *this;}
string::const_iterator& string::const_iterator::operator++(int){
	++x;
	if(x>str->datalen){
		x=str->datalen;}
	return *this;}
string::const_iterator& string::const_iterator::operator--(){
	--x;
	if(x==maxint){
		x=0;}
	return *this;}
string::const_iterator& string::const_iterator::operator--(int){
	--x;
	if(x==maxint){
		x=0;}
	return *this;}
bool string::const_iterator::operator==(const string::const_iterator &it) {
	return x == it.x;
}
bool string::const_iterator::operator!=(const string::const_iterator &it) {
	return x != it.x;
}
string::const_iterator& string::const_iterator::operator+=(unsigned n){
	x+=n;
	if(x>str->datalen){
		x=str->datalen;}
	return *this;}
string::const_iterator& string::const_iterator::operator-=(unsigned n){
	x-=n;
	if(x==string::maxint){
		x=0;}
	return *this;}
const char& string::const_iterator::operator*(){
	return str->data[x];}
string::iterator string::begin(){
	iterator ret;
	ret.str=this;
	ret.x=0;
	return ret;}
string::const_iterator string::begin() const{
	const_iterator ret;
	ret.str=this;
	ret.x=0;
	return ret;}
string::iterator string::end(){
	iterator ret;
	ret.str=this;
	ret.x=datalen;
	return ret;}
string::const_iterator string::end() const{
	const_iterator ret;
	ret.str=this;
	ret.x=datalen;
	return ret;}

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
