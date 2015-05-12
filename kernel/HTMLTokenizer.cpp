#include "../headers.h"

using namespace HTML;

void Tokenizer::tokenize(const char *inputStream) {
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
	}
	State state = State::Data; // Data state
	StartTagToken *startTagToken = nullptr;
	char buffer[256];
	int bufferIndex = 0;

	for (;;) {
		char nextInputCharacter = inputStream[i];
		
		switch (state) {
			case Data: // Data state
				switch (nextInputCharacter) {
					case '&':
						// Switch to the character reference in data state.
						state = State::CharacterReferenceInData;
						continue;

					case '<':
						// Switch to the tag open state.
						state = State::TagOpen;
						continue;

					case 0: // EOF
						// Emit the end-of-file token.
						emitEOFToken();
						break;

					default:
						// Emit the current input character as a character token.
						emitCharacterToken(buffer);
						break;
				}
				break;

			case CharacterReferenceInData: // Character reference in data state
				break;

			case RCDATA: // RCDATA state
				break;

			case CharacterReferenceInRCDATA: // Character reference in RCDATA state
				break;

			case RAWTEXT: // RAWTEXT state
				break;

			case ScriptData: // Script data state
				break;

			case PLAINTEXT: // PLAINTEXT state
				break;

			case TagOpen: // Tag open state
				switch (nextInputCharacter) {
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
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x20;
						if ('a' <= nextInputCharacter && nextInputCharacter <= 'z') {
							// Create a new start tag token.
							//delete tagToken;
							tagToken = new TagToken(nextInputCharacter, true);
							state = State::TagName;
							break;
						}

						// Emit a U+003C LESS-THAN SIGN character token and reconsume the current input character in the data state.
						parseError();
						emitCharacterToken("<");
						state = State::Data;
						continue;
				}
				break;

			case EndTagOpen: // End tag open state
				if (nextInputCharacter == 0) {
					// Parse error. Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS character token. Reconsume the EOF character in the data state.
					parseError();
					emitCharacterToken("<");
					emitCharacterToken("/");
					state = State::Data;
				} else if (nextInputCharacter == '>') {
					parseError();
					state = State::Data;
				} else {
					if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
						nextInputCharacter += 0x20;
					if ('a' <= nextInputCharacter && nextInputCharacter <= 'z') {
						// Create a new end tag token, set its tag name to the current input character, then switch to the tag name state. (Don't emit the token yet; further details will be filled in before it is emitted.)
						//delete tagToken;
						tagToken = new TagToken(nextInputCharacter, false);
						state = State::TagName;
					} else {
						parseError();
						state = State::BogusComment;
					}
				}
				break;

			case TagName: // Tag name state
				switch (nextInputCharacter) {
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
						emitTagToken(tagToken);
						state = State::Data;
						break;

					case 0: // NULL
						// Parse error.
						parseError();
						// Append a U+FFFD REPLACEMENT CHARACTER character to the current tag token's tag name.
						tagToken->appendTagName('\ufffd');
						break;

					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						// Append the current input character to the current tag token's tag name.
						tagToken->appendTagName(nextInputCharacter);
						break;
				}
				break;

			case BeforeAttributeName: // Before attribute name state
				switch (nextInputCharacter) {
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
				}
				break;

			case SelfClosingStartTag: // Self-closing start tag state
				if (nextInputCharacter == 0) {
					parseError();
					state = State::Data;
				} else if (nextInputCharacter == '>') {
					// Set the self-closing flag of the current tag token. Switch to the data state. Emit the current tag token.
					tagToken->selfClosingFlag = true;
					emitTagToken(tagToken);
					state = State::Data;
				} else {
					state = State::BeforeAttributeName;
					continue;
				}
				break;
			
			case MarkupDeclarationOpenState:
				if (strncmp(inputStream + i, "--", 2) == 0) {
					// create a comment token whose data is the empty string, and switch to the comment start state.
					
					i += 2;
					state = State::CommentStart;
					continue;
				} else if (strncmpi(inputStream + i, "DOCTYPE", 7) == 0) {
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
			
			case DOCTYPEState:
				if (i == size) { // EOF
					// Parse error. Create a new DOCTYPE token. Set its force-quirks flag to on. Emit the token. Reconsume the EOF character in the data state.
				}
				switch (nextInputCharacter) {
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
			
			case BeforeDOCTYPENameState:
				switch (nextInputCharacter) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case 0:
						// Parse error. Set the token's name to a U+FFFD REPLACEMENT CHARACTER character. Switch to the DOCTYPE name state.
						parseError();
						
						break;
					
					case '>':
						state = State::Data;
						break;
					
					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						// create a new DOCTYPE token
						state = State::DOCTYPEName;
						break;
				break;
			
			case DOCTYPENameState:
				switch (nextInputCharacter) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::AfterDOCTYPEName;
						break;
					
					case '>':
						state = State::Data;
						// emit the current DOCTYPE token
						break;
					
					case 0:
						break;
					
					default:
						break;
				}
				break;
			
			case AfterDOCTYPENameState:
				switch (nextInputCharacter) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					
				}
				break;

			default:
				break;
		}
		
		if (nextInputCharacter == 0) { // EOF
			break;
		}
		
		i++;
	}
}

void Tokenizer::emitCharacterToken(const char *string) {

}

void Tokenizer::emitEOFToken() {
	
}
