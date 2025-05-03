#pragma once

#include <List.h>
#include <SmartPointer.h>
#include <pistring.h>

namespace HTML {
	class Node {
	public:
		List<shared_ptr<Node>> children;
		
		virtual ~Node() {}
		virtual string getData() {
			return "";
		}
		const shared_ptr<Node> &appendChild(const shared_ptr<Node> &node);
	};
	
	class Element : public Node {
	private:
		string _tagName;
		string id;
		string className;
	
	public:
		//const string &tagName = _tagName;
	
		//Element() {}
		Element(const string &name);
		string getData() {
			return "<" + _tagName + ">";
		}
	};
	
	class TextNode : public Node {
	public:
		string wholeText;
		
		TextNode(string str) : wholeText(str) {}
		string getData() {
			return wholeText;
		}
	};
	
	class DocumentType : public Node {
	public:
		string name;
		string publicId = "";
		string systemId = "";
		
		explicit DocumentType(const string &n) : name(n) {}
		DocumentType(const string &n, const string &p, const string &s) : name(n), publicId(p), systemId(s) {}
		string getData() {
			return "<!doctype " + name + ">";
		}
	};
	
	class Comment : public Node {
	public:
		explicit Comment(string data = "");
		
		string getData() {
			return "<!-- コメント -->";
		}
	};
	
	class Document : public Node {
	public:
		//DocumentType doctype;
		//Element documentElement;
		
		string getData() {
			return "Document ノード";
		}
	};
}
