#include "../headers.h"

using namespace HTML;

void Node::appendChild(const shared_ptr<Node> &node) {
	children.append(node);
}
