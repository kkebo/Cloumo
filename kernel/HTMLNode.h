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
		virtual void appendChild(const shared_ptr<Node> &node);
	};
	
	class Element : public Node {
	private:
		string _tagName;
		string id;
		string className;
	
	public:
		const string &tagName = _tagName;
	
		//Element() {}
		Element(const string &name);
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