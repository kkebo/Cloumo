#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

namespace HTML {
	class Node {
	private:
		List<Node> children;
	
	public:
		void appendChild(Node &node);
	};
}

#endif