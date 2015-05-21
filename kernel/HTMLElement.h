#ifndef _HTMLELEMENT_H_
#define _HTMLELEMENT_H_

namespace HTML {
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
}

#endif