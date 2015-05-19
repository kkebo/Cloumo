#include "../headers.h"

using namespace HTML;

void Node::appendChild(const shared_ptr<Node> &node) {
	children.append(node);
}

void Node::appendChild(const shared_ptr<Element> &node) {
	appendChild(static_cast<shared_ptr<Node>>(node));
}

void Node::appendChild(const shared_ptr<TextNode> &node) {
	appendChild(static_cast<shared_ptr<Node>>(node));
}
