#ifndef _HTMLTREECONSTRUCTOR_H_
#define _HTMLTREECONSTRUCTOR_H_

#include <Queue.h>
#include <SmartPointer.h>

namespace HTML {
	class TreeConstructor {
	private:
		enum class Mode {
			Initial,
			BeforeHtml,
			BeforeHead,
			InHead,
			InHeadNoscript,
			AfterHead,
			InBody,
			Text,
			InTable,
			InTableText,
			InCaption,
			InColumnGroup,
			InTableBody,
			InRow,
			InCell,
			InSelect,
			InSelectInTable,
			InTemplate,
			AfterBody,
			InFrameset,
			AfterFrameset,
			AfterAfterBody,
			AfterAfterFrameseet
		};
		Document document;
	
	public:
		TreeConstructor() = default;
		const Document &construct(Queue<shared_ptr<Token>> &tokens);
		void parseError();
	};
}

#endif