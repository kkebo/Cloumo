#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

#include <List.h>
#include <SmartPointer.h>

namespace HTML {
	class Element;
	class TextNode;
	
	class Node {
	protected:
		List<shared_ptr<Node>> children;
	
	public:		
		virtual void appendChild(const shared_ptr<Node> &node);
		virtual void appendChild(const shared_ptr<Element> &node);
		virtual void appendChild(const shared_ptr<TextNode> &node);
	};
}

#endif