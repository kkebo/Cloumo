#pragma once

#include <Queue.h>
#include <pistring.h>
#include <SmartPointer.h>
#include "HTMLToken.h"

namespace HTML {
	class Tokenizer {
	private:
		enum class State;
		Queue<shared_ptr<Token>> tokens;
		
		void emitCharacterToken(char c);
		void emitEOFToken();
		void emitToken(unique_ptr<Token> &token);
		void parseError();
	
	public:
		Tokenizer();
		Queue<shared_ptr<Token>> &tokenize(const string &inputStream);
	};
}

