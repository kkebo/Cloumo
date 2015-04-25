#ifndef _HTMLTOKENIZER_H_
#define _HTMLTOKENIZER_H_

class HTMLTokenizer {
private:
	char *tokens
public:
	tokenize(const char *, unsigned int);

private:
	emit(const char *);
};

#endif
