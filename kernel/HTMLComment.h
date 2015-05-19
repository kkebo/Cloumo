#ifndef _HTMLCOMMENT_H_
#define _HTMLCOMMENT_H_

namespace HTML {
	class Comment : public Node {
	public:
		Comment(string data = "");
	};
}

#endif