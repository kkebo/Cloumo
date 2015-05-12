#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

#include <List.h>

namespace HTML {
	class Node {
	private:
		List<Node> children;
	
	public:
		void appendChild(Node &node);
	};
}

#endif