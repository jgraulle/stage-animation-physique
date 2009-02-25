/*
	parser.c
	
	TODO : ecrire un  vrai parser 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "parser.h"

PARSER *parser_init_open(PARSER *parser, char *filename)
{
	FILE *in;
	int i;
	
	in= fopen(filename, "rt");
	if(in==NULL)
	{
		printf("parser : open failed\n");
		return NULL;
	}

	if(parser==NULL)
	{
		parser= (PARSER *) malloc(sizeof(PARSER));
		assert(parser!=NULL);
	}
	
	parser->in= in;
	parser->filename= strdup(filename);
        parser->string= NULL;
        parser->string_size= 0;
	parser->unget= NULL;
	parser->line= 1;

	// insere les caracteres alpha numeriques dans la liste des tokens
	for(i= 0; i < 256; i++)
	{
		if(isalnum(i))
			parser->lex[i]= parser_lex;
		else
			parser->lex[i]= parser_undef;
	}
	
	return parser;
}

void parser_close(PARSER *parser)
{
	assert(parser!=NULL);

	if(parser->filename!=NULL)
		free(parser->filename);
	
	if(parser->unget!=NULL)
		free(parser->unget);

	fclose(parser->in);
}

void parser_set_tokens(PARSER *parser, char *tokens)
{
	int i;
	
	assert(parser!=NULL);
	
	for(i= 0; tokens[i]!=0; i++)
		parser->lex[(unsigned int) tokens[i]]= parser_lex;
}

void parser_set_separators(PARSER *parser, char *separators)
{
	int i;
	
	assert(parser!=NULL);
	
	for(i= 0; separators[i]!=0; i++)
		parser->lex[(unsigned int) separators[i]]= parser_separator;
}

void parser_set_string(PARSER *parser, char *tmp, int tmp_size)
{
    assert(parser!=NULL);
    
    parser->string= tmp;
    parser->string_size= tmp_size;
}


static inline int is_token(PARSER *parser, char c)
{
	if(c < 0)
		return 0;
	
	return (parser->lex[(unsigned int) c]==parser_lex);
}

static inline int is_separator(PARSER *parser, char c)
{
	if(c < 0)
		return 0;

	return (parser->lex[(unsigned int) c]==parser_separator);
}


int parser_get_token(PARSER *parser)
{
	char c;
	int i= 0;

	assert(parser!=NULL);
	assert(parser->in!=NULL);
	assert(parser->string!=NULL);
	
	if(parser->unget!=NULL)
	{
		strncpy(parser->string, parser->unget, parser->string_size);
		parser->string[parser->string_size -1]= 0;
		
		free(parser->unget);
		parser->unget= NULL;
		
		return 0;
	}
	
	// sauter les blancs
	c= fgetc(parser->in);
	while(c!=EOF && (c==' ' || c=='\t'))
		c= fgetc(parser->in);

	// lire tous les caracteres alphanumeriques
	while(c!=EOF && is_token(parser, c))
	{
		parser->string[i++]= c;
		
		if(i>=parser->string_size)
		{
			printf("parser : line length > %d\n", parser->string_size);
			return EOF;
		}
		
		c= fgetc(parser->in);
	}

	// separateurs
	if(is_separator(parser, c))
	{
		if(i==0)
			parser->string[i++]= c;
		else
			ungetc(c, parser->in);
	}
	
	// terminer la chaine
	parser->string[i]= 0;

	// indiquer la fin de la ligne ou du fichier
	if(i==0)
		return c;
	
	// forcer une fin de ligne avant la fin du fichier
	if(c==EOF)
		ungetc('\n', parser->in);
		
	else if(c=='\n')
		ungetc(c, parser->in);

	if(c=='\r' || c=='\n')
		parser->line+= 1;
	
	return 0;
}


void parser_unget_token(PARSER *parser, char *tmp)
{
	assert(parser!=NULL);
	
	if(parser->unget!=NULL)
		free(parser->unget);
	
	parser->unget= strdup(tmp);
}


int parser_scan_token(PARSER *parser, char *token)
{
	int tok;
    
	assert(parser!=NULL);
	assert(parser->string!=NULL);
    
	tok= parser_get_token(parser);
	while(tok!=EOF)
	{
		if(strcmp(parser->string, token)==0)
			break;
		tok= parser_get_token(parser);
	}
	
	if(tok==EOF)
		return -1;
	else
		return 0;
}

int parser_casescan_token(PARSER *parser, char *token)
{
	int tok;
	
	assert(parser!=NULL);
	assert(parser->string!=NULL);

        tok= parser_get_token(parser);
	while(tok!=EOF)
	{
		if(strcasecmp(parser->string, token)==0)
			break;
		tok= parser_get_token(parser);
	}
	
	if(tok==EOF)
		return -1;
	else
		return 0;
}
