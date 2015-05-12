#ifndef _HTMLTOKENIZER_H_
#define _HTMLTOKENIZER_H_

#include <Queue.h>

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
		Queue<Token *> *tokens;
		char buffer[256];
		int bufferIndex;
		
		void emitCharacterToken();
		void emitEOFToken();
		void emitToken(Token *token);
		void parseError();
		void finalizeBuffer();
	
	public:
		Tokenizer();
		~Tokenizer();
		Queue<Token *> *tokenize(const unsigned char *inputStream);
	};
}

#endif
