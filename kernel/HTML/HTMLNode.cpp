#include "HTMLNode.h"

using namespace HTML;

// Node
const shared_ptr<Node> &Node::appendChild(const shared_ptr<Node> &node) {
	children.push_back(node);
	return node;
}

// Element
Element::Element(const string &name) : _tagName(name) {}
