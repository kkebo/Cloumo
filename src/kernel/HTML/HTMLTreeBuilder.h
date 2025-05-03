#pragma once

#include <Queue.h>
#include <SmartPointer.h>
#include "HTMLToken.h"
#include "HTMLNode.h"

namespace HTML {
	class TreeBuilder {
	private:
		enum class Mode;
		Document document;
	
	public:
		TreeBuilder() {}
		Document &construct(Queue<shared_ptr<Token>> &tokens);
		void parseError();
	};
}
