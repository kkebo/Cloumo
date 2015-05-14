#ifndef _HTMLTOKEN_H_
#define _HTMLTOKEN_H_

#include <List.h>
#include <pistring.h>

namespace HTML {
	class Token {
	public:
		enum class Type {
			Character, StartTag, EndTag, DOCTYPE, Comment, EndOfFile
		};

	private:
		Type type;
		string data;
		
		// for StartTag and EndTag
		struct Attribute {
		public:
			string name;
			string value;
			
			Attribute() = default;
			Attribute(string &_name, string _value) : name(_name), value(_value) {}
		};
		bool selfClosingFlag = false;
		List<Attribute> attributes;
	
	public:
		Token(Type tokenType) : type(tokenType) {}
		void setData(string &str);
		string &getData();
		Type getType();
		
		// for StartTag and EndTag
		void setSelfClosingFlag();
		void addAttribute(string &name);
		void setAttributeValue(string &value);
	};
}

#endif