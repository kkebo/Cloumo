#ifndef _HTMLTREECONSTRUCTOR_H_
#define _HTMLTREECONSTRUCTOR_H_

#include <Queue.h>

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
			InForeignContent,
			AfterBody,
			InFrameset,
			AfterFrameset,
			AfterAfterBody,
			AfterAfterFrameseet
		};
		Document document;
	
	public:
		TreeConstructor() {}
		Document &construct(Queue<Token *> &tokens);
		void parseError();
	};
}

#endif