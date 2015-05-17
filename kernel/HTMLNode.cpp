#include "../headers.h"
#include <List.h>

using namespace HTML;

void Node::appendChild(const shared_ptr<Node> &node) {
	node->parent = this;
	children.append(node);
}
