#ifndef _HTMLTEXTNODE_H_
#define _HTMLTEXTNODE_H_

namespace HTML {
	class TextNode : public Node {
	public:
		string wholeText;
		
		TextNode(string str) : wholeText(str) {}
	};
}

#endif