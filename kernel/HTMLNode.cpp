#include "HTMLNode.h"

using namespace HTML;

// Node
void Node::appendChild(const shared_ptr<Node> &node) {
	children.append(node);
}

// Element
Element::Element(const string &name) : tagName(name) {}

const string &Element::getTagName() {
	return tagName;
}
