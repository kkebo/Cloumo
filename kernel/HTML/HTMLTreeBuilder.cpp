#include <Stack.h>
#include "HTMLTreeBuilder.h"

using namespace HTML;

enum class TreeBuilder::Mode {
	Initial,
	BeforeHtml,
	BeforeHead,
	InHead,
	InHeadNoscript,
	AfterHead,
	InBody,
	Text,
	InTable,
	InTableText,
	InCaption,
	InColumnGroup,
	InTableBody,
	InRow,
	InCell,
	InSelect,
	InSelectInTable,
	InTemplate,
	AfterBody,
	InFrameset,
	AfterFrameset,
	AfterAfterBody,
	AfterAfterFrameseet
};

Document &TreeBuilder::construct(Queue<shared_ptr<Token>> &tokens) {
	Mode mode = Mode::Initial;
	shared_ptr<Token> token;
	Stack<shared_ptr<Node>> openTags(256); // stack of open elements
	bool scripting = false; // scripting flag
	
	if (tokens.isempty()) return document;
	token = tokens.pop();
	
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
						
						document.appendChild(shared_ptr<Node>(new DocumentType(token->data)));
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
					shared_ptr<Node> elem(new Element(token->data));
					document.appendChild(elem);
					openTags.push(elem);
	
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
							// Create an element for the token in the HTML namespace.
							shared_ptr<Node> elem(new Element(token->data));
							// Append it to the Document object.
							document.appendChild(elem);
							// Put this element in the stack of open elements.
							openTags.push(elem);

							// If the Document is being loaded as part of navigation of a browsing context,
							// then: if the newly created element has a manifest attribute whose value is not the empty string,
							// then resolve the value of that attribute to an absolute URL, relative to the newly created element,
							// and if that is successful, run the application cache selection algorithm
							// with the resulting absolute URL with any <fragment> component removed;
							// otherwise, if there is no such attribute, or its value is the empty string,
							// or resolving its value fails, run the application cache selection algorithm with no manifest.
							// The algorithm must be passed the Document object.
							
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
							shared_ptr<Node> elem(new Element(token->data));
							openTags.top()->appendChild(elem);
							openTags.push(elem);
							
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
			
			case Mode::InHead: {
				auto actAsAnythingElse = [&] {
					openTags.pop();
					mode = Mode::AfterHead;
				};
				switch (token->type) {
					case Token::Type::Character:
						break;
					
					case Token::Type::Comment:
						break;
					
					case Token::Type::DOCTYPE:
						break;
					
					case Token::Type::StartTag:
						if (token->data == "html") {
							
						} else if (token->data == "base" || token->data == "basefont" || token->data == "bgsound" || token->data == "link") {
							
						} else if (token->data == "meta") {
							
						} else if (token->data == "title") {
							// Follow the generic RCDATA element parsing algorithm.
						} else if ((token->data == "noscript" && scripting) || token->data == "noframes" || token->data == "style") {
							
						} else if (token->data == "noscript") { // && !scripting
							
						} else if (token->data == "script") {
							
						} else if (token->data == "template") {
							
						} else if (token->data == "head") {
							parseError();
							// ignore
						}
						break;
					
					case Token::Type::EndTag:
						if (token->data == "head") {
							openTags.pop();
							mode = Mode::AfterHead;
						} else if (token->data == "body" || token->data == "html" || token->data == "br") {
							actAsAnythingElse();
							continue;
						} else if (token->data == "template") {
							
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
			
			case Mode::InHeadNoscript:
				break;
			
			case Mode::AfterHead:
				switch (token->type) {
					case Token::Type::Character:
						break;
					
					case Token::Type::Comment:
						break;
					
					case Token::Type::DOCTYPE:
						break;
					
					case Token::Type::StartTag:
						if (token->data == "html") {
							
						} else if (token->data == "body") {
							openTags.push(openTags.top()->appendChild(shared_ptr<Node>(new Element(token->data))));
							
							// Set the frameset-ok flag to "not ok".
							
							mode = Mode::InBody;
						} else if (token->data == "frameset") {
							
						} else if (token->data == "base"
						|| token->data == "basefont"
						|| token->data == "bgsound"
						|| token->data == "link"
						|| token->data == "meta"
						|| token->data == "noframes"
						|| token->data == "script"
						|| token->data == "style"
						|| token->data == "template"
						|| token->data == "title") {
							parseError();
							
							
						} else if (token->data == "head") {
							parseError();
							// ignore
						}
						break;
					
					case Token::Type::EndTag:
						if (token->data == "template") {
							
						} else if (token->data == "body" || token->data == "html" || token->data == "br") {
							 
						} else {
							parseError();
							// ignore
						}
						break;
					
					default:
						break;
				}
				break;
			
			case Mode::InBody:
				switch (token->type) {
					case Token::Type::Character:
						/*if (token->data == 0) {
							parseError();
							// ignore
						} else */
						if (token->data == "\t" || token->data == "\n" || token->data == "\f" || token->data == "\r" || token->data == " ") {
							// Reconstruct the active formatting elements, if any.

							// Insert the token's character.
						} else {
							// Reconstruct the active formatting elements, if any.

							// Insert the token's character.

							// Set the frameset-ok flag to "not ok".
						}
						break;
					
					case Token::Type::Comment:
						break;
					
					case Token::Type::DOCTYPE:
						parseError();
						// ignore
						break;
					
					case Token::Type::StartTag:
						if (token->data == "html") {
							
						} else if (token->data == "base"
						|| token->data == "basefont"
						|| token->data == "bgsound"
						|| token->data == "link"
						|| token->data == "meta"
						|| token->data == "noframes"
						|| token->data == "script"
						|| token->data == "style"
						|| token->data == "template"
						|| token->data == "title") {
							
						} else if (token->data == "body") {
							
						} else if (token->data == "frameset") {
							
						} else if (token->data == "address"
						|| token->data == "article"
						|| token->data == "aside"
						|| token->data == "blockquote"
						|| token->data == "center"
						|| token->data == "details"
						|| token->data == "dialog"
						|| token->data == "dir"
						|| token->data == "div"
						|| token->data == "dl"
						|| token->data == "fieldset"
						|| token->data == "figcaption"
						|| token->data == "figure"
						|| token->data == "footer"
						|| token->data == "header"
						|| token->data == "hgroup"
						|| token->data == "main"
						|| token->data == "menu"
						|| token->data == "nav"
						|| token->data == "ol"
						|| token->data == "p"
						|| token->data == "section"
						|| token->data == "summary"
						|| token->data == "ul") {
							// If the stack of open elements does not have an element in scope that is an HTML element with
							// the same tag name as that of the token, then this is a parse error; ignore the token.

							// Otherwise, run these steps:
							// 1. Generate implied end tags.
							// 2. If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.
							// 3. Pop elements from the stack of open elements until an HTML element with the same tag name as the token has been popped from the stack.
						} else if (token->data == "h1"
						|| token->data == "h2"
						|| token->data == "h3"
						|| token->data == "h4"
						|| token->data == "h5"
						|| token->data == "h6") {
							openTags.push(openTags.top()->appendChild(shared_ptr<Node>(new Element(token->data))));
							// If the stack of open elements does not have an element in scope that is an HTML element and
							// whose tag name is one of "h1", "h2", "h3", "h4", "h5", or "h6", then this is a parse error; ignore the token.

							// Otherwise, run these steps:
							// 1. Generate implied end tags.
							// 2. If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.
							// 3. Pop elements from the stack of open elements until an HTML element whose tag name is one of "h1", "h2", "h3", "h4", "h5", or "h6" has been popped from the stack.
						} else if (token->data == "pre" || token->data == "listing") {
							
						} else if (token->data == "form") {
							
						} else if (token->data == "li") {
							
						} else if (token->data == "dd" || token->data == "dt") {
							
						} else if (token->data == "plaintext") {
							
						} else if (token->data == "button") {
							
						} else if (token->data == "a") {
							
						} else if (token->data == "b"
						|| token->data == "big"
						|| token->data == "code"
						|| token->data == "em"
						|| token->data == "font"
						|| token->data == "i"
						|| token->data == "s"
						|| token->data == "small"
						|| token->data == "strike"
						|| token->data == "strong"
						|| token->data == "tt"
						|| token->data == "u") {
							
						} else if (token->data == "nobr") {
							
						} else if (token->data == "applet" || token->data == "marquee" || token->data == "object") {
							
						} else if (token->data == "table") {
							
						} else if (token->data == "area"
						|| token->data == "br"
						|| token->data == "embed"
						|| token->data == "img"
						|| token->data == "keygen"
						|| token->data == "wbr") {
							
						} else if (token->data == "input") {
							
						} else if (token->data == "menuitem"
						|| token->data == "param"
						|| token->data == "source"
						|| token->data == "track") {
							
						} else if (token->data == "hr") {
							
						} else if (token->data == "image") {
							
						} else if (token->data == "isindex") {
							
						} else if (token->data == "textarea") {
							
						} else if (token->data == "xmp") {
							
						} else if (token->data == "iframe") {
							
						} else if (token->data == "noembed" || (token->data == "noscript" && scripting)) {
							
						} else if (token->data == "select") {
							
						} else if (token->data == "optgroup" || token->data == "option") {
							
						} else if (token->data == "rp" || token->data == "rt") {
							
						} else if (token->data == "math") {
							
						} else if (token->data == "svg") {
							
						} else if (token->data == "caption"
						|| token->data == "col"
						|| token->data == "colgroup"
						|| token->data == "frame"
						|| token->data == "head"
						|| token->data == "tbody"
						|| token->data == "td"
						|| token->data == "tfoot"
						|| token->data == "th"
						|| token->data == "thead"
						|| token->data == "tr") {
							
						} else {
							
						}
						break;
					
					case Token::Type::EndTag:
						if (token->data == "template") {
							// Process the token using the rules for the "in head" insertion mode.
						} else if (token->data == "body") {
							// If the stack of open elements does not have a body element in scope, this is a parse error; ignore the token.

							// Otherwise, if there is a node in the stack of open elements that is not either
							// a dd element, a dt element, an li element, an optgroup element, an option element,
							// a p element, an rp element, an rt element, a tbody element, a td element, a tfoot element,
							// a th element, a thead element, a tr element, the body element, or the html element,
							// then this is a parse error.

							// Switch the insertion mode to "after body".
							mode = Mode::AfterBody;
						} else if (token->data == "html") {
							
						} else if (token->data == "address"
						|| token->data == "article"
						|| token->data == "aside"
						|| token->data == "blockquote"
						|| token->data == "button"
						|| token->data == "center"
						|| token->data == "details"
						|| token->data == "dialog"
						|| token->data == "dir"
						|| token->data == "div"
						|| token->data == "dl"
						|| token->data == "fieldset"
						|| token->data == "figcaption"
						|| token->data == "figure"
						|| token->data == "footer"
						|| token->data == "header"
						|| token->data == "hgroup"
						|| token->data == "main"
						|| token->data == "menu"
						|| token->data == "nav"
						|| token->data == "ol"
						|| token->data == "pre"
						|| token->data == "section"
						|| token->data == "summary"
						|| token->data == "ul") {
							
						} else if (token->data == "form") {
							
						} else if (token->data == "p") {
							
						} else if (token->data == "li") {
							
						} else if (token->data == "dd" || token->data == "dt") {
							
						} else if (token->data == "h1"
						|| token->data == "h2"
						|| token->data == "h3"
						|| token->data == "h4"
						|| token->data == "h5"
						|| token->data == "h6") {
							
						} else if (token->data == "sarcasm") {
							
						} else if (token->data == "a"
						|| token->data == "b"
						|| token->data == "big"
						|| token->data == "code"
						|| token->data == "em"
						|| token->data == "font"
						|| token->data == "i"
						|| token->data == "nobr"
						|| token->data == "s"
						|| token->data == "small"
						|| token->data == "strike"
						|| token->data == "strong"
						|| token->data == "tt"
						|| token->data == "u") {
							
						} else if (token->data == "applet" || token->data == "marquee" || token->data == "object") {
							
						} else if (token->data == "br") {
							
						} else {
							
						}
						break;
					
					case Token::Type::EndOfFile:
						break;
				}
				break;
			
			case Mode::Text:
				break;
			
			case Mode::InTable:
				break;
			
			case Mode::InTableText:
				break;
			
			case Mode::InCaption:
				break;
			
			case Mode::InColumnGroup:
				break;
			
			case Mode::InTableBody:
				break;
			
			case Mode::InRow:
				break;
			
			case Mode::InCell:
				break;
			
			case Mode::InSelect:
				break;
			
			case Mode::InSelectInTable:
				break;
			
			case Mode::InTemplate:
				break;
			
			case Mode::AfterBody:
				switch (token->type) {
					case Token::Type::Character:
						break;
					
					case Token::Type::Comment:
						break;
					
					case Token::Type::DOCTYPE:
						break;
					
					case Token::Type::StartTag:
						break;
					
					case Token::Type::EndTag:
						if (token->data == "html") {
							// If the parser was originally created as part of the HTML fragment parsing algorithm, this is a parse error;
							// ignore the token. (fragment case)
							
							mode = Mode::AfterAfterBody;
						}
						break;
					
					case Token::Type::EndOfFile:
						// Stop parsing.
						break;
					
					default:
						break;
				}
				break;
			
			case Mode::InFrameset:
				break;
			
			case Mode::AfterFrameset:
				break;
			
			case Mode::AfterAfterBody:
				switch (token->type) {
					case Token::Type::Comment:
						break;
					
					case Token::Type::DOCTYPE:
						break;
					
					case Token::Type::Character:
						break;
					
					case Token::Type::StartTag:
						break;
					
					case Token::Type::EndOfFile:
						// Stop parsing.
						break;
					
					default:
						parseError();
						mode = Mode::InBody;
						continue;
				}
				break;
			
			case Mode::AfterAfterFrameseet:
				break;
		}
		
		token = tokens.pop();
	} while (!tokens.isempty());
	
	return document;
}

void TreeBuilder::parseError() {
	
}
