#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

#include <List.h>

namespace HTML {
	class Node {
	protected:
		List<Node> children;
	
	public:
		Node *parent = nullptr;
		
		virtual Node &appendChild(Node &node);
	};
}

#endif