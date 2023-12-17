#include <stdio.h>
#include <ctype.h>

#include "lexer.h"
#include "parse.h"
#include "../Common/trees.h"
#include "lexer.h"
#include "../Stack/stack.h"

static const char *output_file_name = "TOP_G_DUMP.txt";
static FILE *output_file = nullptr;


#define LOG_PRINT(...) fprintf(output_file, __VA_ARGS__);

static void SkipSpaces(Expr *expr);

TreeNode *GetG(Variables  *vars,
               const char *file_name)
{
    output_file = fopen(output_file_name, "w");
    LOG_PRINT("im GetG leading all work\n\n");

    Text program;
    ReadTextFromFile(&program, file_name, SplitBufIntoLines);

    Stack lexems;
    StackInit(&lexems);

    SplitOnLexems(&program, &lexems, vars);

    size_t i = 0;
    TreeNode *node = GetL(vars, &lexems, &i);;
    SetParents(node);


    fclose(output_file);

    LOG_PRINT("GetG finished work\n");

    return node;
}

//==============================================================================

TreeNode *GetL(Variables *vars, Stack *stk, size_t *iter)
{
    #define CUR_NODE      stk->stack_data.data[*iter]
    #define CUR_NODE_TYPE stk->stack_data.data[*iter]->type
    #define CUR_NODE_DATA stk->stack_data.data[*iter]->data.key_word_code
    #define DPR           printf("POS %d, LINE %d, FUNC %s\n", *iter, __LINE__, __func__);

    if (*iter >= stk->stack_data.size)
    {
        return nullptr;
    }

    if (CUR_NODE_TYPE == kEndOfLine)
    {
DPR
        TreeNode *node = CUR_NODE;
        ++*iter;
DPR
        node->left = GetE(vars, stk, iter);
        printf("%p\n", node);

DPR

        if (node->left == nullptr)
        {
            return nullptr;//?
        }
DPR

        node->right = GetL(vars, stk, iter);
DPR

        return node;
    }
    printf("syntax error POS %d", *iter);

    return nullptr;
}


TreeNode *GetP(Variables *vars, Stack *stk, size_t *iter)
{
DPR
    printf("POS -> %d, LINE -> %d. FUNC %s\n", *iter, __LINE__, __func__);

    LOG_PRINT("i'm getP reading '(' and ')' on pos %d\n", *iter);

    TreeNode *node = nullptr;
DPR
    if (*iter <stk->stack_data.size)
    {
        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftBracket)
        {
            ++*iter;

            node = GetE(vars, stk, iter);
    DPR
            if (CUR_NODE_TYPE != kOperator && CUR_NODE_DATA != kRightBracket)
            {
                printf("GetP() syntax error stk pos %d\n", *iter);

                return nullptr;
            }

            ++*iter;

            return node;
        }
        else
        {
            return GetA(vars, stk, iter);
        }
    }
    DPR

    return node;
}

//==============================================================================

TreeNode *GetT(Variables *vars, Stack *stk, size_t *iter)
{
DPR
    LOG_PRINT("i'm getT reading '*' and '/' on pos %d\n", *iter);

    TreeNode* node_lhs = GetP(vars, stk, iter);
DPR

    if (*iter < stk->stack_data.size)
    {
        while ((*iter < stk->stack_data.size) &&
                CUR_NODE_TYPE == kOperator &&
               (CUR_NODE_DATA == kMult || CUR_NODE_DATA == kDiv ))
        {

    DPR
            TreeNode *op = CUR_NODE;

            ++*iter;

            TreeNode *node_rhs = GetP(vars, stk, iter);

            op->left  = node_lhs;
            op->right = node_rhs;

            node_lhs = op;
        }
    DPR
    }
    DPR
    return node_lhs;
}

//==============================================================================

TreeNode *GetA(Variables *vars, Stack *stk, size_t *iter)
{
    LOG_PRINT("Im GetA[rgument] reading args on pos %d,\n", *iter);

DPR
    if (CUR_NODE_TYPE == kConstNumber)
    {
        return GetN(vars, stk, iter);
    }
    else
    {
        return GetId(vars, stk, iter);
    }
}

//==============================================================================

TreeNode *GetE(Variables *vars, Stack *stk, size_t *iter)
{
    LOG_PRINT("i'm getE reading '+' and '-' op pos %d\n", *iter);

    TreeNode *node_lhs = GetT(vars, stk, iter);
DPR
    if (*iter < stk->stack_data.size)
    {
        while ((*iter < stk->stack_data.size) &&
                 CUR_NODE_TYPE == kOperator &&
                (CUR_NODE_DATA == kAdd ||
                 CUR_NODE_DATA == kSub ||
                 CUR_NODE_DATA == kAssign))
        {
            TreeNode *op = CUR_NODE;

            if (op->data.key_word_code == kAssign)
            {
                if (node_lhs->type != kIdentificator)
                {
                    printf("Expected identificator POS %d\n ", *iter);

                    return nullptr;
                }
            }

            ++*iter;

            TreeNode *node_rhs = GetT(vars, stk, iter);

            op->left  = node_lhs;
            op->right = node_rhs;

            node_lhs = op;
        }
    }
   DPR

    return node_lhs;
}

//==============================================================================

TreeNode* GetN(Variables *vars, Stack *stk, size_t *iter)
{
    LOG_PRINT("i'm getN reading numbers on pos %d\n",*iter);

    if (CUR_NODE_TYPE == kConstNumber)
    {
        TreeNode *node = CUR_NODE;
        ++*iter;

    LOG_PRINT("i'm getN rode numbers %d\n",*iter);

        return node;
    }
}

//==============================================================================

static const int kMaxIdLen = 64;

TreeNode *GetId(Variables *vars, Stack *stk, size_t *iter)
{
    LOG_PRINT("Im GetV reading variables on pos %d,\n", *iter);

    if (CUR_NODE_TYPE == kIdentificator)
    {
        TreeNode *node = CUR_NODE;
        ++*iter;

        LOG_PRINT("Im GetV rode variables on pos %d,\n", *iter);

        return node;
    }

    return nullptr;
}

//==============================================================================
