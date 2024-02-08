#ifndef LEXER_HEADER
#define LEXER_HEADER

#include "../TextParse/text_parse.h"
#include "../Stack/stack.h"
#include "../Common/trees.h"

typedef enum
{
    kLexerSuccess,
    kSyntaxError,
} LexerErrs_t;

LexerErrs_t SplitOnLexems(Text *text,
                          Stack *stk,
                          Identificators *vars);

#endif
