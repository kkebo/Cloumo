#include "HTMLTokenizer.h"

using namespace HTML;

enum class Tokenizer::State {
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

Tokenizer::Tokenizer() : tokens(128) {}

Queue<shared_ptr<Token>> &Tokenizer::tokenize(const string &inputStream) {
	State state = State::Data; // Data state
	unique_ptr<Token> token;

	auto it = inputStream.begin();
	int i = 0;
	bool endFlag = false;
	while (!endFlag) {
		switch (state) {
			case State::Data: // Data state
				if (it == inputStream.end()) {
					// EOF
					// Emit the end-of-file token.
					emitEOFToken();
					endFlag = true;
					continue;
				}
				switch (*it) {
					case '&':
						// Switch to the character reference in data state.
						state = State::CharacterReferenceInData;
						break;

					case '<':
						// Switch to the tag open state.
						state = State::TagOpen;
						break;

					default:
						// Emit the current input character as a character token.
						emitCharacterToken(*it);
						break;
				}
				break;

			case State::CharacterReferenceInData: // Character reference in data state
				break;

			case State::RCDATA: // RCDATA state
				break;

			case State::CharacterReferenceInRCDATA: // Character reference in RCDATA state
				break;

			case State::RAWTEXT: // RAWTEXT state
				break;

			case State::ScriptData: // Script data state
				break;

			case State::PLAINTEXT: // PLAINTEXT state
				break;

			case State::TagOpen: // Tag open state
				switch (*it) {
					case '!':
						// Switch to the markup declaration open state.
						state = State::MarkupDeclarationOpen;
						break;

					case '/':
						// Switch to the end tag open state.
						state = State::EndTagOpen;
						break;

					case '?':
						// Parse Error.
						parseError();
						// Switch to the bogus comment state.
						state = State::BogusComment;
						break;

					default:
						// ASCII letter
						if (('A' <= *it && *it <= 'Z') || ('a' <= *it && *it <= 'z')) {
							// Create a new start tag token.
							token.reset(new Token(Token::Type::StartTag));
							state = State::TagName;
							continue;
						}

						// Emit a U+003C LESS-THAN SIGN character token and reconsume the current input character in the data state.
						parseError();
						emitCharacterToken('<');
						state = State::Data;
						continue;
				}
				break;

			case State::EndTagOpen: // End tag open state
				if (it == inputStream.end()) {
					// Parse error. Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS character token. Reconsume the EOF character in the data state.
					parseError();
					emitCharacterToken('<');
					emitCharacterToken('/');
					state = State::Data;
					continue;
				} else if (*it == '>') {
					parseError();
					state = State::Data;
				} else {
					if (('A' <= *it && *it <= 'Z') || ('a' <= *it && *it <= 'z')) {
						// Create a new end tag token, set its tag name to the current input character, then switch to the tag name state. (Don't emit the token yet; further details will be filled in before it is emitted.)
						token.reset(new Token(Token::Type::EndTag));
						token->data = ('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it;
						state = State::TagName;
						break;
					} else {
						parseError();
						state = State::BogusComment;
					}
				}
				break;

			case State::TagName: // Tag name state
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x0009: // Tab
					case 0x000a: // LF
					case 0x000c: // FF
					case ' ':
						// Switch to the before attribute name state.
						state = State::BeforeAttributeName;
						break;

					case '/':
						// Switch to the self-closing start tag state.
						state = State::SelfClosingStartTag;
						break;

					case '>':
						// Emit the current tag token.
						emitToken(token);
						state = State::Data;
						break;

					case 0: // NULL
						// Parse error.
						parseError();
						// Append a U+FFFD REPLACEMENT CHARACTER character to the current tag token's tag name.
						token->data += "\ufffd";
						emitToken(token);
						break;

					default:
						// Append the current input character to the current tag token's tag name.
						token->data += ('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it;
						break;
				}
				break;

			case State::BeforeAttributeName: // Before attribute name state
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// Ignore
						break;

					case '/':
						// Switch to the self-closing start tag state.
						state = State::SelfClosingStartTag;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					case '"':
					case '\'':
					case '<':
					case '=':
						parseError();
					default:
						// Start a new attribute in the current tag token.
						// Set that attribute's name to the current input character, and its value to the empty string.
						token->appendAttribute(('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it);
						// Switch to the attribute name state.
						state = State::AttributeName;
						break;
				}
				break;
			
			case State::AttributeName:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::AfterAttributeName;
						break;
					
					case '/':
						state = State::SelfClosingStartTag;
						break;
					
					case '=':
						state = State::BeforeAttributeValue;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					case '"':
					case '\'':
					case '<':
						parseError();
					default:
						token->appendAttributeName(('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it);
						break;
				}
				break;
			
			case State::AfterAttributeName:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '/':
						state = State::SelfClosingStartTag;
						break;
					
					case '=':
						state = State::BeforeAttributeValue;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					case '"':
					case '\'':
					case '<':
						parseError();
					default:
						// Start a new attribute in the current tag token.
						// Set that attribute's name to the current input character, and its value to the empty string.
						token->appendAttribute(('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it);
						// Switch to the attribute name state.
						state = State::AttributeName;
						break;
				}
				break;
			
			case State::BeforeAttributeValue:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '"':
						state = State::AttributeValueDoubleQuoted;
						break;
					
					case '&':
						state = State::AttributeValueUnquoted;
						continue;
					
					case '\'':
						state = State::AttributeValueSingleQuoted;
						break;
					
					case '>':
						parseError();
						state = State::Data;
						emitToken(token);
						break;
					
					case '<':
					case '=':
					case '`':
						parseError();
					default:
						state = State::AttributeValueUnquoted;
						continue;
				}
				break;
			
			case State::AttributeValueDoubleQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '"':
						state = State::AfterAttributeValueQuoted;
						break;
					
					case '&':
						state = State::CharacterReferenceInAttributeValue;
						// with the additional allowed character being U+0022 QUOTATION MARK (").
						break;
					
					default:
						token->appendAttributeValue(*it);
						break;
				}
				break;
			
			case State::AttributeValueSingleQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '\'':
						state = State::AfterAttributeValueQuoted;
						break;
					
					case '&':
						state = State::CharacterReferenceInAttributeValue;
						// with the additional allowed character being U+0027 APOSTROPHE (').
						break;
					
					default:
						token->appendAttributeValue(*it);
						break;
				}
				break;
			
			case State::AttributeValueUnquoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BeforeAttributeName;
						break;
					
					case '&':
						state = State::CharacterReferenceInAttributeValue;
						// with the additional allowed character being U+003E GREATER-THAN SIGN (>).
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					case '"':
					case '\'':
					case '<':
					case '=':
					case '`':
						parseError();
					default:
						token->appendAttributeValue(*it);
						break;
				}
				break;
			
			case State::CharacterReferenceInAttributeValue:
				break;
			
			case State::AfterAttributeValueQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BeforeAttributeName;
						break;
					
					case '/':
						state = State::SelfClosingStartTag;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						state = State::BeforeAttributeName;
						continue;
				}
				break;

			case State::SelfClosingStartTag: // Self-closing start tag state
				if (it == inputStream.end()) { // EOF
					parseError();
					state = State::Data;
					continue;
				} else if (*it == '>') {
					// Set the self-closing flag of the current tag token. Switch to the data state. Emit the current tag token.
					token->setSelfClosingFlag();
					state = State::Data;
					emitToken(token);
				} else {
					parseError();
					state = State::BeforeAttributeName;
					continue;
				}
				break;
			
			case State::MarkupDeclarationOpen:
				if (inputStream.compare(i, 2, "--") == 0) {
					// create a comment token whose data is the empty string, and switch to the comment start state.
					token.reset(new Token(Token::Type::Comment));
					i += 2;
					state = State::CommentStart;
					continue;
				} else if (inputStream.comparei(i, 7, "DOCTYPE") == 0) {
					i += 7;
					state = State::DOCTYPE;
					continue;
				}
				// Otherwise, if the insertion mode is "in foreign content" and the current node is not an element in the HTML namespace and the next seven characters are an case-sensitive match for the string "[CDATA[" (the five uppercase letters "CDATA" with a U+005B LEFT SQUARE BRACKET character before and after), then consume those characters and switch to the CDATA section state.
				// Otherwise, this is a parse error. Switch to the bogus comment state. The next character that is consumed, if any, is the first character that will be in the comment.
				else {
					parseError();
					state = State::BogusComment;
					continue;
				}
				break;
			
			case State::CommentStart:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '-':
						state = State::CommentStartDash;
						break;
					
					case '>':
						parseError();
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						state = State::Comment;
						continue;
				}
				break;
			
			case State::CommentStartDash:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '-':
						state = State::CommentEnd;
						break;
					
					case '>':
						parseError();
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						token->data += '-';
						state = State::Comment;
						continue;
				}
				break;
			
			case State::Comment:
				if (it == inputStream.end()) { // EOF
					parseError();
					emitToken(token);
					state = State::Data;
					continue;
				} else if (*it == '-') {
					state = State::CommentEndDash;
				} else {
					token->data += *it;
				}
				break;
			
			case State::CommentEndDash:
				if (it == inputStream.end()) { // EOF
					parseError();
					emitToken(token);
					state = State::Data;
					continue;
				} else if (*it == '-') {
					state = State::CommentEnd;
				} else {
					token->data += '-';
					state = State::Comment;
					continue;
				}
				break;
			
			case State::CommentEnd:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					case '!':
						parseError();
						state = State::CommentEndBang;
						break;
					
					case '-':
						parseError();
						token->data += '-';
						break;
					
					default:
						parseError();
						state = State::Comment;
						continue;
				}
				break;
			
			case State::CommentEndBang:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '-':
						token->data += "-!";
						state = State::CommentEndDash;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						token->data += "-!";
						state = State::Comment;
						continue;
				}
				break;
			
			case State::DOCTYPE:
				if (it == inputStream.end()) {
					// EOF
					// Parse error. Create a new DOCTYPE token. Set its force-quirks flag to on. Emit the token. Reconsume the EOF character in the data state.
					parseError();
					token.reset(new Token(Token::Type::DOCTYPE));
					// force-quirks flag to on
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BeforeDOCTYPEName;
						break;
					
					default:
						parseError();
						state = State::BeforeDOCTYPEName;
						continue;
				}
				break;
			
			case State::BeforeDOCTYPEName:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					state = State::Data;
					token.reset(new Token(Token::Type::DOCTYPE));
					// set forse-quirks flag to on
					emitToken(token);
					continue;
				}
				switch (*it) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case 0: // NULL
						// Parse error. Set the token's name to a U+FFFD REPLACEMENT CHARACTER character. Switch to the DOCTYPE name state.
						parseError();
						token->data += "\ufffd";
						state = State::DOCTYPEName;
						continue;
					
					case '>':
						state = State::Data;
						break;
					
					default:
						// create a new DOCTYPE token
						token.reset(new Token(Token::Type::DOCTYPE));
						token->data += ('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it;
						state = State::DOCTYPEName;
						break;
				}
				break;
			
			case State::DOCTYPEName:
				if (it == inputStream.end()) {
					// EOF
					// Set the DOCTYPE token's force-quirks flag to on. Emit that DOCTYPE token. Reconsume the EOF character in the data state.
					// set force-quirks flag to on.
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::AfterDOCTYPEName;
						break;
					
					case '>':
						state = State::Data;
						// emit the current DOCTYPE token
						emitToken(token);
						break;
					
					default:
						token->data += ('A' <= *it && *it <= 'Z') ? *it + 0x20 : *it;
						break;
				}
				break;
			
			case State::AfterDOCTYPEName:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// set force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						if (inputStream.comparei(i, 6, "public") == 0) {
							// consume those characters and switch to the after DOCTYPE public keyword state.
							i += 6;
							state = State::AfterDOCTYPEPublicKeyword;
							continue;
						} else if (inputStream.comparei(i, 6, "system") == 0) {
							// consume those characters and switch to the after DOCTYPE system keyword state.
							i += 6;
							state = State::AfterDOCTYPESystemKeyword;
							continue;
						} else {
							parseError();
							// force-quirks flag to on
							state = State::BogusDOCTYPE;
						}
						break;
				}
				break;
			
			case State::AfterDOCTYPEPublicKeyword:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BeforeDOCTYPEPublicIdentifier;
						break;
					
					case '"':
						parseError();
						// Set the DOCTYPE token's public identifier to the empty string (not missing),
						// then switch to the DOCTYPE public identifier (double-quoted) state.
						state = State::DOCTYPEPublicIdentifierDoubleQuoted;
						break;
					
					case '\'':
						parseError();
						// Set the DOCTYPE token's public identifier to the empty string (not missing),
						// then switch to the DOCTYPE public identifier (single-quoted) state.
						state = State::DOCTYPEPublicIdentifierSingleQuoted;
						break;
					
					case '>':
						parseError();
						// force-quirks flag to on
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						// force-quirks flag to on
						state = State::BogusDOCTYPE;
						break;
				}
				break;
			
			case State::BeforeDOCTYPEPublicIdentifier:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '"':
						parseError();
						// Set the DOCTYPE token's public identifier to the empty string (not missing),
						// then switch to the DOCTYPE public identifier (double-quoted) state.
						state = State::DOCTYPEPublicIdentifierDoubleQuoted;
						break;
					
					case '\'':
						parseError();
						// Set the DOCTYPE token's public identifier to the empty string (not missing),
						// then switch to the DOCTYPE public identifier (single-quoted) state.
						state = State::DOCTYPEPublicIdentifierSingleQuoted;
						break;
					
					case '>':
						parseError();
						// force-quirks flag to on
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						// force-quirks flag to on
						state = State::BogusDOCTYPE;
						break;
				}
				break;
			
			case State::DOCTYPEPublicIdentifierDoubleQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '"':
						state = State::AfterDOCTYPEPublicIdentifier;
						break;
					
					case '>':
						parseError();
						// force-quirks flag to on
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						// Append the current input character to the current DOCTYPE token's public identifier.
						break;
				}
				break;
			
			case State::DOCTYPEPublicIdentifierSingleQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '\'':
						state = State::AfterDOCTYPEPublicIdentifier;
						break;
					
					case '>':
						parseError();
						// force-quirks flag to on
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						// Append the current input character to the current DOCTYPE token's public identifier.
						break;
				}
				break;
			
			case State::AfterDOCTYPEPublicIdentifier:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BetweenDOCTYPEPublicAndSystemIdentifiers;
						break;
					
					case '"':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (double-quoted) state.
						state = State::DOCTYPESystemIdentifierDoubleQuoted;
						break;
					
					case '\'':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (single-quoted) state.
						state = State::DOCTYPESystemIdentifierSingleQuoted;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						// force-quirks flag to on
						state = State::BogusDOCTYPE;
						break;
				}
				break;
			
			case State::BetweenDOCTYPEPublicAndSystemIdentifiers:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '"':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (double-quoted) state.
						state = State::DOCTYPESystemIdentifierDoubleQuoted;
						break;
					
					case '\'':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (single-quoted) state.
						state = State::DOCTYPESystemIdentifierSingleQuoted;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						// force-quirks flag to on
						state = State::BogusDOCTYPE;
						break;
				}
				break;
			
			case State::AfterDOCTYPESystemKeyword:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BeforeDOCTYPESystemIdentifier;
						break;
					
					case '"':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (double-quoted) state.
						state = State::DOCTYPESystemIdentifierDoubleQuoted;
						break;
					
					case '\'':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (single-quoted) state.
						state = State::DOCTYPESystemIdentifierSingleQuoted;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						// force-quirks flag to on
						state = State::BogusDOCTYPE;
						break;
				}
				break;
			
			case State::BeforeDOCTYPESystemIdentifier:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '"':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (double-quoted) state.
						state = State::DOCTYPESystemIdentifierDoubleQuoted;
						break;
					
					case '\'':
						parseError();
						// Set the DOCTYPE token's system identifier to the empty string (not missing),
						// then switch to the DOCTYPE system identifier (single-quoted) state.
						state = State::DOCTYPESystemIdentifierSingleQuoted;
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						// force-quirks flag to on
						state = State::BogusDOCTYPE;
						break;
				}
				break;
			
			case State::DOCTYPESystemIdentifierDoubleQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '"':
						state = State::AfterDOCTYPESystemIdentifier;
						break;
					
					case '>':
						parseError();
						// force-quirks flag to on
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						// Append the current input character to the current DOCTYPE token's system identifier.
						break;
				}
				break;
			
			case State::DOCTYPESystemIdentifierSingleQuoted:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case '\'':
						state = State::AfterDOCTYPESystemIdentifier;
						break;
					
					case '>':
						parseError();
						// force-quirks flag to on
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						// Append the current input character to the current DOCTYPE token's system identifier.
						break;
				}
				break;
			
			case State::AfterDOCTYPESystemIdentifier:
				if (it == inputStream.end()) {
					// EOF
					parseError();
					// force-quirks flag to on
					emitToken(token);
					state = State::Data;
					continue;
				}
				switch (*it) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '>':
						state = State::Data;
						emitToken(token);
						break;
					
					default:
						parseError();
						state = State::BogusDOCTYPE;
						// This does not set the DOCTYPE token's force-quirks flag to on.
						break;
				}
				break;
			
			case State::BogusDOCTYPE:
				if (it == inputStream.end()) { // EOF
					emitToken(token);
					state = State::Data;
					continue;
				} else if (*it == '>') {
					state = State::Data;
					emitToken(token);
				} else {
					// ignore
				}
				break;

			default:
				break;
		}
		
		// TODO: 本来はこれがなくても無限ループは起こらないはずなので，解決したら外す
		if (it == inputStream.end()) {
			// EOF
			endFlag = true;
			continue;
		}
		
		++it;
		++i;
	}
	
	return tokens;
}

void Tokenizer::emitCharacterToken(char c) {
	shared_ptr<Token> token(new Token(Token::Type::Character));
	token->data += c;
	tokens.push(token);
}

void Tokenizer::emitEOFToken() {
	tokens.push(shared_ptr<Token>(new Token(Token::Type::EndOfFile)));
}

void Tokenizer::emitToken(unique_ptr<Token> &token) {
	// 現在のままではよくない．
	// std::move 相当のものを実装したら修正するべき
	tokens.push(shared_ptr<Token>(token.release()));
}

void Tokenizer::parseError() {}
