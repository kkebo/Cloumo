#ifndef _HTMLTOKENIZER_H_
#define _HTMLTOKENIZER_H_

namespace HTML {
	class Tokenizer {
	private:
		
	
	public:
		tokenize(const char *inputStream);
	
	private:
		emitCharacterToken(const char *);
		emitEOFToken();
		emitTagToken(TagToken *);
	};
}

#endif
