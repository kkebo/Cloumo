#include "../headers.h"
#include <List.h>

using namespace HTML;

Node &Node::appendChild(Node &node) {
	children.append(node);
	return node;
}
