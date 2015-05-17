#ifndef _HTMLELEMENT_H_
#define _HTMLELEMENT_H_

namespace HTML {
	class Element : public Node {
	public:
		string tagName;
		string id;
		string className;
		
		Element() {}
		Element(const string &name, const List<Token::Attribute> &attr);
	};
}

#endif