#ifndef _HTMLTOKEN_H_
#define _HTMLTOKEN_H_

#include <List.h>

namespace HTML {
	class Token {
	public:
		enum class Type {
			Character, StartTag, EndTag, DOCTYPE, Comment, EndOfFile
		};

	private:
		Type type;
		char data[100];
		
		// for StartTag and EndTag
		struct Attribute {
			char name[50];
			char value[128];
		};
		bool selfClosingFlag = false;
		List<Attribute> attributes;
	
	public:
		Token(Type tokenType) : type(tokenType) {}
		void setData(const char *str);
		char *getData();
		
		// for StartTag and EndTag
		void setSelfClosingFlag();
		void addAttribute(const char *name);
		void setAttributeValue(const char *value);
	};
}

#endif