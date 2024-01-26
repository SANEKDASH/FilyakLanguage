#include <stdio.h>
#include <ctype.h>

#include "lexer.h"
#include "parse.h"
#include "../Common/trees.h"
#include "../Stack/stack.h"

static const char *output_file_name = "TOP_G_DUMP.txt";
static FILE       *output_file      = nullptr;

#define LOG_PRINT(...) fprintf(output_file, __VA_ARGS__);

static bool IsUnaryOp      (KeyCode_t op_code);
static bool IsBinaryOpLower(KeyCode_t op_code);
static bool IsType         (KeyCode_t op_code);
static bool IsFunc         (KeyCode_t op_code);

static void SkipSpaces(Expr *expr);

static TreeNode *GetDeclaration  (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetConditionalOp(Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetParams       (Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetG(Variables  *vars,
               const char *file_name)
{
    output_file = fopen(output_file_name, "w");


    LOG_PRINT("im GetG leading all work\n\n");

    Text program;
    ReadTextFromFile(&program, file_name, SplitBufIntoLines);
    fclose(output_file);

    Stack lexems;
    StackInit(&lexems);
    SplitOnLexems(&program, &lexems, vars);
    size_t i = 0;
    TreeNode *node = GetL(vars, &lexems, &i);;
    SetParents(node);



    LOG_PRINT("GetG finished work\n");

    return node;
}

//==============================================================================

TreeNode *GetL(Variables *vars, Stack *stk, size_t *iter)
{
    #define OP_CTOR(op_code) NodeCtor(nullptr, nullptr, nullptr, kOperator, op_code)

    #define CUR_NODE       stk->stack_data.data[*iter]
    #define CUR_NODE_TYPE  stk->stack_data.data[*iter]->type
    #define CUR_NODE_DATA  stk->stack_data.data[*iter]->data.key_word_code

    #define CUR_ID_STATE   vars->var_array[stk->stack_data.data[*iter]->data.variable_pos].declaration_state
    #define CUR_ID_TYPE    vars->var_array[stk->stack_data.data[*iter]->data.variable_pos].id_type
    #define CUR_ID         vars->var_array[stk->stack_data.data[*iter]->data.variable_pos].id

    #define NEXT_NODE      stk->stack_data.data[*iter + 1]
    #define NEXT_NODE_TYPE stk->stack_data.data[*iter + 1]->type
    #define NEXT_NODE_DATA stk->stack_data.data[*iter + 1]->data.key_word_code

    #define DPR printf("POS %d, LINE %d, FUNC %s\n", *iter, __LINE__, __func__);


    if (*iter >= stk->stack_data.size)
    {
        return nullptr;
    }

    TreeNode *cur_op = nullptr;

    if (CUR_NODE_TYPE == kOperator)
    {
        if (CUR_NODE_DATA == kIf)
        {
            cur_op = OP_CTOR(kEndOfLine);

            cur_op->left  = GetConditionalOp(vars, stk, iter);

            cur_op->right = GetL(vars, stk, iter);

            return cur_op;
        }
        else if (IsType(CUR_NODE_DATA))
        {
            cur_op = GetDeclaration(vars, stk, iter);

            if (cur_op->type == kFuncDef)
            {
                TreeNode *next_func = OP_CTOR(kEndOfLine);

                next_func->left  = cur_op;

                next_func->right = GetL(vars, stk, iter);

                return next_func;
            }
        }
        else if (CUR_NODE_DATA == kReturn)
        {
    DPR
            cur_op = CUR_NODE;
            ++(*iter);
    DPR
            cur_op->right = GetE(vars, stk, iter);
        }
        else
        {
            cur_op = GetE(vars, stk, iter);
        }
    }
    else if (CUR_NODE_TYPE  == kIdentificator)
    {
        if (CUR_ID_TYPE == kVar)
        {
            if (NEXT_NODE_TYPE == kOperator &&
                NEXT_NODE_DATA == kAssign)
            {
                cur_op        = NEXT_NODE;
                cur_op->right = CUR_NODE;

                *iter += 2;

                cur_op->left = GetE(vars, stk, iter);
            }
        }
        else if (CUR_ID_TYPE == kFunc)
        {
            cur_op = GetId(vars, stk, iter);
        }
        else
        {
            printf(">> UNDEFINED IDENTIFICATOR - \"%s\", POS: %d\n", CUR_ID, *iter);
        }
    }
    else
    {
        cur_op->left = GetE(vars, stk, iter);

        if (cur_op->left == nullptr)
        {
            return nullptr;
        }
    }

    if (*iter < stk->stack_data.size)
    {
        if (CUR_NODE_TYPE == kOperator &&
            CUR_NODE_DATA == kEndOfLine)
        {
    DPR
            TreeNode *next_op = CUR_NODE;

            ++(*iter);

            next_op->left = cur_op;

            if (*iter < stk->stack_data.size)
            {
    DPR
                next_op->right = GetL(vars, stk, iter);
            }

            return next_op;
        }
    }

    return nullptr;;
}


TreeNode *GetP(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *node = nullptr;

    if (*iter <stk->stack_data.size)
    {
        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftBracket)
        {
            ++*iter;

            node = GetE(vars, stk, iter);

            if (CUR_NODE_TYPE != kOperator && CUR_NODE_DATA != kRightBracket)
            {
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

    return node;
}

//==============================================================================

TreeNode *GetT(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode* node_lhs = GetP(vars, stk, iter);

    if (*iter < stk->stack_data.size)
    {
        while ((*iter < stk->stack_data.size) &&
                CUR_NODE_TYPE == kOperator &&
               (CUR_NODE_DATA == kMult || CUR_NODE_DATA == kDiv ))
        {
            TreeNode *op = CUR_NODE;

            ++*iter;

            TreeNode *node_rhs = GetP(vars, stk, iter);

            op->left  = node_lhs;
            op->right = node_rhs;

            node_lhs = op;
        }
    }

    return node_lhs;
}

//==============================================================================

TreeNode *GetA(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kOperator)
    {
        if (IsUnaryOp(CUR_NODE_DATA))
        {
            TreeNode *un_func = CUR_NODE;

            ++*iter;

            un_func->left  = nullptr;
            un_func->right = GetP(vars, stk, iter);

            return un_func;
        }
    }
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
DPR
    TreeNode *node_lhs = GetT(vars, stk, iter);

    if (*iter < stk->stack_data.size)
    {
        while ( (*iter < stk->stack_data.size)  &&
                CUR_NODE_TYPE == kOperator      &&
                IsBinaryOpLower(CUR_NODE_DATA)     )
        {
            TreeNode *op = CUR_NODE;

            if (op->data.key_word_code == kAssign)
            {
                if (node_lhs->type != kIdentificator)
                {
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

    return node_lhs;
}

//==============================================================================

TreeNode* GetN(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kConstNumber)
    {
        TreeNode *node = CUR_NODE;

        ++*iter;

        return node;
    }
}

//==============================================================================

static const int kMaxIdLen = 64;

TreeNode *GetId(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kIdentificator)
    {
        TreeNode *identificator = CUR_NODE;
DPR
        ++*iter;

        if (CUR_NODE_TYPE == kOperator &&
            CUR_NODE_DATA == kLeftBracket)
        {
            if (vars->var_array[identificator->data.variable_pos].declaration_state == true &&
                vars->var_array[identificator->data.variable_pos].id_type           == kFunc)
            {
            DPR
                TreeNode *call = NodeCtor(nullptr, nullptr, nullptr, kCall, 0);
            DPR
//params
                call->left = nullptr;
            DPR
                call->right = identificator;
                *iter += 2;

                return call;
            }
            else
            {
                printf(">>MISSING FUNC IDENTIFICATOR: \"%s\"\n",
                       vars->var_array[identificator->data.variable_pos].id);

                return nullptr;
            }
        }

        return identificator;
    }
    else if (CUR_NODE_TYPE == kOperator &&
             IsFunc(CUR_NODE_DATA))
    {
    DPR
        TreeNode *func = CUR_NODE;
        ++(*iter);
        func->right = GetP(vars, stk, iter);

        return func;
    }

    return nullptr;
}

//==============================================================================

TreeNode *GetCode(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *node = nullptr;

    if (*iter <stk->stack_data.size)
    {
        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftZoneBracket)
        {
            ++*iter;

            node = GetL(vars, stk, iter);

            if (CUR_NODE_TYPE != kOperator && CUR_NODE_DATA != kRightZoneBracket)
            {
                return nullptr;
            }

            ++*iter;

            return node;
        }
        else
        {
            return nullptr;
        }
    }

    return node;
}

//==============================================================================

static bool IsUnaryOp(KeyCode_t op_code)
{
    return op_code == kSin   ||
           op_code == kCos   ||
           op_code == kFloor ||
           op_code == kDiff  ||
           op_code == kSqrt;
}

//==============================================================================

static bool IsBinaryOpLower(KeyCode_t op_code)
{
    return op_code == kAdd         ||
           op_code == kSub         ||
           op_code == kMore        ||
           op_code == kLess        ||
           op_code == kOr          ||
           op_code == kMoreOrEqual ||
           op_code == kLessOrEqual ||
           op_code == kNotEqual    ||
           op_code == kAssign      ||
           op_code == kEqual;
}

//==============================================================================

static bool IsType(KeyCode_t op_code)
{
    return op_code == kDoubleType;
}

static bool IsFunc(KeyCode_t op_code)
{
    return op_code == kSin   ||
           op_code == kCos   ||
           op_code == kFloor ||
           op_code == kDiff  ||
           op_code == kSqrt  ||
           op_code == kPrint ||
           op_code == kScan;
}

//==============================================================================

static TreeNode *GetDeclaration(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *decl       = nullptr;
    TreeNode *type       = CUR_NODE;

    ++(*iter);


    if (CUR_NODE_TYPE == kIdentificator)
    {
        TreeNode *identificator = CUR_NODE;

        ++(*iter);

        if (CUR_NODE_TYPE == kOperator &&
            CUR_NODE_DATA == kLeftBracket)
        {
            decl = NodeCtor(nullptr, type, nullptr, kFuncDef, identificator->data.variable_pos);
            ++(*iter);

            TreeNode *params = NodeCtor(nullptr, nullptr, nullptr, kParamsNode, 0);

            params->left = GetParams(vars, stk, iter);

            ++(*iter);

            params->right = GetCode(vars, stk, iter);

            decl->right = params;

            vars->var_array[identificator->data.variable_pos].declaration_state = true;
            vars->var_array[identificator->data.variable_pos].id_type           = kFunc;

            return decl;
        }
        else
        {
            decl = NodeCtor(nullptr,
                            type,
                            nullptr,
                            kVarDecl,
                            identificator->data.variable_pos);

            TreeNode *assign = nullptr;

            if (CUR_NODE_TYPE == kOperator &&
                CUR_NODE_DATA == kAssign)
            {
                assign = CUR_NODE;

                ++(*iter);

                assign->left  = GetE(vars, stk, iter);
                assign->right = NodeCtor(nullptr,
                                         nullptr,
                                         nullptr,
                                         kIdentificator,
                                         identificator->data.variable_pos);
            }
            else
            {
                assign = NodeCtor(nullptr,
                         nullptr,
                         nullptr,
                         kIdentificator,
                         identificator->data.variable_pos);
            }

            decl->right = assign;

            vars->var_array[identificator->data.variable_pos].declaration_state = true;
            vars->var_array[identificator->data.variable_pos].id_type           = kVar;
        }
    }

    DPR

    return decl;
}

//==============================================================================

static TreeNode *GetConditionalOp(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *cond_node = CUR_NODE;

    ++*iter;

    cond_node->left  = GetP(vars, stk, iter);

    cond_node->right = GetCode(vars, stk, iter);

    return cond_node;
}

//==============================================================================

static TreeNode *GetParams(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kOperator &&
        CUR_NODE_DATA == kRightBracket)
    {
        return nullptr;
    }

    TreeNode *param = CUR_NODE;

    ++(*iter);

    if (CUR_NODE_TYPE == kOperator &&
        CUR_NODE_DATA == kEnumOp)
    {
        TreeNode *next_param = CUR_NODE;

        next_param->left = param;

        ++(*iter);

        next_param->right = GetParams(vars, stk, iter);

        return next_param;
    }

    return param;
}

//==============================================================================
