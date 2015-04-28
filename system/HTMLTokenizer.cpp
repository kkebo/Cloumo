#include "../headers.h"

using namespace HTML;

void Tokenizer::tokenize(const char *inputStream, unsigned int size) {
	enum State {
		DataState,
		CharacterReferenceInDataState,
		RCDATAState,
		CharacterReferenceInRCDATAState,
		RAWTEXTState,
		ScriptDataState,
		PLAINTEXTState,
		TagOpenState,
		EndTagOpenState,
		TagNameState,
		RCDATALessThanSignState,
		RCDATAEndTagOpenState,
		RCDATAEndTagNameState,
		RAWTEXTLessThanSignState,
		RAWTEXTEndTagOpenState,
		RAWTEXTEndTagNameState,
		ScriptDataLessThanSignState,
		ScriptDataEndTagOpenState,
		ScriptDataEndTagNameState,
		ScriptDataEscapeStartState,
		ScriptDataEscapeStartDashState,
		ScriptDataEscapedState,
		ScriptDataEscapedDashState,
		ScriptDataEscapedDashDashState,
		ScriptDataEscapedLessThanSignState,
		ScriptDataEscapedEndTagOpenState,
		ScriptDataEscapedEndTagNameState,
		ScriptDataDoubleEscapeStartState,
		ScriptDataDoubleEscapedState,
		ScriptDataDoubleEscapedDashState,
		ScriptDataDoubleEscapedDashDashState,
		ScriptDataDoubleEscapedLessThanSignState,
		ScriptDataDoubleEscapeEndState,
		BeforeAttributeNameState,
		AttributeNameState,
		AfterAttributeNameState,
		BeforeAttributeValueState,
		AttributeValueDoubleQuotedState,
		AttributeValueSingleQuotedState,
		AttributeValueUnquotedState,
		CharacterReferenceInAttributeValueState,
		AfterAttributeValueQuotedState,
		SelfClosingStartTagState,
		BogusCommentState,
		MarkupDeclarationOpenState,
		CommentStartState,
		CommentStartDashState,
		CommentState,
		CommentEndDashState,
		CommentEndState,
		CommentEndBangState,
		DOCTYPEState,
		BeforeDOCTYPENameState,
		DOCTYPENameState,
		AfterDOCTYPENameState,
		AfterDOCTYPEPublicKeywordState,
		BeforeDOCTYPEPublicIdentifierState,
		DOCTYPEPublicIdentifierDoubleQuotedState,
		DOCTYPEPublicIdentifierSingleQuotedState,
		AfterDOCTYPEPublicIdentifierState,
		BetweenDOCTYPEPublicAndSystemIdentifiersState,
		AfterDOCTYPESystemKeywordState,
		BeforeDOCTYPESystemIdentifierState,
		DOCTYPESystemIdentifierDoubleQuotedState,
		DOCTYPESystemIdentifierSingleQuotedState,
		AfterDOCTYPESystemIdentifierState,
		BogusDOCTYPEState,
		CDATASectionState
	}
	State state = DataState; // Data state
	StartTagToken *startTagToken = nullptr;
	char buffer[256];
	int bufferIndex = 0;

	for (int i = 0; i <= size;) {
		// 範囲外にアクセスできてしまうのでは？
		char nextInputCharacter = inputStream[i];
		
		switch (state) {
			case DataState: // Data state
				if (i == size) { // EOF
					// Emit the end-of-file token.
					emitEOFToken();
					break;
				}
				switch (nextInputCharacter) {
					case '&':
						// Switch to the character reference in data state.
						state = CharacterReferenceInDataState;
						continue;

					case '<':
						// Switch to the tag open state.
						state = TagOpenState;
						continue;

					case 0: // NULL
						// Parse Error.
						parseError();
						// Emit the current input character as a character token.
						emitCharacterToken(buffer);
						break;

					default:
						// Emit the current input character as a character token.
						emitCharacterToken(buffer);
						break;
				}
				break;

			case CharacterReferenceInDataState: // Character reference in data state
				break;

			case RCDATAState: // RCDATA state
				break;

			case CharacterReferenceInRCDATAState: // Character reference in RCDATA state
				break;

			case RAWTEXTState: // RAWTEXT state
				break;

			case ScriptDataState: // Script data state
				break;

			case PLAINTEXTState: // PLAINTEXT state
				break;

			case TagOpenState: // Tag open state
				switch (nextInputCharacter) {
					case '!':
						// Switch to the markup declaration open state.
						state = MarkupDeclarationOpenState;
						break;

					case '/':
						// Switch to the end tag open state.
						state = EndTagOpenState;
						break;

					case '?':
						// Parse Error.
						parseError();
						// Switch to the bogus comment state.
						state = BogusCommentState;
						break;

					default:
						// ASCII letter
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x20;
						if ('a' <= nextInputCharacter && nextInputCharacter <= 'z') {
							// Create a new start tag token.
							//if (tagToken) delete tagToken;
							tagToken = new TagToken(nextInputCharacter, true);
							state = TagNameState;
							break;
						}

						// Emit a U+003C LESS-THAN SIGN character token and reconsume the current input character in the data state.
						parseError();
						emitCharacterToken("<");
						state = DataState;
						continue;
				}
				break;

			case EndTagOpenState: // End tag open state
				if (i == size) {
					// Parse error. Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS character token. Reconsume the EOF character in the data state.
					parseError();
					emitCharacterToken("<");
					emitCharacterToken("/");
					state = DataState;
					continue;
				}
				if (nextInputCharacter == '>') {
					parseError();
					state = DataState;
				} else {
					if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
						nextInputCharacter += 0x20;
					if ('a' <= nextInputCharacter && nextInputCharacter <= 'z') {
						// Create a new end tag token, set its tag name to the current input character, then switch to the tag name state. (Don't emit the token yet; further details will be filled in before it is emitted.)
						//if (tagToken) delete tagToken;
						tagToken = new TagToken(nextInputCharacter, false);
						state = TagNameState;
					} else {
						parseError();
						state = BogusCommentState;
					}
				}
				break;

			case TagNameState: // Tag name state
				switch (nextInputCharacter) {
					case 0x0009: // Tab
					case 0x000a: // LF
					case 0x000c: // FF
					case ' ':
						// Switch to the before attribute name state.
						state = BeforeAttributeNameState;
						break;

					case '/':
						// Switch to the self-closing start tag state.
						state = SelfClosingStartTagState;
						break;

					case '>':
						// Emit the current tag token.
						emitTagToken(tagToken);
						state = DataState;
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

			case BeforeAttributeNameState: // Before attribute name state
				switch (nextInputCharacter) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// Ignore
						break;

					case '/':
						// Switch to the self-closing start tag state.
						state = SelfClosingStartTagState;
						break;
				}
				break;

			case SelfClosingStartTagState: // Self-closing start tag state
				if (i == size) {
					parseError();
					state = DataState;
					continue;
				}
				if (nextInputCharacter == '>') {
					// Set the self-closing flag of the current tag token. Switch to the data state. Emit the current tag token.
					tagToken->selfClosingFlag = true;
					emitTagToken(tagToken);
					state = DataState;
				} else {
					state = BeforeAttributeNameState;
					continue;
				}
				break;
			
			case MarkupDeclarationOpenState:
				if (strncmp(inputStream + i, "--", 2) == 0) {
					// create a comment token whose data is the empty string, and switch to the comment start state.
					
					i += 2;
					state = CommentStartState;
					continue;
				} else if (strncmpi(inputStream + i, "DOCTYPE", 7) == 0) {
					i += 7;
					state = DOCTYPEState;
					continue;
				}
				// Otherwise, if the insertion mode is "in foreign content" and the current node is not an element in the HTML namespace and the next seven characters are an case-sensitive match for the string "[CDATA[" (the five uppercase letters "CDATA" with a U+005B LEFT SQUARE BRACKET character before and after), then consume those characters and switch to the CDATA section state.
				// Otherwise, this is a parse error. Switch to the bogus comment state. The next character that is consumed, if any, is the first character that will be in the comment.
				else {
					parseError();
					state = BogusCommentState;
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
						state = BeforeDOCTYPENameState;
						break;
					
					default:
						parseError();
						state = BeforeDOCTYPENameState;
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
						state = DataState;
						break;
					
					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						// create a new DOCTYPE token
						state = DOCTYPENameState;
						break;
				break;
			
			case DOCTYPENameState:
				switch (nextInputCharacter) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						state = afterDOCTYPENameState;
						break;
					
					case '>':
						state = DataState;
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
		i++;
	}
}

void Tokenizer::emitCharacterToken(const char *string) {

}

void Tokenizer::emitEOFToken() {
	
}
