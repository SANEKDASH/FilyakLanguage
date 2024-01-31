#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "../Common/trees.h"
#include "../Stack/stack.h"
#include "../Common/NameTable.h"

static TreeNode *GetOperation    (Expr *expr);
static TreeNode *GetNumber       (Expr *expr);
static TreeNode *GetOperation    (Expr *expr);
static TreeNode *GetIdentificator(Expr      *expr,
                                  Variables *vars);

static int GetLexem(Stack     *stk,
                    Expr      *expr,
                    Variables *vars);

static void SkipExprSpaces(Expr *expr);

static bool RusIsalpha(char c);

static bool IsOperator(char c);

//==============================================================================


LexerErrs_t SplitOnLexems(Text      *text,
                          Stack     *stk,
                          Variables *vars)
{
    InitLog();

    #define CUR_CHAR *text->lines_ptr[i]

    for (size_t i = 0; i < text->lines_count; i++)
    {
        Expr expr;

        expr.pos         = 0;
        expr.string      = text->lines_ptr[i].str;
        expr.line_number = text->lines_ptr[i].real_line_number;

        while (expr.string[expr.pos] != '\0')
        {
            if (GetLexem(stk, &expr, vars) < 0)
            {
                printf("Syntax error. POS: %d, LINE: %d\n", expr.pos + 1, i + 1);

                return kSyntaxError;
            }
        }
    }

    CloseLog();

    return kLexerSuccess;
}

//==============================================================================

#define OP_CTOR(op_code) NodeCtor(nullptr, nullptr       ,nullptr, kOperator,      op_code)
#define NUM_CTOR(val)    NodeCtor(nullptr, nullptr       ,nullptr, kConstNumber,   val)
#define ID_CTOR(pos)     NodeCtor(nullptr, nullptr       ,nullptr, kIdentificator, pos)

#define CUR_CHAR expr->string[expr->pos]
#define STRING   expr->string
#define POS      expr->pos
#define CUR_STR  expr->string + expr->pos

#define D_PR printf("LINE: %d, STRING[%s], POS: %d\n", __LINE__, CUR_STR + POS, POS);

//==============================================================================

static int GetLexem(Stack     *stk,
                    Expr      *expr,
                    Variables *vars)
{
    SkipExprSpaces(expr);

    TreeNode *node = nullptr;

    if (isdigit(CUR_CHAR) || CUR_CHAR == '-')
    {
        Push(stk, GetNumber(expr));
    }
    else if (IsOperator(CUR_CHAR))
    {
        TreeNode *node = GetOperation(expr);

        if (node == nullptr)
        {
            node = GetIdentificator(expr, vars);
        }

        if (Push(stk, node) != kStackClear)
        {
            printf("Failed to push");
        }
    }
    else
    {
        printf(">>LINE %d:\n"
               "  Что ты блять пишешь? '%s'\n", STRING);
        return -1;
    }

    return 0;
}

//==============================================================================

static TreeNode *GetNumber(Expr *expr)
{
    SkipExprSpaces(expr);

    double  val     = 0;
    size_t  old_pos = POS;
    char   *num_end = nullptr;

    val = strtod(STRING + POS, &num_end);

    POS = num_end - STRING;

    if (POS <= old_pos)
    {
        printf(">>LINE %d:\n"
               "  Ты писать числа разучился? '%s'\n", expr->line_number, STRING);
    }

    SkipExprSpaces(expr);

    TreeNode *number =  NodeCtor(nullptr,
                                 nullptr,
                                 nullptr,
                                 kConstNumber,
                                 val);

    number->line_number = expr->line_number;

    return number;
}

//==============================================================================

static TreeNode *GetOperation(Expr *expr)
{
    SkipExprSpaces(expr);

    TreeNode *node = nullptr;

    for (size_t i = 0; i < kKeyWordCount; i++)
    {
        if (strncmp(expr->string + expr->pos,
                    NameTable[i].key_word,
                    NameTable[i].word_len) == 0)
        {
            node = OP_CTOR(NameTable[i].key_code);

            node->line_number = expr->line_number;

            POS += NameTable[i].word_len;
        }
    }

    SkipExprSpaces(expr);

    return node;
}

//==============================================================================

static const int kMaxIdLen = 64;

static TreeNode *GetIdentificator(Expr      *expr,
                                  Variables *vars)
{
    SkipExprSpaces(expr);

    static char var_name[kMaxIdLen] = {0};

    var_name[0] = '\0';

    size_t i = 0;

    while (isalpha   (CUR_CHAR)    ||
           RusIsalpha(CUR_CHAR)    ||
                      CUR_CHAR == '_')
    {
        var_name[i++] = CUR_CHAR;

        ++POS;
    }

    var_name[i] = '\0';

    int id_pos = SeekVariable(vars, var_name);

    if (id_pos < 0)
    {
        id_pos = AddVar(vars, strdup(var_name));
    }

    SkipExprSpaces(expr);

    TreeNode *id_node =  ID_CTOR(id_pos);

    id_node->line_number = expr->line_number;

    return id_node;
}

//==============================================================================

static void SkipExprSpaces(Expr *expr)
{
    while (isspace(CUR_CHAR))
    {
        POS++;
    }
}

//==============================================================================

static bool RusIsalpha(char c)
{
    return (c >= 'а' && c <= 'я') ||
           (c >= 'А' && c <= 'Я') ||
           (c == 'ё' || c == 'Ё');
}

//==============================================================================

static bool IsOperator(char c)
{
    return c == '?'      ||
           c == ';'      ||
           c == ','      ||
           c == '_'      ||
           RusIsalpha(c) ||
           isalpha(c);

}
