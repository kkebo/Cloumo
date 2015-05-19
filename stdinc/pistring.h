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
		void resizedata(unsigned newsize);
		//doesn't check for memory overlapping
		void* memcpy(void* dst,const void* src,const unsigned len);
		//does check for memory overlapping
		void* memmove(void* dst,const void* src,unsigned len);
		unsigned strlen(const char* str) const;
	//functions that are called by other functions
		//count the number of occurrences of str
		unsigned countstr(const char* str,const unsigned size) const;
		//replace every occurrence of str1 with str2 a maximum of num times
		unsigned replacestr(const char* str1,const char* str2,const unsigned size1,const unsigned size2,const unsigned num);
		//find the nth occurrence of str starting at x
		unsigned findstr(const char* str,const unsigned s,unsigned x,const unsigned n) const;
		//find the nth occurrence of str starting at x going backwards
		unsigned rfindstr(const char* str,const unsigned s,unsigned x,const unsigned n) const;
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
		string();
		string(const char c);
		string(const char* str);
		string(const char *str, size_t len);
		string(const string& str);
		~string();
		inline operator const char*() const{
			return data;}
		inline char* c_str() {
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
		string& operator=(const char c);
		string& operator=(const char* str);
		string& operator=(const string& str);
		string& operator+=(const char c);
		string& operator+=(const char* str);
		string& operator+=(const string& str);
		string& operator<<(const char c);
		string& operator<<(const char *str);
		string& operator<<(const string& str);
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
		void clear();
		unsigned insert(const char c,const unsigned pos);
		unsigned insert(const char* str,const unsigned pos);
		unsigned insert(const string& str,const unsigned pos);
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
		unsigned erase(const unsigned start,const unsigned end);
		//compare
		int compare(const char *str);
		int compare(string &str);
		int compare(unsigned index, size_t len, const char *str);
		int compare(unsigned index, size_t len, string &str);
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
};

//I couldn't use the ones defined in the string class, so I made an obscurely named namespace to hide them
//(this isn't bad practice, is it?)
namespace strexternalfuncs{
	bool isequal(const char* str1,const char* str2,const unsigned len1,const unsigned len2);
	unsigned strlen(const char* str);
};

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