#include "../headers.h"

using namespace HTML;

void Token::setData(string &str) {
	data = str;
	str = "";
}

string &Token::getData() {
	return data;
}

void Token::setSelfClosingFlag() {
	selfClosingFlag = true;
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
