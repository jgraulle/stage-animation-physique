#ifndef _PARSER_H
#define _PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

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

PARSER *parser_init_open(PARSER *parser, char *filename);
void parser_close(PARSER *parser);

void parser_set_tokens(PARSER *parser, char *tokens);
void parser_set_separators(PARSER *parser, char *separators);
void parser_set_string(PARSER *parser, char *tmp, int tmp_size);

int parser_get_token(PARSER *parser);
int parser_scan_token(PARSER *parser, char *token);
int parser_casescan_token(PARSER *parser, char *token);
void parser_unget_token(PARSER *parser, char *tmp);

static inline char *parser_get_string(PARSER *parser)
{
    assert(parser!=NULL);

    return parser->string;
}

#ifdef __cplusplus
}
#endif

#endif
