#ifndef _HTMLTREECONSTRUCTOR_H_
#define _HTMLTREECONSTRUCTOR_H_

#include <Queue.h>

namespace HTML {
	class TreeConstructor {
	private:
		Document document;
	
	public:
		TreeConstructor();
		Document &construct(Queue<Token *> &tokens);
	};
}

#endif