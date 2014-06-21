#include "headers.h"

void Tokenizer::tokenization(const char *inputstream, unsigned int filesize) {
	char input;
	int state = 0; // Data state
	StartTagToken *starttagtoken;
	
	for (int i = 0; i <= filesize; i++) {
		input = inputstream[i];
		switch (state) {
			case 0: // Data state
				if (i == filesize) { // EOF
					// Emit the end-of-file token.
					break;
				}
				switch (input) {
					case '&':
						// Switch to the character reference in data state.
						state = 1;
						break;
						
					case '<':
						// Switch to the tag open state.
						state = 7;
						break;
						
					case 0: // NULL
						// Parse Error.
						// Emit the current input character as a character token.
						break;
						
					default:
						// Emit the current input character as a character token.
						break;
				}
				break;
			
			case 1: // Character reference in data state
				break;
				
			case 2: // RCDATA state
				break;
				
			case 3: // Character reference in RCDATA state
				break;
				
			case 4: // RAWTEXT state
				break;
				
			case 5: // Script data state
				break;
				
			case 6: // PLAINTEXT state
				break;

			case 7: // Tag open state
				switch (input) {
					case '!':
						<#statements#>
						break;
						
					case '/':
						// Switch to the end tag open state.
						state = 3;
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
							break;
						}
						
						// Parse Error.
						break;
				}
				break;
			
			case 8: // End tag open state
				break;
			
			case 9: // Tag name state
				switch (input) {
					case 0x0009: // tab
					case 0x000a: // LF
					case 0x000c: // FF
					case ' ':
						// Switch to the before attribute name state.
						break;

					case '/':
						// Switch to the self-closing start tag state.
						break;
						
					case '>':
						// Emit the current tag token.
						state = 0;
						break;
						
					case 0:
						// Parse error.
						// Append a U+FFFD REPLACEMENT CHARACTER character to the current tag token's tag name.
						break;
						
					default:
						if ('A' <= input && input <= 'Z')
							input += 0x0020;
						// Append the current input character to the current tag token's tag name.
						break;
				}
				break;
			
			case 33: // Before attribute name state
				switch (input) {
					case 0x09:
					case 0x0a:
					case 0x0c:
					case ' ':
						// Ignore
						break;
					
					case '/':
						// Switch to the self-closing start tag state.
						state = 42;
						break;
				}
				break;
			
			case 42: // Self-closing start tag state
				switch (input) {
					case '>':
						// Set the self-closing flag of the current tag token. Switch to the data state. Emit the current tag token.
						state = 0;
						break;
				}
				break;
				
			default:
				break;
		}
	}
}