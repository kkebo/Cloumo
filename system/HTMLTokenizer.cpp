#include "../headers.h"

void HTMLTokenizer::tokenize(const char *inputStream, unsigned int fileSize) {
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

	if (inputStream[0] == 0) {
		return;
	}

	for (int i = 0; i <= filesize;) {
		buffer[bufferIndex++] = inputStream[i];

		switch (state) {
			case DataState: // Data state
				if (i == fileSize) { // EOF
					// Emit the end-of-file token.
					emit("\0");
					break;
				}
				switch (inputStream[i + 1]) {
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
						// Emit the current input character as a character token.
						emit(buffer);
						break;

					default:
						// Emit the current input character as a character token.
						emit(buffer);
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
				switch (input) {
					case '!':

						break;

					case '/':
						// Switch to the end tag open state.
						state = EndTagOpenState;
						break;

					case '?':
						// Parse Error.
						break;

					default:
						// if (input == EOF)
						// Parse error. Switch to the data state. Reconsume the EOF character.

						// ASCII letter
						if ('A' <= input && input <= 'Z')
							input += 0x20;
						if ('a' <= input && input <= 'z') {
							state = 9;
							// Create a new start tag token.
							if (starttagtoken) delete starttagtoken;
							starttagtoken = new StartTagToken(input);
							state = 9;
							break;
						}

						// Parse Error.
						break;
				}
				break;

			case EndTagOpenState: // End tag open state
				break;

			case TagNameState: // Tag name state
				switch (input) {
					case 0x0009: // tab
					case 0x000a: // LF
					case 0x000c: // FF
					case ' ':
						// Switch to the before attribute name state.
						state = BeforeAttributeNameState;
						break;

					case '/':
						// Switch to the self-closing start tag state.
						break;

					case '>':
						// Emit the current tag token.
						state = DataState;
						break;

					case 0:
						// Parse error.
						// Append a U+FFFD REPLACEMENT CHARACTER character to the current tag token's tag name.
						break;

					default:
						if ('A' <= input && input <= 'Z')
							input += 0x0020;
						// Append the current input character to the current tag token's tag name.
						starttagtoken->tagname += input;
						break;
				}
				break;

			case BeforeAttributeNameState: // Before attribute name state
				switch (input) {
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

			case SeflClosingStartTagState: // Self-closing start tag state
				switch (input) {
					case '>':
						// Set the self-closing flag of the current tag token. Switch to the data state. Emit the current tag token.
						state = DataState;
						break;
				}
				break;

			default:
				break;
		}
		i++;
	}
}

void emit(const char *string) {

}
