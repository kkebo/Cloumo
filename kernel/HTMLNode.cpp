#include "../headers.h"
#include <List.h>

using namespace HTML;

Node &Node::appendChild(Node &node) {
	node.parent = this;
	children.append(node);
	return node;
}
