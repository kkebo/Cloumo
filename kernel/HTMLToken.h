#ifndef _HTMLTOKEN_H_
#define _HTMLTOKEN_H_

#include <List.h>
#include <pistring.h>
#include <SmartPointer.h>

namespace HTML {
	class Token {
	private:
		// for StartTag and EndTag
		struct Attribute;
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
		Token(Type tokenType);
		Type getType();
		
		// for StartTag and EndTag
		void setSelfClosingFlag();
		bool isSelfClosing();
		void appendAttribute(char c);
		void appendAttributeName(char c);
		void appendAttributeValue(char c);
	};
}

#endif