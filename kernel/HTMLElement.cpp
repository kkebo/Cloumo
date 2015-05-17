#include "../headers.h"

using namespace HTML;

Element::Element(const string &name, const List<Token::Attribute> &attr) : tagName(name) {}
