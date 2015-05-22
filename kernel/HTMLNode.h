#ifndef _HTMLNODE_H_
#define _HTMLNODE_H_

#include <List.h>
#include <SmartPointer.h>
#include <pistring.h>

namespace HTML {
	class Node {
	protected:
		List<shared_ptr<Node>> children;
	
	public:
		virtual ~Node() = default;
		virtual void appendChild(const shared_ptr<Node> &node);
		virtual const string &getTagName() {}
	};
	
	class Element : public Node {
	private:
		string tagName;
		string id;
		string className;
	
	public:
		//Element() {}
		Element(const string &name);
		const string &getTagName();
	};
	
	class TextNode : public Node {
	public:
		string wholeText;
		
		TextNode(string str) : wholeText(str) {}
	};
	
	class DocumentType : public Node {
	public:
		string name;
		string publicId = "";
		string systemId = "";
		
		DocumentType(const string &n) : name(n) {}
		DocumentType(const string &n, const string &p, const string &s) : name(n), publicId(p), systemId(s) {}
	};
	
	class Comment : public Node {
	public:
		Comment(string data = "");
	};
	
	class Document : public Node {
	public:
		//DocumentType doctype;
		//Element documentElement;
	};
}

#endif