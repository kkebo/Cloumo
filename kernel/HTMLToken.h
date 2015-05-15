#ifndef _HTMLTOKEN_H_
#define _HTMLTOKEN_H_

#include <List.h>
#include <pistring.h>

namespace HTML {
	class Token {
	private:
		// for StartTag and EndTag
		bool selfClosingFlag = false;
	
	public:
		enum class Type {
			Character, StartTag, EndTag, DOCTYPE, Comment, EndOfFile
		};
		
		const Type type;
		string data;
		
		// for StartTag and EndTag
		struct Attribute {
		public:
			string name;
			string value;
			
			Attribute() = default;
			Attribute(string _name, string _value) : name(_name), value(_value) {}
		};
		List<Attribute> attributes;
		
		Token(Type tokenType) : type(tokenType) {}
		Type getType();
		
		// for StartTag and EndTag
		void setSelfClosingFlag();
		void addAttribute(string &name);
		void setAttributeValue(string &value);
	};
}

#endif