#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

#include <List.h>

namespace HTML {
	class Node {
	private:
		List<Node> children;
	
	public:
		Node &appendChild(Node &node);
	};
}

#endif