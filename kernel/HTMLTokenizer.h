#ifndef _HTMLTOKENIZER_H_
#define _HTMLTOKENIZER_H_

#include <Queue.h>
#include <pistring.h>
#include <SmartPointer.h>

namespace HTML {
	class Tokenizer {
	private:
		enum class State {
			Data,
			CharacterReferenceInData,
			RCDATA,
			CharacterReferenceInRCDATA,
			RAWTEXT,
			ScriptData,
			PLAINTEXT,
			TagOpen,
			EndTagOpen,
			TagName,
			RCDATALessThanSign,
			RCDATAEndTagOpen,
			RCDATAEndTagName,
			RAWTEXTLessThanSign,
			RAWTEXTEndTagOpen,
			RAWTEXTEndTagName,
			ScriptDataLessThanSign,
			ScriptDataEndTagOpen,
			ScriptDataEndTagName,
			ScriptDataEscapeStart,
			ScriptDataEscapeStartDash,
			ScriptDataEscaped,
			ScriptDataEscapedDash,
			ScriptDataEscapedDashDash,
			ScriptDataEscapedLessThanSign,
			ScriptDataEscapedEndTagOpen,
			ScriptDataEscapedEndTagName,
			ScriptDataDoubleEscapeStart,
			ScriptDataDoubleEscaped,
			ScriptDataDoubleEscapedDash,
			ScriptDataDoubleEscapedDashDash,
			ScriptDataDoubleEscapedLessThanSign,
			ScriptDataDoubleEscapeEnd,
			BeforeAttributeName,
			AttributeName,
			AfterAttributeName,
			BeforeAttributeValue,
			AttributeValueDoubleQuoted,
			AttributeValueSingleQuoted,
			AttributeValueUnquoted,
			CharacterReferenceInAttributeValue,
			AfterAttributeValueQuoted,
			SelfClosingStartTag,
			BogusComment,
			MarkupDeclarationOpen,
			CommentStart,
			CommentStartDash,
			Comment,
			CommentEndDash,
			CommentEnd,
			CommentEndBang,
			DOCTYPE,
			BeforeDOCTYPEName,
			DOCTYPEName,
			AfterDOCTYPEName,
			AfterDOCTYPEPublicKeyword,
			BeforeDOCTYPEPublicIdentifier,
			DOCTYPEPublicIdentifierDoubleQuoted,
			DOCTYPEPublicIdentifierSingleQuoted,
			AfterDOCTYPEPublicIdentifier,
			BetweenDOCTYPEPublicAndSystemIdentifiers,
			AfterDOCTYPESystemKeyword,
			BeforeDOCTYPESystemIdentifier,
			DOCTYPESystemIdentifierDoubleQuoted,
			DOCTYPESystemIdentifierSingleQuoted,
			AfterDOCTYPESystemIdentifier,
			BogusDOCTYPE,
			CDATASection
		};
		Queue<shared_ptr<Token>> tokens;
		string buffer;
		
		void emitCharacterToken(char c);
		void emitEOFToken();
		void emitToken(const unique_ptr<Token> &token);
		void parseError();
	
	public:
		Tokenizer();
		Queue<shared_ptr<Token>> &tokenize(const char *inputStream);
	};
}

#endif
