#include "HTMLToken.h"

using namespace HTML;

Token::Attribute::Attribute(string _name, string _value) : name(_name), value(_value) {}

Token::Token(Type tokenType) : type(tokenType) {}

void Token::setSelfClosingFlag() {
	selfClosingFlag = true;
}

bool Token::isSelfClosing() {
	return selfClosingFlag;
}

void Token::appendAttribute(char c) {
	attributes.push_back(shared_ptr<Attribute>(new Attribute(string(1, c), "")));
}

void Token::appendAttributeName(char c) {
	attributes.back()->name += c;
}

void Token::appendAttributeValue(char c) {
	attributes.back()->value += c;
}
