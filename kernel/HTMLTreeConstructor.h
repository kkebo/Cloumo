#ifndef _HTMLTREECONSTRUCTOR_H_
#define _HTMLTREECONSTRUCTOR_H_

#include <Queue.h>
#include <SmartPointer.h>

namespace HTML {
	class TreeConstructor {
	private:
		enum class Mode;
		Document document;
	
	public:
		TreeConstructor() {}
		const Document &construct(Queue<shared_ptr<Token>> &tokens);
		void parseError();
	};
}

#endif