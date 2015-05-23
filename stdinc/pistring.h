/*Created by: PiMaster

Member Functions:
	inline const char* c_str() const - return the data of the string as a const c string
	clear() - clear the contents of the string (no return)
	insert(const char c,unsigned pos) - inserts a character at pos (returns new length)
	insert(const char* str,unsigned pos) - inserts a c-string at pos (returns new length)
	insert(const string& str,unsigned pos) - inserts a string at pos (returns new length)
	count(const char c) - returns the number of occurrences of c
	count(const char* str) - returns the number of occurrences of str
	count(const string& str) - returns the number of occurrences of str
	replace(const char* str1,const char* str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	replace(const string& str1,const char* str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	replace(const char* str1,const string& str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	replace(const string& str1,const string& str2) - replace all occurrences of str1 with str2 (returns number of occurrences)
	erase(unsigned start,unsigned end) - erase data between start and end (returns new length)
	find(const char c,unsigned n,unsigned start=0) - returns the position of the nth occurrence of c starting at start
	find(const char* str,unsigned n,unsigned start=0) - returns the position of the nth occurrence of str starting at start
	find(const string& str,unsigned n,unsigned start=0) - returns the position of the nth occurrence of str starting at start
	rfind(const char c,unsigned n,unsigned start) - returns the position of the nth occurrence of c going backwards starting at start
	rfind(const char* str,unsigned n,unsigned start) - returns the position of the nth occurrence of str going backwards starting at start
	rfind(const string& str,unsigned n,unsigned start) - returns the position of the nth occurrence of str going backwards starting at start
	reserve(unsigned newsize) - request a new maximum size for the string (returns new size)
		NOTE: If newsize is less than the current size, the reallocation will NOT happen
	upper() - make all letters in the string uppercase (no return)
	lower() - make all letters in the string lowercase (no return)
	length() - returns the length of the string
	capacity() - returns the capacity of the string (size of the buffer)
	reverse() - reverse the characters in the string (no return)

Member Operators:
	inline operator char*() - converts the string to a c string
	inline operator const char*() - converts the string to a const c string
	inline char& operator[](const int x) - returns the character at index x
		NOTE: If x is greater than the string length, it will return a reference to stroob
	inline const char& operator[](const int x) const - returns the const character at index x
		NOTE: If x is greater than the string length, it will return a reference to stroob
	string& operator=(const char c) - sets the string equal to c (nullptr terminated) (returns the string)
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

extern char stroob;

struct string{
	private:
	//memory related functions
		//resize the string data array
		void resizedata(unsigned newsize);
		//doesn't check for memory overlapping
		void* memcpy(void* dst,const void* src,unsigned len);
		//does check for memory overlapping
		void* memmove(void* dst,const void* src,unsigned len);
		unsigned strlen(const char* str) const;
	//functions that are called by other functions
		//count the number of occurrences of str
		unsigned countstr(const char* str,unsigned size) const;
		//replace every occurrence of str1 with str2 a maximum of num times
		unsigned replacestr(const char* str1,const char* str2,unsigned size1,unsigned size2,unsigned num);
		//find the nth occurrence of str starting at x
		unsigned findstr(const char* str,unsigned s,unsigned x,unsigned n) const;
		//find the nth occurrence of str starting at x going backwards
		unsigned rfindstr(const char* str,unsigned s,unsigned x,unsigned n) const;
		int comparestr(const char* str,unsigned first,unsigned last,unsigned start,unsigned end) const;
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
		//to_string
		template <typename T> friend string to_string(T n);
	//iterator
		friend struct iterator;
	protected:
		//data
		char* data;
		unsigned datasize,datalen;
	public:
		string();
		string(const char* str);
		string(const char *str, size_t len);
		string (size_t n, char c);
		string(const string& str);
		~string();
		inline operator const char*() const{
			return data;}
		inline const char* c_str(){
			if (data[datalen]) {
				// append NULL '\0'
				if (datalen + 1 > datasize) {
					resizedata(minbuffsize);
				}
				data[datalen + 1] = '\0';
			}
			return data;}
		inline char& operator[](unsigned x){
			if(x>datalen){
				return stroob;}
			return data[x];}
		inline const char& operator[](unsigned x) const{
			if(x>datalen){
				return stroob;}
			return data[x];}
		string& operator=(const char c);
		string& operator=(const char* str);
		string& operator=(const string& str);
		string& operator+=(const char c);
		string& operator+=(const char* str);
		string& operator+=(const string& str);
		void clear();
		unsigned insert(const char c,unsigned pos);
		unsigned insert(const char* str,unsigned pos);
		unsigned insert(const string& str,unsigned pos);
		//count
		unsigned count(const char c) const;
		unsigned count(const char* str) const;
		unsigned count(const string& str) const;
		//replace
		unsigned replace(const char* str1,const char* str2,unsigned num=maxint);
		unsigned replace(const string& str1,const char* str2,unsigned num=maxint);
		unsigned replace(const char* str1,const string& str2,unsigned num=maxint);
		unsigned replace(const string& str1,const string& str2,unsigned num=maxint);
		//erase
		unsigned erase(unsigned start,unsigned end);
		//find
		unsigned find(const char c,unsigned n=0,unsigned start=0) const;
		unsigned find(const char* str,unsigned n=0,unsigned start=0) const;
		unsigned find(const string& str,unsigned n=0,unsigned start=0) const;
		//rfind (reverse find)
		unsigned rfind(const char c,unsigned n=0,unsigned start=maxint) const;
		unsigned rfind(const char* str,unsigned n=0,unsigned start=maxint) const;
		unsigned rfind(const string& str,unsigned n=0,unsigned start=maxint) const;
		//reserve
		int reserve(const unsigned newsize);
		void upper();
		void lower();
		unsigned length() const;
		unsigned capacity() const;
		void reverse();
		//compare
		int compare(const char* str) const;
		int compare(const string& str) const;
		int compare(unsigned pos1,unsigned n1,const string& str) const;
		int compare(unsigned pos1,unsigned n1,const char* s) const;
		int compare(unsigned pos1,unsigned n1,const string& str,unsigned pos2,unsigned n2) const;
		int compare(unsigned pos1,unsigned n1,const char* s,unsigned n2) const;
		//iterator functions
		struct iterator{
			private:
				string* str;
				unsigned x;
				friend struct string;
			public:
				iterator& operator++();
				iterator& operator++(int);
				iterator& operator--();
				iterator& operator--(int);
				bool operator==(const iterator &it);
				bool operator!=(const iterator &it);
				iterator& operator+=(unsigned n);
				iterator& operator-=(unsigned n);
				char& operator*();};
		struct const_iterator{
			private:
				const string* str;
				unsigned x;
				friend struct string;
			public:
				const_iterator& operator++();
				const_iterator& operator++(int);
				const_iterator& operator--();
				const_iterator& operator--(int);
				bool operator==(const const_iterator &it);
				bool operator!=(const const_iterator &it);
				const_iterator& operator+=(unsigned n);
				const_iterator& operator-=(unsigned n);
				const char& operator*();};
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;};

//I couldn't use the ones defined in the string class, so I made an obscurely named namespace to hide them
//(this isn't bad practice, is it?)
namespace strexternalfuncs{
	bool isequal(const char* str1,const char* str2,unsigned len1,unsigned len2);
	unsigned strlen(const char* str);};

//global operators
//add
template <typename T, typename U>
string operator+ (T lhs, U rhs) {
	return string(lhs) += rhs;
}

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

// number to string
template <typename T>
string to_string(T n) {
	bool minus = false;
	string tmp;
	if (n < 0) {
		minus = true;
		n = -n;
	}
	while (n > 0) {
		tmp += char(n % 10 + '0');
		n /= 10;
	}
	if (minus) tmp += '-';
	tmp.reverse();
	return tmp;
}

#endif