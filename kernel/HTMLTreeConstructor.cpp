#include "../headers.h"
#include <SmartPointer.h>

using namespace HTML;

Document &TreeConstructor::construct(Queue<Token *> &tokens) {
	Mode mode = Mode::Initial;
	unique_ptr<Token> token;
	
	if (tokens.isempty()) return;
	token.reset(tokens.pop());
	
	// token 取り出し
	do {
		switch (mode) {
			case Mode::Initial:
				switch (token->type) {
					case Token::Type::Character:
						// ignore
						break;
					
					case Token::Type::Comment:
						// Append a Comment node to the Document object with the data attribute set to the data given in the comment token.
						break;
					
					case Token::Type::DOCTYPE:
						/* parseError の条件あり */
						
						document.doctype.name = token->data;
						// publicId and systemId も
						
						mode = Mode::BeforeHtml;
						break;
					
					default:
						// If the document is not an iframe srcdoc document, then this is a parse error; set the Document to quirks mode.

						// In any case, switch the insertion mode to "before html", then reprocess the current token.
						mode = Mode::BeforeHtml;
						continue;
				}
				break;
			
			case Mode::BeforeHtml: {
				auto actAsAnythingElse = [&] {
					// Create an html element. Append it to the Document object. Put this element in the stack of open elements.
	
					// If the Document is being loaded as part of navigation of a browsing context, then: run the application cache selection algorithm with no manifest, passing it the Document object.

					// Switch the insertion mode to "before head", then reprocess the current token.
					mode = Mode::BeforeHead;
				};
				switch (token->type) {
					case Token::Type::DOCTYPE:
						parseError();
						// ignore
						break;
					
					case Token::Type::Comment:
						// Append a Comment node to the Document object with the data attribute set to the data given in the comment token.
						break;
					
					case Token::Type::Character:
						// ignore
						break;
					
					case Token::Type::StartTag:
						if (token->data == "html") {
							// Create an element for the token in the HTML namespace. Append it to the Document object. Put this element in the stack of open elements.

							// If the Document is being loaded as part of navigation of a browsing context, then: if the newly created element has a manifest attribute whose value is not the empty string, then resolve the value of that attribute to an absolute URL, relative to the newly created element, and if that is successful, run the application cache selection algorithm with the resulting absolute URL with any <fragment> component removed; otherwise, if there is no such attribute, or its value is the empty string, or resolving its value fails, run the application cache selection algorithm with no manifest. The algorithm must be passed the Document object.
							
							mode = Mode::BeforeHead;
						} else {
							actAsAnythingElse();
							continue;
						}
						break;
					
					case Token::Type::EndTag:
						if (token->data == "head" || token->data == "body" || token->data == "html" || token->data == "br") {
							actAsAnythingElse();
							continue;
						} else {
							parseError();
							// ignore
						}
						break;
					
					default:
						actAsAnythingElse();
						continue;
				}
				break;
			}
			
			case Mode::BeforeHead: {
				auto actAsAnythingElse = [&] {
					// Act as if a start tag token with the tag name "head" and no attributes had been seen, then reprocess the current token.
					// Insert an HTML element for the token.
					
					// Set the head element pointer to the newly created head element.
					
					mode = Mode::InHead;
				};
				switch (token->type) {
					case Token::Type::Character:
						if (token->data == "\t" || token->data == "\n" || token->data == "\f" || token->data == "\r" || token->data == " ") {
							// ignore
						} else {
							actAsAnythingElse();
							continue;
						}
						break;
					
					case Token::Type::Comment:
						// Append a Comment node to the Document object with the data attribute set to the data given in the comment token.
						break;
					
					case Token::Type::DOCTYPE:
						parseError();
						// ignore
						break;
					
					case Token::Type::StartTag:
						if (token->data == "html") {
							mode = Mode::InBody;
							continue;
						} else if (token->data == "head") {
							// Insert an HTML element for the token.
							
							// Set the head element pointer to the newly created head element.
							
							mode = Mode::InHead;
						}
						break;
					
					case Token::Type::EndTag:
						if (token->data == "head" || token->data == "body" || token->data == "html" || token->data == "br") {
							actAsAnythingElse();
							continue;
						} else {
							parseError();
							// ignore
						}
						break;
					
					default:
						actAsAnythingElse();
						continue;
				}
				break;
			}
			
			case Mode::InHead:
				break;
		}
		
		token.reset(tokens.pop());
	} while (!tokens.isempty());
	
	return document;
}

void parseError() {
	
}
