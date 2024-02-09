#include <stdio.h>
#include <ctype.h>

#include "lexer.h"
#include "parse.h"
#include "../Common/trees.h"
#include "../Stack/stack.h"
#include "../debug/debug.h"

static const int kExternalTableCode = -1;

static const char *id_table_file_name = "id_table.txt";

#define LOG_PRINT(...) fprintf(output_file, __VA_ARGS__);

static bool IsUnaryOp      (KeyCode_t keyword_code);
static bool IsBinaryOpLower(KeyCode_t keyword_code);
static bool IsType         (KeyCode_t keyword_code);
static bool IsFunc         (KeyCode_t keyword_code);
static bool IsCycleKeyWord (KeyCode_t keyword_code);

TreeNode *GetDiff(Identificators *vars,
                  Stack          *stk,
                  size_t         *iter);

static TreeNode *GetDeclaration(Identificators   *vars,
                                Stack            *stk,
                                NameTables       *tables,
                                TableOfNames     *cur_table,
                                size_t           *iter);

static TreeNode *GetConditionalOp(Identificators *vars,
                                  Stack          *stk,
                                  NameTables     *tables,
                                  TableOfNames   *cur_table,
                                  size_t         *iter);

static TreeNode *GetDeclarationList(Identificators *vars,
                                    Stack          *stk,
                                    NameTables     *tables,
                                    TableOfNames   *cur_table,
                                    size_t         *iter);

static TreeNode *GetIdTree(Identificators *vars, Stack *stk, size_t *iter);

static TreeNode *GetInstructionList  (Identificators *vars,
                                      Stack          *stk,
                                      NameTables     *tables,
                                      TableOfNames   *cur_table,
                                      size_t         *iter);

static TreeNode *GetInstruction(Identificators *vars,
                                Stack          *stk,
                                NameTables     *tables,
                                TableOfNames   *cur_table,
                                size_t         *iter);

static TreeNode *GetAssignment(Identificators *vars, Stack *stk, size_t *iter);
static TreeNode *GetCondition (Identificators *vars, Stack *stk, size_t *iter);

static TreeNode *GetChoiceInstruction(Identificators *vars,
                                      Stack          *stk,
                                      NameTables     *tables,
                                      TableOfNames   *cur_table,
                                      size_t         *iter);

static TreeNode *GetCycleInstruction(Identificators *vars,
                                     Stack          *stk,
                                     NameTables     *tables,
                                     TableOfNames   *cur_table,
                                     size_t         *iter);

static TreeNode *GetParams  (Identificators *vars, Stack *stk, size_t *iter);
static TreeNode *GetFuncCall(Identificators *vars, Stack *stk, size_t *iter);

static TreeNode *GetExternalDecl(Identificators *vars,
                                 Stack          *stk,
                                 NameTables     *tables,
                                 TableOfNames   *cur_table,
                                 size_t         *iter);

static TreeNode *GetFuncDeclaration(TreeNode       *identificator,
                                    TreeNode       *type,
                                    NameTables     *tables,
                                    TableOfNames   *cur_table,
                                    Identificators *vars,
                                    Stack          *stk,
                                    size_t         *iter);
//type
static int PrintNameTableInFile(Identificators *idents,
                                NameTables     *tables);

//==============================================================================

TreeNode *GetSyntaxTree(Identificators  *vars,
                        const char *file_name)
{
    CHECK(vars);
    CHECK(file_name);

    Text              program;
    ReadTextFromFile(&program, file_name);

    Stack      lexems;
    StackInit(&lexems);
    printf("HUY");
    SplitOnLexems(&program, &lexems, vars);

    size_t i = 0;

    NameTables tables;
    NameTablesInit(&tables);

    TABLES_DUMP(&tables);

    TableOfNames *external_table = AddTableOfNames(&tables, kExternalTableCode);

    TreeNode *node = GetExternalDecl(vars, &lexems, &tables, external_table, &i);
    TABLES_DUMP(&tables);

    if (PrintNameTableInFile(vars ,&tables) < 0)
    {
        return nullptr;
    }

    SetParents(node);

    return node;
}

//==============================================================================
    #undef DEBUG

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

static int PrintNameTableInFile(Identificators *idents,
                                NameTables     *tables)
{
    CHECK(idents);
    CHECK(tables);

    FILE *id_file = fopen(id_table_file_name, "w");

    if (id_file == nullptr)
    {
        perror("GetSyntaxTree() failed to open id_table file");

        return -1;
    }

    fprintf(id_file, "%d\n", idents->var_count);

    for (size_t i = 0; i < idents->var_count; i++)
    {
        fprintf(id_file, "%s\n", idents->var_array[i].id);
    }

    fprintf(id_file, "%d\n", tables->tables_count);

    fprintf(id_file, "\n\n");

    for (size_t i = 0; i < tables->tables_count; i++)
    {
        fprintf(id_file, "%d %d\n", tables->name_tables[i]->name_count,
                                    tables->name_tables[i]->func_code);

        for (size_t j = 0; j < tables->name_tables[i]->name_count; j++)
        {
            fprintf(id_file, "%d %d\n", tables->name_tables[i]->names[j].pos,
                                        tables->name_tables[i]->names[j].type);
        }

        fprintf(id_file, "\n");
    }

    fclose(id_file);

    if (errno != 0)
    {
        printf(">>PrintNameTableInFile()");
    }

    return 0;
}

//==============================================================================

static TreeNode *GetExternalDecl(Identificators *vars,
                                 Stack          *stk,
                                 NameTables     *tables,
                                 TableOfNames   *cur_table,
                                 size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(iter);

    TreeNode *ext_decl_unit = OP_CTOR(kEndOfLine);
    TreeNode *cur_unit      = ext_decl_unit;

    cur_unit->left = GetDeclaration(vars, stk, tables, cur_table, iter);

    if (cur_unit->left == nullptr)
    {
        return nullptr;
    }

    if(cur_unit->left->type == kFuncDef)
    {
        AddName(cur_table, cur_unit->left->data.variable_pos, kFunc);
    }
DPR
    while (*iter < stk->stack_data.size - 1)
    {
DPR
        cur_unit->right = OP_CTOR(kEndOfLine);
DPR
        cur_unit = cur_unit->right;
DPR
        cur_unit->left = GetDeclaration(vars, stk, tables, cur_table, iter);
DPR
        if (cur_unit->left == nullptr)
        {
            return nullptr;
        }

        if (cur_unit->left->type == kFuncDef)
        {
            AddName(cur_table, cur_unit->left->data.variable_pos, kFunc);
        }
DPR

    }

    return ext_decl_unit;
}

//==============================================================================

static TreeNode *GetDeclaration(Identificators *vars,
                                Stack          *stk,
                                NameTables     *tables,
                                TableOfNames   *cur_table,
                                size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(iter);

    if (!IsType(CUR_NODE_DATA))
    {
        return nullptr;
    }

    TreeNode *type = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    if (CUR_NODE_TYPE != kIdentificator)
    {
        printf(">>LINE %d:\n"
               "  я думал, что тут будет идентификатор.\n", CUR_LINE);

        return nullptr;
    }

    TreeNode *identificator = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kLeftBracket)
    {
        TableOfNames *local_name_table = AddTableOfNames(tables,
                                                         identificator->data.variable_pos);
DPR
        return GetFuncDeclaration(identificator,
                                  type,
                                  tables,
                                  local_name_table,
                                  vars,
                                  stk,
                                  iter);
    }

    TreeNode *decl = NodeCtor(nullptr,
                              type,
                              nullptr,
                              kVarDecl,
                              identificator->data.variable_pos);

    AddName(cur_table, identificator->data.variable_pos, kVar);

    if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kAssign)
    {
        (*iter)--;

        decl->right = GetAssignment(vars, stk, iter);
    }
    else
    {
        decl->right = identificator;
    }

    vars->var_array[identificator->data.variable_pos].declaration_state = true;
    vars->var_array[identificator->data.variable_pos].id_type           = kVar;


    DPR

    return decl;
}

//==============================================================================

static TreeNode *GetFuncDeclaration(TreeNode       *identificator,
                                    TreeNode       *type,
                                    NameTables     *tables,
                                    TableOfNames   *cur_table,
                                    Identificators *vars,
                                    Stack          *stk,
                                    size_t         *iter)
{
    CHECK(identificator);
    CHECK(type);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

    TreeNode *decl = NodeCtor(nullptr, type, nullptr, kFuncDef, identificator->data.variable_pos);
DPR
    TreeNode *params = NodeCtor(nullptr, nullptr, nullptr, kParamsNode, 0);
DPR
    params->left  = GetDeclarationList(vars, stk, tables, cur_table, iter);
DPR
    params->right = GetInstructionList(vars, stk, tables, cur_table, iter);

    if (params->right == nullptr)
    {
        return nullptr;
    }
DPR
    decl->right = params;

    vars->var_array[identificator->data.variable_pos].declaration_state = true;
    vars->var_array[identificator->data.variable_pos].id_type           = kFunc;
DPR

    return decl;
}

//==============================================================================

static TreeNode *GetDeclarationList(Identificators *vars,
                                    Stack          *stk,
                                    NameTables     *tables,
                                    TableOfNames   *cur_table,
                                    size_t         *iter)
{
    CHECK(tables);
    CHECK(cur_table);
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

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

    decl->left = GetDeclaration(vars, stk, tables, cur_table, iter);
DPR
    while (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kEnumOp)
    {
        decl->right = CUR_NODE;

        decl = decl->right;

        GO_TO_NEXT_TOKEN;

        decl->left = GetDeclaration(vars, stk, tables, cur_table, iter);
    }

    SYNTAX_OP_ASSERT(kRightBracket);

    GO_TO_NEXT_TOKEN;

    return decl_list;
}

//==============================================================================

static TreeNode *GetIdTree(Identificators *vars,
                           Stack          *stk,
                           size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

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

TreeNode *GetExpression(Identificators *vars,
                        Stack          *stk,
                        size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

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
        DPR
            return GetPrimaryExpression(vars, stk, iter);
        }
    }

    return node;
}

//==============================================================================

TreeNode *GetMultExpression(Identificators *vars,
                            Stack          *stk,
                            size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

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

TreeNode *GetPrimaryExpression(Identificators *vars,
                               Stack          *stk,
                               size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

    if (CUR_NODE_TYPE == kOperator)
    {

        if (IsFunc(CUR_NODE_DATA))
        {
            if (CUR_NODE_DATA == kDiff)
            {
                return GetDiff(vars, stk, iter);
            }

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

TreeNode *GetDiff(Identificators *vars,
                  Stack          *stk,
                  size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

    TreeNode *diff_tree = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    SYNTAX_OP_ASSERT(kLeftBracket);

    TreeNode *params_node = OP_CTOR(kEnumOp);

    GO_TO_NEXT_TOKEN;

    TreeNode *param = params_node;

    diff_tree->left = GetAddExpression(vars, stk, iter);
DPR
    GO_TO_NEXT_TOKEN;

    diff_tree->right = GetAddExpression(vars, stk, iter);

    SYNTAX_OP_ASSERT(kRightBracket);

    GO_TO_NEXT_TOKEN;

    return diff_tree;
}

//==============================================================================

TreeNode *GetAddExpression(Identificators *vars,
                           Stack          *stk,
                           size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);
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

TreeNode* GetConstant(Identificators *vars,
                      Stack          *stk,
                      size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

    if (CUR_NODE_TYPE != kConstNumber)
    {
        printf(">>LINE : %d\n"
               "  Ќикак же ты бл€ть не научишьс€...—десь должно быть число", CUR_LINE);

        return nullptr;
    }

    TreeNode *node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    return node;
}

//==============================================================================

static const int kMaxIdLen = 64;

TreeNode *GetIdentificator(Identificators *vars,
                           Stack          *stk,
                           size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

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
DPR
        func->right = GetAddExpression(vars, stk, iter);
DPR
        return func;
    }

    return nullptr;
}

//==============================================================================

static TreeNode *GetFuncCall(Identificators *vars,
                             Stack          *stk,
                             size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

    TreeNode *identificator_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    return CALL_CTOR(GetParams(vars, stk, iter), identificator_node);
}

//==============================================================================

static TreeNode *GetParams(Identificators *vars,
                           Stack          *stk,
                           size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

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

static TreeNode *GetInstructionList(Identificators *vars,
                                    Stack          *stk,
                                    NameTables     *tables,
                                    TableOfNames   *cur_table,
                                    size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(iter);

    SYNTAX_OP_ASSERT(kLeftZoneBracket);
DPR

    GO_TO_NEXT_TOKEN;

    TreeNode *cur_instruction = OP_CTOR(kEndOfLine);
DPR

    TABLES_DUMP(tables);
    cur_instruction->left = GetInstruction(vars, stk, tables, cur_table, iter);

    TreeNode *instructions = cur_instruction;

    while (!(CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kRightZoneBracket))
    {
        cur_instruction->right = OP_CTOR(kEndOfLine);

        cur_instruction = cur_instruction->right;
DPR
        cur_instruction->left = GetInstruction(vars, stk, tables, cur_table, iter);
DPR
        if (cur_instruction->left == nullptr)
        {
            return nullptr;
        }
    }
DPR
    GO_TO_NEXT_TOKEN;

    TABLES_DUMP(tables);

    return instructions;
}

//==============================================================================

static TreeNode *GetInstruction(Identificators *vars,
                                Stack          *stk,
                                NameTables     *tables,
                                TableOfNames   *cur_table,
                                size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(iter);

        DPR
    if (CUR_NODE_TYPE == kIdentificator)
    {
DPR
        if (NEXT_NODE_TYPE == kOperator && NEXT_NODE_DATA == kAssign)
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
        DPR

        TreeNode *standart_func = GetIdentificator(vars, stk, iter);
        DPR

        GO_TO_NEXT_TOKEN;

        return standart_func;
    }
    else if (CUR_NODE_TYPE == kOperator && IsType(CUR_NODE_DATA))
    {
        TreeNode *decl_node =  GetDeclaration(vars, stk, tables, cur_table, iter);

        if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kEndOfLine)
        {
            GO_TO_NEXT_TOKEN;
        }

        return decl_node;
    }
    else if (CUR_NODE_TYPE == kOperator && CUR_NODE_DATA == kIf)//not ony if
    {
        return GetChoiceInstruction(vars, stk, tables, cur_table, iter);
    }
    else if (CUR_NODE_TYPE == kOperator && IsCycleKeyWord(CUR_NODE_DATA))
    {
        return GetCycleInstruction(vars, stk, tables, cur_table, iter);
    }
    else
    {
        printf(">>—“–ќ ј: %d\n"
               "  ≈ЅЋјЌ, ќ∆»ƒјЋј—№ »Ќ—“–” ÷»я. ј € увидел\n", CUR_LINE);
    }
DPR

    return nullptr;
}

//==============================================================================

static TreeNode *GetAssignment(Identificators *vars,
                               Stack          *stk,
                               size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);
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

static TreeNode *GetChoiceInstruction(Identificators *vars,
                                      Stack          *stk,
                                      NameTables     *tables,
                                      TableOfNames   *cur_table,
                                      size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(iter);

    TreeNode *choice_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    choice_node->left  = GetCondition(vars, stk, iter);

    choice_node->right = GetInstructionList(vars, stk, tables, cur_table, iter);

    if (choice_node->right == nullptr)
    {
        return nullptr;
    }

    return choice_node;
}

//==============================================================================

static TreeNode *GetCondition(Identificators *vars,
                              Stack          *stk,
                              size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(iter);

    SYNTAX_OP_ASSERT(kLeftBracket);

    GO_TO_NEXT_TOKEN;

    TreeNode *condition_node = GetAddExpression(vars, stk, iter);

    SYNTAX_OP_ASSERT(kRightBracket);

    GO_TO_NEXT_TOKEN;

    return condition_node;
}

//==============================================================================

static TreeNode *GetCycleInstruction(Identificators *vars,
                                     Stack          *stk,
                                     NameTables     *tables,
                                     TableOfNames   *cur_table,
                                     size_t         *iter)
{
    CHECK(vars);
    CHECK(stk);
    CHECK(tables);
    CHECK(cur_table);
    CHECK(iter);

    TreeNode *cycle_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    cycle_node->left  = GetCondition(vars, stk, iter);

    cycle_node->right = GetInstructionList(vars, stk, tables, cur_table, iter);

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
           keyword_code == kReturn||
           keyword_code == kAbort;
}

//==============================================================================


static TreeNode *GetConditionalOp(Identificators *vars,
                                  Stack          *stk,
                                  NameTables     *tables,
                                  TableOfNames   *cur_table,
                                  size_t         *iter)
{

    DPR
    TreeNode *cond_node = CUR_NODE;

    GO_TO_NEXT_TOKEN;

    cond_node->left  = GetExpression(vars, stk, iter);

    cond_node->right = GetInstructionList(vars, stk, tables, cur_table, iter);

    return cond_node;
}

//==============================================================================
    #undef GO_TO_NEXT_TOKEN
    #undef OP_CTOR
    #undef CALL_CTOR
    #undef CUR_NODE
    #undef CUR_NODE_TYPE
    #undef CUR_NODE_DATA
    #undef CUR_LINE
    #undef CUR_ID_STATE
    #undef CUR_ID_TYPE
    #undef CUR_ID
    #undef NEXT_NODE
    #undef NEXT_NODE_TYPE
    #undef NEXT_NODE_DATA
//==============================================================================
