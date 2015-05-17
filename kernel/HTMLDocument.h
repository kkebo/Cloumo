#ifndef _HTMLDOCUMENT_H_
#define _HTMLDOCUMENT_H_

namespace HTML {
	class Document {
	public:
		DocumentType doctype;
		Element documentElement;
		
		Document() : doctype("") {}
	};
}

#endif