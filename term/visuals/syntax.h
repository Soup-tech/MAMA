#ifndef SYNTAX_H
#define SYNTAX_H

#define MAX_SYNTAX_SWITCHES 40

enum SyntaxState {
	CMD_NAME,
	PARAM_NAME,
	PARAM_VALUE,
	DOUBLE_QUOTE_STRING,
	DOUBLE_QUOTE_STRING_END_QUOTE,
	SINGLE_QUOTE_STRING,
	SINGLE_QUOTE_STRING_END_QUOTE,
	DEFAULT
};

#define SYNTAX_COLOR_CMD_NAME BLUE
#define SYNTAX_COLOR_PARAM_NAME MAGENTA
#define SYNTAX_COLOR_PARAM_VALUE WHITE
#define SYNTAX_COLOR_DOUBLE_QUOTE_STRING YELLOW
#define SYNTAX_COLOR_SINGLE_QUOTE_STRING YELLOW
#define SYNTAX_COLOR_DEFAULT WHITE

void syntax_init();
void syntax_handle_char(char, int);

#endif
