#include "../headers.h"

using namespace HTML;

Token::Type Token::getType() {
	return type;
}

void Token::setSelfClosingFlag() {
	selfClosingFlag = true;
}

bool Token::isSelfClosingFlag() {
	return selfClosingFlag;
}

void Token::addAttribute(string &name) {
	attributes.append(Attribute(name, ""));
	name = "";
}

void Token::setAttributeValue(string &value) {
	if (!attributes.isEmpty()) {
		attributes.getLast().value = value;
	}
	value = "";
}
