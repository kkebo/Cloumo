#pragma once

#include <List.h>
#include <pistring.h>
#include <SmartPointer.h>

namespace HTML {
	class Token {
	private:
		// for StartTag and EndTag
		struct Attribute {
			string name;
			string value;
			
			Attribute(string _name, string _value);
		};
		bool selfClosingFlag = false;
		List<shared_ptr<Attribute>> attributes;
	
	public:
		// for all types
		enum class Type {
			Character, StartTag, EndTag, DOCTYPE, Comment, EndOfFile
		};
		const Type type;
		string data;
		
		// for all types
		explicit Token(Type tokenType);
		
		// for StartTag and EndTag
		void setSelfClosingFlag();
		bool isSelfClosing();
		void appendAttribute(char c);
		void appendAttributeName(char c);
		void appendAttributeValue(char c);
	};
}
