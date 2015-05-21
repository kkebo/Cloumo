#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

#include <List.h>
#include <SmartPointer.h>

namespace HTML {
	class Node {
	protected:
		List<shared_ptr<Node>> children;
	
	public:
		virtual ~Node() = default;
		virtual void appendChild(const shared_ptr<Node> &node);
		virtual const string &getTagName() {}
	};
}

#endif