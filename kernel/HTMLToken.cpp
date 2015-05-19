#include "../headers.h"

using namespace HTML;

Token::Attribute::Attribute(string _name, string _value) : name(_name), value(_value) {}

Token::Token(Type tokenType) : type(tokenType) {}

Token::Type Token::getType() {
	return type;
}

void Token::setSelfClosingFlag() {
	selfClosingFlag = true;
}

bool Token::isSelfClosing() {
	return selfClosingFlag;
}

void Token::appendAttribute(char c) {
	attributes.append(shared_ptr<Attribute>(new Attribute(string(c), "")));
}

void Token::appendAttributeName(char c) {
	attributes.getLast()->name += c;
}

void Token::appendAttributeValue(char c) {
	attributes.getLast()->value += c;
}
