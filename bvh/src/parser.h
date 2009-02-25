#ifndef _PARSER_H
#define _PARSER_H

#include <assert.h>

typedef struct
{
	signed char lex[256];
	
	char *filename;
	FILE *in;

        char *string;
        int string_size;
	char *unget;
	int line;
} PARSER;


enum {parser_undef= 0, parser_lex= 1, parser_separator= 2};

extern PARSER *parser_init_open(PARSER *parser, char *filename);
extern void parser_close(PARSER *parser);

extern void parser_set_tokens(PARSER *parser, char *tokens);
extern void parser_set_separators(PARSER *parser, char *separators);
extern void parser_set_string(PARSER *parser, char *tmp, int tmp_size);

extern int parser_get_token(PARSER *parser);
extern int parser_scan_token(PARSER *parser, char *token);
extern int parser_casescan_token(PARSER *parser, char *token);
extern void parser_unget_token(PARSER *parser, char *tmp);

static inline char *parser_get_string(PARSER *parser)
{
    assert(parser!=NULL);

    return parser->string;
}

#endif
