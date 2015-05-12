#include "../headers.h"
#include <List.h>

using namespace HTML;

void Node::appendChild(Node &node) {
	children.append(node);
}
