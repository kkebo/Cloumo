#ifndef _HTMLTOKEN_H_
#define _HTMLTOKEN_H_

namespace HTML {
	class Token {
	private:
		
	};
	
	class TagToken : Token {
	private:
		char tagName[100];
		bool selfClosingFlag = false;
	
	public:
		TagToken(bool isOpenTag);
		TagToken(char tagName, bool isOpenTag);
		void appendTagName(char c);
	};
}

#endif