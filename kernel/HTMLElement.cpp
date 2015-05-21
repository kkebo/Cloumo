#include "../headers.h"

using namespace HTML;

Element::Element(const string &name) : tagName(name) {}

const string &Element::getTagName() {
	return tagName;
}
