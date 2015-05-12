#include "../headers.h"
#include <string.h>

using namespace HTML;

void Token::setData(const char *str) {
	strcpy(data, str);
}

char *Token::getData() {
	return data;
}

void Token::setSelfClosingFlag() {
	selfClosingFlag = true;
}

void Token::addAttribute(const char *name) {
	Attribute attr;
	strcpy(attr.name, name);
	attr.value[0] = 0;
	attributes.append(attr);
}

void Token::setAttributeValue(const char *value) {
	if (!attributes.isEmpty()) {
		strcpy(attributes.getLast().value, value);
	}
}
