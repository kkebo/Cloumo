#ifndef _HTMLDOCUMENTTYPE_H_
#define _HTMLDOCUMENTTYPE_H_

namespace HTML {
	class DocumentType {
	public:
		string name;
		string publicId = "";
		string systemId = "";
		
		DocumentType(const string &n) : name(n) {}
		DocumentType(const string &n, const string &p, const string &s) : name(n), publicId(p), systemId(s) {}
	};
}

#endif