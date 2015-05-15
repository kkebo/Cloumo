#include "../headers.h"

using namespace HTML;

TreeConstructor::TreeConstructor() {
}

Document &TreeConstructor::construct(Queue<Token *> &tokens) {
	Element *nowElem, *child;
	
	while (!tokens->isempty()) {
		unique_ptr<Token> token(tokens.pop());
		
		switch (token->getType()) {
			case Token::Type::Character:
				nowElem->appendChild(new TextNode());
				break;
			
			case Token::Type::StartTag:
				child = nowElem->appendChild(new Element());
				if (!token->isSelfClosing()) {
					nowElem = child;
				}
				break;
			
			case Token::Type::EndTag:
				nowElem = nowElem->parent;
				break;
			
			case Token::Type::DOCTYPE:
				break;
			
			case Token::Type::Comment:
				break;
			
			case Token::Type::EndOfFile:
				break;
		}
	}
}
