#include <stdio.h>
#include <ctype.h>

#include "lexer.h"
#include "parse.h"
#include "../Common/trees.h"
#include "../Stack/stack.h"

static const char *output_file_name = "TOP_G_DUMP.txt";

#define LOG_PRINT(...) fprintf(output_file, __VA_ARGS__);

static bool IsUnaryOp      (KeyCode_t keyword_code);
static bool IsBinaryOpLower(KeyCode_t keyword_code);
static bool IsType         (KeyCode_t keyword_code);
static bool IsFunc         (KeyCode_t keyword_code);
static bool IsCycleKeyWord (KeyCode_t keyword_code);


static void SkipSpaces(Expr *expr);

static TreeNode *GetDeclaration      (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetConditionalOp    (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetParams           (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetDeclarationList  (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetIdTree           (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetExternalDecl     (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetInstructionList  (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetInstruction      (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetAssignment       (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetCondition        (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetChoiceInstruction(Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetCycleInstruction (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetParams           (Variables *vars, Stack *stk, size_t *iter);
static TreeNode *GetFuncCall         (Variables *vars, Stack *stk, size_t *iter);

static TreeNode *GetFuncDeclaration(TreeNode  *identificator,
                                    TreeNode  *type,
                                    Variables *vars,
                                    Stack     *stk,
                                    size_t    *iter);

TreeNode *GetSyntaxTree(Variables  *vars,
                        const char *file_name)
{
    FILE *output_file = fopen(output_file_name, "w");

    LOG_PRINT("im GetSyntaxTree leading all work\n\n");

    Text              program;
    ReadTextFromFile(&program, file_name);

    fclose(output_file);

    Stack      lexems;
    StackInit(&lexems);

    SplitOnLexems(&program, &lexems, vars);

    size_t i = 0;

    TreeNode *node = GetExternalDecl(vars, &lexems, &i);;

    SetParents(node);

    LOG_PRINT("GetSyntaxTree finished work\n");

    return node;
}

//==============================================================================
    #define GO_TO_NEXT_TOKEN ++*iter

    #define OP_CTOR(op_code) NodeCtor(nullptr, nullptr, nullptr, kOperator, op_code)

    #define CALL_CTOR(identificator_node, params_node) \
        NodeCtor(nullptr, identificator_node, params_node, kCall, 0)

    #define CUR_NODE       stk->stack_data.data[*iter]
    #define CUR_NODE_TYPE  stk->stack_data.data[*iter]->type
    #define CUR_NODE_DATA  stk->stack_data.data[*iter]->data.key_word_code
    #define CUR_LINE       stk->stack_data.data[*iter]->line_number

    #define CUR_ID_STATE   vars->var_array[stk->stack_data.data[*iter]->data.variable_pos].declaration_state
    #define CUR_ID_TYPE    vars->var_array[stk->stack_data.data[*iter]->data.variable_pos].id_type
    #define CUR_ID         vars->var_array[stk->stack_data.data[*iter]->data.variable_pos].id

    #define NEXT_NODE      stk->stack_data.data[*iter + 1]
    #define NEXT_NODE_TYPE stk->stack_data.data[*iter + 1]->type
    #define NEXT_NODE_DATA stk->stack_data.data[*iter + 1]->data.key_word_code

    #ifdef DEBUG
        #define DPR printf("POS %d, LINE %d, FUNC %s\n", *iter, __LINE__, __func__);
    #else
        #define DPR
    #endif

    #define SYNTAX_OP_ASSERT(key_code)                                                                                  \
        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA != key_code)                                                    \
        {                                                                                                               \
            printf(">>LINE %d:\n"                                                                                       \
                   "  Ѕл€€€€.... “ут должно быть '%s'\n", CUR_LINE, NameTable[GetNameTablePos(key_code)].key_word);     \
                                                                                                                        \
            return nullptr;                                                                                             \
        }

//==============================================================================

static TreeNode *GetExternalDecl(Variables *vars,
                                 Stack     *stk,
                                 size_t    *iter)
{
    TreeNode *cur_unit = OP_CTOR(kEndOfLine);

    cur_unit->left = GetDeclaration(vars, stk, iter);

DPR
    if (*iter < stk->stack_data.size - 1)
    {
        cur_unit->right = OP_CTOR(kEndOfLine);

        cur_unit->right->left = GetDeclaration(vars, stk, iter);
    }

    return cur_unit;
}

//==============================================================================

static TreeNode *GetDeclaration(Variables *vars, Stack *stk, size_t *iter)
{
    if (!IsType(CUR_NODE_DATA))
    {
        return nullptr;
    }

    TreeNode *type = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    if (CUR_NODE_TYPE != kIdentificator)
    {
        //mistake

        return nullptr;
    }

    TreeNode *identificator = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftBracket)
    {
        return GetFuncDeclaration(identificator, type, vars, stk, iter);
    }

    TreeNode *decl = NodeCtor(nullptr,
                              type,
                              nullptr,
                              kVarDecl,
                              identificator->data.variable_pos);

    if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kAssign)
    {
        (*iter)--;

        decl->right = GetAssignment(vars, stk, iter);
    }

    vars->var_array[identificator->data.variable_pos].declaration_state = true;
    vars->var_array[identificator->data.variable_pos].id_type           = kVar;


    DPR

    return decl;
}

//==============================================================================

static TreeNode *GetFuncDeclaration(TreeNode  *identificator,
                                    TreeNode  *type,
                                    Variables *vars,
                                    Stack     *stk,
                                    size_t    *iter)
{

    TreeNode *decl = NodeCtor(nullptr, type, nullptr, kFuncDef, identificator->data.variable_pos);
DPR
    TreeNode *params = NodeCtor(nullptr, nullptr, nullptr, kParamsNode, 0);
DPR
    params->left = GetDeclarationList(vars, stk, iter);
DPR
    params->right = GetInstructionList(vars, stk, iter);
DPR
    decl->right = params;

    vars->var_array[identificator->data.variable_pos].declaration_state = true;
    vars->var_array[identificator->data.variable_pos].id_type           = kFunc;
DPR

    return decl;
}

//==============================================================================

static TreeNode *GetDeclarationList(Variables *vars, Stack *stk, size_t *iter)
{
    SYNTAX_OP_ASSERT(kLeftBracket);

    TreeNode *decl_list = OP_CTOR(kEnumOp);
DPR
    GO_TO_NEXT_TOKEN;
DPR
    if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kRightBracket)
    {
        GO_TO_NEXT_TOKEN;

        return decl_list;
    }

    TreeNode *decl = decl_list;

    decl->left = GetDeclaration(vars, stk, iter);
DPR
    while (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kEnumOp)
    {
        decl->right = CUR_NODE;

        decl = decl->right;

        GO_TO_NEXT_TOKEN;

        decl->left = GetDeclaration(vars, stk, iter);
    }

    SYNTAX_OP_ASSERT(kRightBracket);

    GO_TO_NEXT_TOKEN;

    return decl_list;
}

//==============================================================================

static TreeNode *GetIdTree(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *id_tree = nullptr;

    if (CUR_ID_TYPE == kVar)
    {
        if (NEXT_NODE_TYPE == kOperator && NEXT_NODE_DATA == kAssign)
        {
            id_tree        = NEXT_NODE;
            id_tree->right = CUR_NODE;

            *iter += 2;

            id_tree->left = GetAddExpression(vars, stk, iter);
        }
    }
    else if (CUR_ID_TYPE == kFunc)
    {
        id_tree = GetIdentificator(vars, stk, iter);
    }
    else
    {
        printf(">> UNDEFINED IDENTIFICATOR - \"%s\", POS: %d\n", CUR_ID, *iter);
    }

    return id_tree;
}

//==============================================================================

TreeNode *GetExpression(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *node = nullptr;

    if (*iter <stk->stack_data.size)
    {
        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftBracket)
        {
            GO_TO_NEXT_TOKEN;

            node = GetAddExpression(vars, stk, iter);

            SYNTAX_OP_ASSERT(kRightBracket);

            GO_TO_NEXT_TOKEN;

            return node;
        }
        else
        {
            return GetPrimaryExpression(vars, stk, iter);
        }
    }

    return node;
}

//==============================================================================

TreeNode *GetMultExpression(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode* node_lhs = GetExpression(vars, stk, iter);

    if (*iter >= stk->stack_data.size)
    {
        return node_lhs;
    }

    while ((*iter < stk->stack_data.size) &&
            CUR_NODE_TYPE == kOperator    &&
           (CUR_NODE_DATA == kMult        ||
            CUR_NODE_DATA == kDiv ))
    {
        TreeNode *op = CUR_NODE;

        GO_TO_NEXT_TOKEN;

        TreeNode *node_rhs = GetExpression(vars, stk, iter);

        op->left  = node_lhs;
        op->right = node_rhs;

        node_lhs = op;
    }


    return node_lhs;
}

//==============================================================================

TreeNode *GetPrimaryExpression(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kOperator)
    {
        if (IsUnaryOp(CUR_NODE_DATA))
        {
            TreeNode *un_func = CUR_NODE;

            GO_TO_NEXT_TOKEN;

            un_func->left  = nullptr;

            un_func->right = GetExpression(vars, stk, iter);

            return un_func;
        }
    }
    if (CUR_NODE_TYPE == kConstNumber)
    {
        return GetConstant(vars, stk, iter);
    }
    else
    {
        return GetIdentificator(vars, stk, iter);
    }
}

//==============================================================================

TreeNode *GetAddExpression(Variables *vars, Stack *stk, size_t *iter)
{
DPR
    TreeNode *node_lhs = GetMultExpression(vars, stk, iter);
DPR

    if (*iter > stk->stack_data.size)
    {
        return node_lhs;
    }

DPR
    while ((*iter < stk->stack_data.size)   &&
            CUR_NODE_TYPE == kOperator      &&
            IsBinaryOpLower(CUR_NODE_DATA))
    {
DPR
        TreeNode *op = CUR_NODE;
        printf("dbg %p\n", op);
DPR
        if (op->data.key_word_code == kAssign)
        {
            if (node_lhs->type != kIdentificator)
            {
                return nullptr;
            }
        }

        GO_TO_NEXT_TOKEN;
DPR

        TreeNode *node_rhs = GetMultExpression(vars, stk, iter);
DPR

        op->left  = node_lhs;
DPR
        op->right = node_rhs;
DPR

        node_lhs = op;
DPR

    }
DPR

    return node_lhs;
}

//==============================================================================

TreeNode* GetConstant(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kConstNumber)
    {
        TreeNode *node = CUR_NODE;

        GO_TO_NEXT_TOKEN;

        return node;
    }
}

//==============================================================================

static const int kMaxIdLen = 64;

TreeNode *GetIdentificator(Variables *vars, Stack *stk, size_t *iter)
{
    if (CUR_NODE_TYPE == kIdentificator)
    {
        TreeNode *identificator_node = CUR_NODE;
DPR
        GO_TO_NEXT_TOKEN;

        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftBracket)
        {
            --(*iter);

            return GetFuncCall(vars, stk, iter);
        }

        return identificator_node;
    }
    else if (CUR_NODE_TYPE == kOperator && IsFunc(CUR_NODE_DATA))
    {
    DPR
        TreeNode *func = CUR_NODE;

        GO_TO_NEXT_TOKEN;

        func->right = GetExpression(vars, stk, iter);

        return func;
    }

    return nullptr;
}

//==============================================================================

static TreeNode *GetFuncCall(Variables *vars,
                             Stack     *stk,
                             size_t    *iter)
{
    TreeNode *identificator_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    return CALL_CTOR(identificator_node, GetParams(vars, stk, iter));
}

//==============================================================================

static TreeNode *GetParams(Variables *vars, Stack *stk, size_t *iter)
{
    SYNTAX_OP_ASSERT(kLeftBracket);

    TreeNode *params_node = OP_CTOR(kEnumOp);

    GO_TO_NEXT_TOKEN;

    if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kRightBracket)
    {
        GO_TO_NEXT_TOKEN;

        return params_node;
    }

    TreeNode *param = params_node;

    param->left = GetAddExpression(vars, stk, iter);
DPR

    while (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kEnumOp)
    {

        param->right = CUR_NODE;
DPR
        param = param->right;

        GO_TO_NEXT_TOKEN;
DPR
        param->left  = GetAddExpression(vars, stk, iter);


    }
DPR
    SYNTAX_OP_ASSERT(kRightBracket);

    GO_TO_NEXT_TOKEN;

    return params_node;
}

//==============================================================================

static TreeNode *GetInstructionList(Variables *vars, Stack *stk, size_t *iter)
{
    SYNTAX_OP_ASSERT(kLeftZoneBracket);
DPR

    GO_TO_NEXT_TOKEN;

    TreeNode *cur_instruction = OP_CTOR(kEndOfLine);
DPR

    cur_instruction->left = GetInstruction(vars, stk, iter);

    TreeNode *instructions = cur_instruction;

    while (CUR_NODE_DATA != kRightZoneBracket)
    {
        cur_instruction->right = OP_CTOR(kEndOfLine);

        cur_instruction = cur_instruction->right;
DPR

        cur_instruction->left = GetInstruction(vars, stk, iter);
DPR
    }
DPR
    GO_TO_NEXT_TOKEN;

    return instructions;
}

//==============================================================================

static TreeNode *GetInstruction(Variables *vars, Stack *stk, size_t *iter)
{
        DPR
    if (CUR_NODE_TYPE == kIdentificator)
    {
DPR     if (NEXT_NODE_TYPE == kOperator && NEXT_NODE_DATA == kAssign)
        {
            return GetAssignment(vars, stk, iter);
        }
        else if (NEXT_NODE_TYPE == kOperator && NEXT_NODE_DATA == kLeftBracket)
        {
DPR
            TreeNode *call_tree = GetFuncCall(vars, stk, iter);

            GO_TO_NEXT_TOKEN;

            return call_tree;
        }
    }
    else if (CUR_NODE_TYPE == kOperator && IsFunc(CUR_NODE_DATA))
    {
        TreeNode *standart_func = GetIdentificator(vars, stk, iter);

        GO_TO_NEXT_TOKEN;

        return standart_func;
    }
    else if (CUR_NODE_TYPE == kOperator && IsType(CUR_NODE_DATA))
    {
        return GetDeclaration(vars, stk, iter);
    }
    else if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kIf)//not ony if
    {
        return GetChoiceInstruction(vars, stk, iter);
    }
    else if (CUR_NODE_TYPE == kOperator && IsCycleKeyWord(CUR_NODE_DATA))
    {
        return GetCycleInstruction(vars, stk, iter);
    }
DPR

    return nullptr;
}

//==============================================================================

static TreeNode *GetAssignment(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *identificator = CUR_NODE; //change on lvalue

    GO_TO_NEXT_TOKEN;

    TreeNode *assign_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    assign_node->right = identificator;
    assign_node->left  = GetAddExpression(vars, stk, iter);

    SYNTAX_OP_ASSERT(kEndOfLine);
DPR

    GO_TO_NEXT_TOKEN;
DPR

    return assign_node;
}

//==============================================================================

static TreeNode *GetChoiceInstruction(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *choice_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    choice_node->left  = GetCondition      (vars, stk, iter);
    choice_node->right = GetInstructionList(vars, stk, iter);

    return choice_node;
}

//==============================================================================

static TreeNode *GetCondition(Variables *vars, Stack *stk, size_t *iter)
{
    SYNTAX_OP_ASSERT(kLeftBracket);

    GO_TO_NEXT_TOKEN;

    TreeNode *condition_node = GetAddExpression(vars, stk, iter);

    SYNTAX_OP_ASSERT(kRightBracket);

    GO_TO_NEXT_TOKEN;

    return condition_node;
}

//==============================================================================

static TreeNode *GetCycleInstruction(Variables *vars, Stack *stk, size_t *iter)
{
    TreeNode *cycle_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    cycle_node->left  = GetCondition      (vars, stk, iter);
    cycle_node->right = GetInstructionList(vars, stk, iter);

    return cycle_node;
}

//==============================================================================

static bool IsUnaryOp(KeyCode_t keyword_code)
{
    return keyword_code == kSin   ||
           keyword_code == kCos   ||
           keyword_code == kFloor ||
           keyword_code == kDiff  ||
           keyword_code == kSqrt;
}

//==============================================================================

static bool IsBinaryOpLower(KeyCode_t keyword_code)
{
    return keyword_code == kAdd         ||
           keyword_code == kSub         ||
           keyword_code == kMore        ||
           keyword_code == kLess        ||
           keyword_code == kOr          ||
           keyword_code == kMoreOrEqual ||
           keyword_code == kLessOrEqual ||
           keyword_code == kNotEqual    ||
           keyword_code == kAssign      ||
           keyword_code == kEqual;
}

//==============================================================================

static bool IsCycleKeyWord(KeyCode_t keyword_code)
{
    return keyword_code == kWhile;
}

//==============================================================================

static bool IsType(KeyCode_t keyword_code)
{
    return keyword_code == kDoubleType;
}

//==============================================================================

static bool IsFunc(KeyCode_t keyword_code)
{
    return keyword_code == kSin   ||
           keyword_code == kCos   ||
           keyword_code == kFloor ||
           keyword_code == kDiff  ||
           keyword_code == kSqrt  ||
           keyword_code == kPrint ||
           keyword_code == kScan  ||
           keyword_code == kReturn;
}

//==============================================================================


static TreeNode *GetConditionalOp(Variables *vars, Stack *stk, size_t *iter)
{

    DPR
    TreeNode *cond_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    cond_node->left  = GetExpression(vars, stk, iter);

    cond_node->right = GetInstructionList(vars, stk, iter);

    return cond_node;
}

//==============================================================================
