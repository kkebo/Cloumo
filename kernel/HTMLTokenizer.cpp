#include "../headers.h"
#include <SmartPointer.h>

using namespace HTML;

Tokenizer::Tokenizer() {
	tokens = new Queue<Token *>(128);
}

Tokenizer::~Tokenizer() {
	delete tokens;
}

Queue<Token *> *Tokenizer::tokenize(const char *inputStream) {
	State state = State::Data; // Data state
	unique_ptr<Token> token;
	
	bufferIndex = 0;

	for (int i = 0;;) {
		char nextInputCharacter = inputStream[i];
		
		switch (state) {
			case State::Data: // Data state
				switch (nextInputCharacter) {
					case '&':
						emitCharacterToken();
						// Switch to the character reference in data state.
						state = State::CharacterReferenceInData;
						break;

					case '<':
						emitCharacterToken();
						// Switch to the tag open state.
						state = State::TagOpen;
						break;

					case 0: // EOF
						emitCharacterToken();
						// Emit the end-of-file token.
						emitEOFToken();
						break;

					default:
						// Emit the current input character as a character token.
						buffer[bufferIndex++] = nextInputCharacter;
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
							token.reset(new Token(Token::Type::StartTag));
							state = State::TagName;
							continue;
						}

						// Emit a U+003C LESS-THAN SIGN character token and reconsume the current input character in the data state.
						parseError();
						buffer[bufferIndex++] = '<';
						state = State::Data;
						continue;
				}
				break;

			case State::EndTagOpen: // End tag open state
				if (nextInputCharacter == 0) {
					// Parse error. Emit a U+003C LESS-THAN SIGN character token and a U+002F SOLIDUS character token. Reconsume the EOF character in the data state.
					parseError();
					buffer[bufferIndex++] = '<';
					buffer[bufferIndex++] = '/';
					state = State::Data;
					continue;
				} else if (nextInputCharacter == '>') {
					parseError();
					state = State::Data;
				} else {
					if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
						nextInputCharacter += 0x20;
					if ('a' <= nextInputCharacter && nextInputCharacter <= 'z') {
						// Create a new end tag token, set its tag name to the current input character, then switch to the tag name state. (Don't emit the token yet; further details will be filled in before it is emitted.)
						token.reset(new Token(Token::Type::EndTag));
						state = State::TagName;
						continue;
					} else {
						parseError();
						state = State::BogusComment;
					}
				}
				break;

			case State::TagName: // Tag name state
				switch (nextInputCharacter) {
					case 0x0009: // Tab
					case 0x000a: // LF
					case 0x000c: // FF
					case ' ':
						finalizeBuffer();
						token->setData(buffer);
						// Switch to the before attribute name state.
						state = State::BeforeAttributeName;
						break;

					case '/':
						finalizeBuffer();
						token->setData(buffer);
						// Switch to the self-closing start tag state.
						state = State::SelfClosingStartTag;
						break;

					case '>':
						// Emit the current tag token.
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						state = State::Data;
						break;

					case 0: // NULL
						// Parse error.
						parseError();
						// Append a U+FFFD REPLACEMENT CHARACTER character to the current tag token's tag name.
						buffer[bufferIndex++] = '\ufffd';
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						break;

					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						// Append the current input character to the current tag token's tag name.
						buffer[bufferIndex++] = nextInputCharacter;
						break;
				}
				break;

			case State::BeforeAttributeName: // Before attribute name state
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
					
					case '>':
						state = State::Data;
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					case '"':
					case '\'':
					case '<':
					case '=':
						parseError();
					default:
						/*if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;*/
						// Start a new attribute in the current tag token.
						// Set that attribute's name to the current input character, and its value to the empty string.
						// Switch to the attribute name state.
						state = State::AttributeName;
						continue;
				}
				break;
			
			case State::AttributeName:
				switch (nextInputCharacter) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						finalizeBuffer();
						token->addAttribute(buffer);
						state = State::AfterAttributeName;
						break;
					
					case '/':
						finalizeBuffer();
						token->addAttribute(buffer);
						state = State::SelfClosingStartTag;
						break;
					
					case '=':
						finalizeBuffer();
						token->addAttribute(buffer);
						state = State::BeforeAttributeValue;
						break;
					
					case '>':
						state = State::Data;
						finalizeBuffer();
						token->addAttribute(buffer);
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					case '"':
					case '\'':
					case '<':
						parseError();
					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						buffer[bufferIndex++] = nextInputCharacter;
						break;
				}
				break;
			
			case State::AfterAttributeName:
				switch (nextInputCharacter) {
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
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					case '"':
					case '\'':
					case '<':
						parseError();
					default:
						/*if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;*/
						// Start a new attribute in the current tag token.
						// Set that attribute's name to the current input character, and its value to the empty string.
						// Switch to the attribute name state.
						state = State::AttributeName;
						continue;
				}
				break;
			
			case State::BeforeAttributeValue:
				switch (nextInputCharacter) {
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
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
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
				switch (nextInputCharacter) {
					case '"':
						finalizeBuffer();
						token->setAttributeValue(buffer);
						state = State::AfterAttributeValueQuoted;
						break;
					
					case '&':
						state = State::CharacterReferenceInAttributeValue;
						// with the additional allowed character being U+0022 QUOTATION MARK (").
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					default:
						buffer[bufferIndex++] = nextInputCharacter;
						break;
				}
				break;
			
			case State::AttributeValueSingleQuoted:
				switch (nextInputCharacter) {
					case '\'':
						finalizeBuffer();
						token->setAttributeValue(buffer);
						state = State::AfterAttributeValueQuoted;
						break;
					
					case '&':
						state = State::CharacterReferenceInAttributeValue;
						// with the additional allowed character being U+0027 APOSTROPHE (').
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					default:
						buffer[bufferIndex++] = nextInputCharacter;
						break;
				}
				break;
			
			case State::AttributeValueUnquoted:
				switch (nextInputCharacter) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						finalizeBuffer();
						token->setAttributeValue(buffer);
						state = State::BeforeAttributeName;
						break;
					
					case '&':
						state = State::CharacterReferenceInAttributeValue;
						// with the additional allowed character being U+003E GREATER-THAN SIGN (>).
						break;
					
					case '>':
						state = State::Data;
						finalizeBuffer();
						token->setAttributeValue(buffer);
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					case '"':
					case '\'':
					case '<':
					case '=':
					case '`':
						parseError();
					default:
						buffer[bufferIndex++] = nextInputCharacter;
						break;
				}
				break;
			
			case State::CharacterReferenceInAttributeValue:
				break;
			
			case State::AfterAttributeValueQuoted:
				switch (nextInputCharacter) {
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
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						state = State::Data;
						continue;
					
					default:
						parseError();
						state = State::BeforeAttributeName;
						continue;
				}
				break;

			case State::SelfClosingStartTag: // Self-closing start tag state
				if (nextInputCharacter == 0) { // EOF
					parseError();
					state = State::Data;
					continue;
				} else if (nextInputCharacter == '>') {
					// Set the self-closing flag of the current tag token. Switch to the data state. Emit the current tag token.
					token->setSelfClosingFlag();
					state = State::Data;
					emitToken(token.release());
				} else {
					parseError();
					state = State::BeforeAttributeName;
					continue;
				}
				break;
			
			case State::MarkupDeclarationOpen:
				if (strncmp(inputStream + i, "--", 2) == 0) {
					// create a comment token whose data is the empty string, and switch to the comment start state.
					token.reset(new Token(Token::Type::Comment));
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
			
			case State::CommentStart:
				switch (nextInputCharacter) {
					case '-':
						state = State::CommentStartDash;
						break;
					
					case '>':
						parseError();
						state = State::Data;
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						emitToken(token.release());
						state = State::Data;
						continue;
					
					default:
						state = State::Comment;
						continue;
				}
				break;
			
			case State::CommentStartDash:
				switch (nextInputCharacter) {
					case '-':
						state = State::CommentEnd;
						break;
					
					case '>':
						parseError();
						state = State::Data;
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						emitToken(token.release());
						state = State::Data;
						continue;
					
					default:
						buffer[bufferIndex++] = '-';
						state = State::Comment;
						continue;
				}
				break;
			
			case State::Comment:
				if (nextInputCharacter == '-') {
					state = State::CommentEndDash;
				} else if (nextInputCharacter == 0) { // EOF
					parseError();
					finalizeBuffer();
					token->setData(buffer);
					emitToken(token.release());
					state = State::Data;
					continue;
				} else {
					buffer[bufferIndex++] = nextInputCharacter;
					continue;
				}
				break;
			
			case State::CommentEndDash:
				if (nextInputCharacter == '-') {
					state = State::CommentEnd;
				} else if (nextInputCharacter == 0) { // EOF
					parseError();
					finalizeBuffer();
					token->setData(buffer);
					emitToken(token.release());
					state = State::Data;
					continue;
				} else {
					buffer[bufferIndex++] = '-';
					state = State::Comment;
					continue;
				}
				break;
			
			case State::CommentEnd:
				switch (nextInputCharacter) {
					case '>':
						state = State::Data;
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						break;
					
					case '!':
						parseError();
						state = State::CommentEndBang;
						break;
					
					case '-':
						parseError();
						buffer[bufferIndex++] = '-';
						break;
					
					case 0: // EOF
						parseError();
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						state = State::Data;
						continue;
					
					default:
						parseError();
						state = State::Comment;
						continue;
				}
				break;
			
			case State::CommentEndBang:
				switch (nextInputCharacter) {
					case '-':
						buffer[bufferIndex++] = '-';
						buffer[bufferIndex++] = '!';
						state = State::CommentEndDash;
						break;
					
					case '>':
						state = State::Data;
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						state = State::Data;
						continue;
					
					default:
						buffer[bufferIndex++] = '-';
						buffer[bufferIndex++] = '!';
						state = State::Comment;
						continue;
				}
				break;
			
			case State::DOCTYPE:
				switch (nextInputCharacter) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						state = State::BeforeDOCTYPEName;
						break;
					
					case 0:
						// Parse error. Create a new DOCTYPE token. Set its force-quirks flag to on. Emit the token. Reconsume the EOF character in the data state.
						parseError();
						token.reset(new Token(Token::Type::DOCTYPE));
						// force-quirks flag to on
						state = State::Data;
						continue;
					
					default:
						parseError();
						state = State::BeforeDOCTYPEName;
						continue;
				}
				break;
			
			case State::BeforeDOCTYPEName:
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
						buffer[bufferIndex++] = '\ufffd';
						state = State::DOCTYPEName;
						continue;
					
					case '>':
						state = State::Data;
						break;
					
					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						// create a new DOCTYPE token
						token.reset(new Token(Token::Type::DOCTYPE));
						state = State::DOCTYPEName;
						continue;
				}
				break;
			
			case State::DOCTYPEName:
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
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						break;
					
					case 0: // EOF
						// Set the DOCTYPE token's force-quirks flag to on. Emit that DOCTYPE token. Reconsume the EOF character in the data state.
						finalizeBuffer();
						token->setData(buffer);
						// force-quirks flag to on.
						emitToken(token.release());
						state = State::Data;
						continue;
					
					default:
						if ('A' <= nextInputCharacter && nextInputCharacter <= 'Z')
							nextInputCharacter += 0x0020;
						buffer[bufferIndex++] = nextInputCharacter;
						break;
				}
				break;
			
			case State::AfterDOCTYPEName:
				switch (nextInputCharacter) {
					case '\t':
					case 0x0a:
					case 0x0c:
					case ' ':
						// ignore
						break;
					
					case '>':
						state = State::Data;
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						break;
					
					case 0: // EOF
						parseError();
						// force-quirks flag to on
						finalizeBuffer();
						token->setData(buffer);
						emitToken(token.release());
						state = State::Data;
						continue;
					
					default:
						if (strncmpi(inputStream + i, "public", 6) == 0) {
							// consume those characters and switch to the after DOCTYPE public keyword state.
							i += 6;
							state = State::AfterDOCTYPEPublicKeyword;
							continue;
						} else if (strncmpi(inputStream + i, "system", 6) == 0) {
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

			default:
				break;
		}
		
		if (nextInputCharacter == 0) { // EOF
			break;
		}
		
		i++;
	}
	
	return tokens;
}

void Tokenizer::emitCharacterToken() {
	if (bufferIndex > 0) {
		Token *token = new Token(Token::Type::Character);
		finalizeBuffer();
		token->setData(buffer);
		tokens->push(token);
	}
}

void Tokenizer::emitEOFToken() {
	tokens->push(new Token(Token::Type::EndOfFile));
}

void Tokenizer::emitToken(Token *token) {
	tokens->push(token);
}

void Tokenizer::parseError() {}

void Tokenizer::finalizeBuffer() {
	buffer[bufferIndex] = 0;
	bufferIndex = 0;
}
