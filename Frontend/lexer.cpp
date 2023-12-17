#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "../Common/trees.h"
#include "../Stack/stack.h"
#include "../Common/NameTable.h"

static TreeNode *GetOperation(Expr *expr);
static TreeNode *GetNumber(Expr *expr);
static TreeNode *GetOperation(Expr *expr);
static TreeNode *GetId(Expr      *expr,
                       Variables *vars);

static int GetLexem(Stack     *stk,
                    Expr      *expr,
                    Variables *vars);

static void SkipExprSpaces(Expr *expr);

LexerErrs_t SplitOnLexems(Text      *text,
                          Stack     *stk,
                          Variables *vars)
{
    InitLog();

    #define CUR_CHAR *text->lines_ptr[i]

    for (size_t i = 0; i < text->lines_count; i++)
    {
        printf("%d", i);
        Expr expr;
        expr.string = text->lines_ptr[i];
        expr.pos    = 0;

        #define OP_CTOR(op_code) NodeCtor(nullptr, nullptr, nullptr, kOperator,      op_code)
        #define NUM_CTOR(val)    NodeCtor(nullptr, nullptr, nullptr, kConstNumber,   val)
        #define ID_CTOR(pos)     NodeCtor(nullptr, nullptr, nullptr, kIdentificator, pos)
        #define EOL_CTOR(line)   NodeCtor(nullptr, nullptr, nullptr, kEndOfLine,     line)
        //       ^--------+
        //                |
        //  end of line---+

        Push(stk, EOL_CTOR(i));
        while (expr.string[expr.pos] != '\0')
        {
            if (GetLexem(stk, &expr, vars) < 0)
            {
                printf("Syntax error. POS %d\n", expr.pos);
                return kSyntaxError;
            }
        }

    }


    CloseLog();

    return kLexerSuccess;
}

//==============================================================================

#define CUR_CHAR expr->string[expr->pos]
#define STRING   expr->string
#define POS      expr->pos
#define CUR_STR  expr->string + expr->pos

//==============================================================================

static int GetLexem(Stack     *stk,
                    Expr      *expr,
                    Variables *vars)
{
        printf("POS %s\n", CUR_STR);
    if (isdigit(CUR_CHAR))
    {
        Push(stk, GetNumber(expr));
    }
    /*else if (CUR_CHAR == '(')
    {
        Push(stk, OP_CTOR(kLeftBracket));
        ++POS;
    }
    else if (CUR_CHAR == ')')
    {
        printf("huy");

        Push(stk, OP_CTOR(kRightBracket));
        ++POS;
    }*/
    else if (isalpha(CUR_CHAR) || CUR_CHAR == ')' || CUR_CHAR == '(')
    {
        printf("POS %s\n", CUR_STR);

        TreeNode *node = GetOperation(expr);
        printf("POINTR 1 %p\n", node);

        if (node == nullptr)
        {
            node = GetId(expr, vars);
        }
        printf("2 POINTR %p, TYPE = %d, \n", node, node->type);
        if (node ->type == kIdentificator)
        {
            printf("%s\n", vars->var_array[node->data.variable_pos].id);
        }
        if (Push(stk, node) != kStackClear)
        {
            printf("Failed to push");
        }
    }
    else
    {
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

    val = strtod(STRING+ POS, &num_end);

    POS = num_end - STRING;

    if (POS <= old_pos)
    {
        printf("GetNumber() syntax error pos %d, string %s\n", POS, STRING + POS);
    }

    SkipExprSpaces(expr);

    return NodeCtor(nullptr,
                    nullptr,
                    nullptr,
                    kConstNumber,
                    val);

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
            node = OP_CTOR(i + 1);
            POS += NameTable[i].word_len;
        }
    }

    SkipExprSpaces(expr);

    return node;
}

//==============================================================================

static const int kMaxIdLen = 64;

static TreeNode *GetId(Expr      *expr,
                       Variables *vars)
{
    SkipExprSpaces(expr);

    static char var_name[kMaxIdLen] = {0};

    var_name[0] = '\0';

    size_t i = 0;

    while (isalpha(CUR_CHAR))
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
    printf("VAR POS %s\n", CUR_STR);

    return ID_CTOR(id_pos);
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
