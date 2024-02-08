#include <stdio.h>
#include <stdlib.h>

#include "../debug/color_print.h"
#include "../debug/debug.h"
#include "../TextParse/text_parse.h"
#include "trees.h"
#include "tree_dump.h"
#include "NameTable.h"
//#include "diff.h"

static const char *kTreeSaveFileName = "tree_save.txt";

static const int kPoisonVal = 0xBADBABA;

static FILE *dump_file = nullptr;

static const char *dmp_name = "suka_dump.txt";

static TreeNode* CreateNodeFromText(Identificators     *vars,
                                    Text          *text,
                                    size_t        *iterator);

static TreeNode *CreateNodeFromBrackets(Identificators     *vars,
                                        Text          *text,
                                        size_t        *iterator);

static TreeErrs_t PrintTree(const TreeNode *root,
                            Identificators *vars,
                            FILE           *output_file);

static TreeErrs_t ReallocVarArray(Identificators *vars,
                                  size_t          new_size);

static int ReadNameTablesOutOfFile(LanguageElems *l_elems,
                                   const char    *tables_file_name);

static int ReallocNameTables(NameTables *tables, size_t new_size);

static int ReallocTableOfNames(TableOfNames *table, size_t new_size);

static const size_t kBaseVarCount = 16;

static const size_t kBaseTablesCount = 4;

static const size_t kBaseNamesCount  = 8;

//==============================================================================

int InitNamesLog()
{
    dump_file = fopen(dmp_name, "w");

    return 0;
}

//==============================================================================

int CloseNamesLog()
{
    fclose(dump_file);

    dump_file = nullptr;

    return 0;
}

//==============================================================================

int DumpNameTables(NameTables *tables,
                   const char *func,
                   int         line)
{
    #define DMP_PRINT(...) fprintf(dump_file, __VA_ARGS__)
    DMP_PRINT("FUNC: %s, LINE: %d\n", func, line);
    DMP_PRINT("TABLE_COUNT : %d, TABLES_CAP %d\n", tables->tables_count,
                                                   tables->capacity);

    for (size_t i = 0; i < tables->tables_count; i++)
    {
        DMP_PRINT("\tTABLE[%d, POINTER %p] name_count - %d, capacity %d\n", i, tables->name_tables[i],
                                                                               tables->name_tables[i]->name_count,
                                                                               tables->name_tables[i]->capacity);

        DMP_PRINT("\t{\n");
        for (size_t j = 0; j < tables->name_tables[i]->capacity; j++)
        {
            DMP_PRINT("\t\t%d %d\n", tables->name_tables[i]->names[j].pos,
                                     tables->name_tables[i]->names[j].type);
        }
        DMP_PRINT("\t}\n\n");
    }
    DMP_PRINT("+--------------------------------------------------------+\n");

    return 0;
}

//==============================================================================

int NameTablesInit(NameTables *tables)
{
    tables->capacity     = kBaseTablesCount;
    tables->tables_count = 0;
    tables->name_tables  = (TableOfNames **) calloc(tables->capacity, sizeof(TableOfNames));

    return 0;
}

//==============================================================================

static int ReallocNameTables(NameTables *tables, size_t new_size)
{
    size_t old_size = tables->tables_count;

    tables->capacity = new_size;

    tables->name_tables = (TableOfNames **) realloc(tables->name_tables, tables->capacity * sizeof(TableOfNames));

    for (size_t i = old_size; i < tables->capacity; i++)
    {
        tables->name_tables[i] = nullptr;
    }

    return 0;
}

//==============================================================================

TableOfNames *AddTableOfNames(NameTables *tables,
                              int         func_code)
{
    if (tables->tables_count >= tables->capacity)
    {
        ReallocNameTables(tables, tables->capacity * 2);
    }

    tables->name_tables[tables->tables_count] = (TableOfNames *) calloc(1, sizeof(TableOfNames));

    tables->name_tables[tables->tables_count]->pos = tables->tables_count;

    TablesOfNamesInit(tables->name_tables[tables->tables_count], func_code);

    tables->tables_count++;

    return tables->name_tables[tables->tables_count - 1];
}

//==============================================================================

static int ReallocTableOfNames(TableOfNames *table, size_t new_size)
{
    size_t old_size = table->name_count;

    table->capacity = new_size;

    table->names = (Name *) realloc(table->names, table->capacity * sizeof(Name));

    if (table->names == nullptr)
    {
        perror("ReallocTableOfNames() САНЯ ТЫ ДАУН!!\n");

        return -1;
    }

    for (size_t i = old_size; i < table->capacity; i++)
    {
        table->names[i].pos  = 228;
        table->names[i].type = kUndefined;
    }

    return 0;
}

//==============================================================================

int TablesOfNamesInit(TableOfNames *table,
                      int           func_code)
{
    table->name_count = 0;
    table->func_code  = func_code;
    table->capacity   = kBaseNamesCount;

    table->names = (Name *) calloc(table->capacity, sizeof(Name));

    for (size_t i = 0; i < table->capacity; i++)
    {
        table->names[i].pos  = 228;
        table->names[i].type = kUndefined;
    }

    return 0;
}

//==============================================================================

int AddName(TableOfNames *table,
            size_t        id,
            IdType_t      type)
{
    if (table->name_count >= table->capacity)
    {
        ReallocTableOfNames(table, table->capacity * 2);
    }

    table->names[table->name_count].pos  = id;
    table->names[table->name_count].type = type;

    table->name_count++;

    return 0;
}

//==============================================================================

int VarArrayInit(Identificators *vars)
{
    vars->var_array = (Identificator *) calloc(kBaseVarCount, sizeof(Identificator));

    if (vars->var_array == nullptr)
    {
        return -1;
    }

    vars->size = kBaseVarCount;

    vars->var_count = 0;

    return 0;
}

//==============================================================================

int SeekVariable(Identificators *vars, const char *var_name)
{
    for (size_t i = 0; i < vars->var_count; i++)
    {
        if (strcmp(var_name, vars->var_array[i].id) == 0)
        {
            return i;
        }
    }

    return -1;
}

//==============================================================================

int AddVar(Identificators *vars, const char *var_name)
{
    if (vars->var_count >= vars->size)
    {
        ReallocVarArray(vars, vars->size * 2);
    }

    vars->var_array[vars->var_count].id                = var_name;
    vars->var_array[vars->var_count].declaration_state = false;
    vars->var_array[vars->var_count].id_type           = kUndefined;
    ++vars->var_count;

    return vars->var_count - 1;
}

//==============================================================================

int VarArrayDtor(Identificators *vars)
{
    free(vars->var_array);

    vars->var_array = nullptr;

    vars->size = vars->var_count = 0;

    return 0;
}

//==============================================================================

static TreeErrs_t ReallocVarArray(Identificators *vars,
                                  size_t     new_size)
{
    vars->size = new_size;
    vars->var_array = (Identificator *) realloc(vars->var_array, vars->size * sizeof(Identificator));

    if (vars->var_array == nullptr)
    {
        perror("ReallocVarArray() failed to realloc Identificator array");

        return kFailedRealloc;
    }

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t TreeCtor(Tree *tree)
{
    CHECK(tree);

    tree->root = nullptr;

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t TreeDtor(TreeNode *root)
{
    if (root != nullptr)
    {
        if (root->left != nullptr)
        {
            TreeDtor(root->left);
        }

        if (root->right != nullptr)
        {
            TreeDtor(root->right);
        }
    }

    free(root);

    return kTreeSuccess;
}

//==============================================================================

TreeNode *NodeCtor(TreeNode         *parent_node,
                   TreeNode         *left,
                   TreeNode         *right,
                   ExpressionType_t  type,
                   double            data)
{
    TreeNode *node = (TreeNode *) calloc(1, sizeof(TreeNode));

    if (node == nullptr)
    {
        return nullptr;
    }

    node->type = type;

    switch (type)
    {
        case kOperator:
        {
            node->data.key_word_code = (KeyCode_t) data;

            break;
        }

        case kIdentificator:
        {
            node->data.variable_pos = data;

            break;
        }

        case kConstNumber:
        {
            node->data.const_val = data;

            break;
        }

        case kVarDecl:
        {
            node->data.variable_pos = data;

            break;
        }

        case kFuncDef:
        {
            node->data.variable_pos = data;

            break;
        }

        case kParamsNode:
        {
            node->data.key_word_code = (KeyCode_t) data;

            break;
        }

        case kCall:
        {
            //do nothing

            break;
        }

        default:
        {
            printf("NodeCtor() unknown type %d\n", type);

            free(node);

            return nullptr;
        }

    }

    node->left  = left;
    node->right = right;
    node->parent = parent_node;

    return node;
}

//==============================================================================

static TreeErrs_t PrintTree(const TreeNode *root,
                            Identificators      *vars,
                            FILE           *output_file)
{
    CHECK(output_file);

    fprintf(output_file, "( ");

    if (root == nullptr)
    {
        fprintf(output_file,"_ ");

        return kTreeSuccess;
    }

    switch (root->type)
    {
        case kOperator:
        {
            fprintf(output_file,
                    "%d %d ",
                    kOperator,
                    root->data.key_word_code);

            break;
        }
        case kConstNumber:
        {
            fprintf(output_file,
                    "%d %lg ",
                    kConstNumber,
                    root->data.const_val);

            break;
        }
        case kIdentificator:
        {
            fprintf(output_file,
                    "%d %d ",
                    kIdentificator,
                    root->data.variable_pos);

            break;
        }
        case kFuncDef:
        {
            fprintf(output_file,
                    "%d %d ",
                    kFuncDef,
                    root->data.variable_pos);

            break;
        }
        case kParamsNode:
        {
            fprintf(output_file,
                    "%d ",
                    kParamsNode);

            break;
        }
        case kVarDecl:
        {
            fprintf(output_file,
                    "%d %d ",
                    kVarDecl,
                    root->data.variable_pos);

            break;
        }
        case kCall:
        {
            fprintf(output_file,
                    "%d ",
                    kCall);

            break;
        }

        default:
        {
            printf(">>PrintTreeInFile() unknown node type\n");

            break;
        }
    }

    if (root->left != nullptr)
    {
        PrintTree(root->left, vars, output_file);
    }
    else
    {
        fprintf(output_file, "_ ");
    }

    if (root->right != nullptr)
    {
        PrintTree(root->right, vars, output_file);
    }
    else
    {
        fprintf(output_file, "_ ");
    }

    fprintf(output_file, ") ");

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t PrintTreeInFile(LanguageElems *l_elems,
                           const char    *file_name)
{
    FILE *output_file = fopen(file_name, "wb");

    if (output_file == nullptr)
    {
        return kFailedToOpenFile;
    }

    int main_id = kPoisonVal;

    for (size_t i = 0; i < l_elems->vars.var_count; i++)
    {
        if (strcmp(l_elems->vars.var_array[i].id, kMainFuncName) == 0)
        {
            main_id = i;

            break;
        }
    }

    if (main_id == kPoisonVal)
    {
        printf("У любого уважающего себя кери в программе должен быть 'аганим'.\n"
               "У тебя его нет, бездарь...");

        fclose(output_file);

        return kMissingMain;
    }

    fprintf(output_file, "%d ", main_id);

    PrintTree(l_elems->syntax_tree.root, &l_elems->vars, output_file);

    fclose(output_file);

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t ReadLanguageElemsOutOfFile(LanguageElems *l_elems,
                                      const char    *tree_file_name,
                                      const char    *tables_file_name)
{
    CHECK(l_elems);
    CHECK(tree_file_name);
    CHECK(tables_file_name);

    Text tree_text = {0};

    if (ReadWordsFromFile(&tree_text, tree_file_name) != kSuccess)
    {
        printf("\nReadTreeOutOfFile() failed to read text from file\n");

        return kFailedToReadText;

        TextDtor(&tree_text);
    }

    size_t iterator = 0;

    l_elems->tables.main_id_pos = atoi(tree_text.lines_ptr[iterator++].str);

    l_elems->syntax_tree.root = CreateNodeFromText(&l_elems->vars, &tree_text, &iterator);

    if (l_elems->syntax_tree.root == nullptr)
    {
        printf("ReatTreeOutOfFile() failed to read tree");

        return kFailedToReadTree;
    }

    ReadNameTablesOutOfFile(l_elems, tables_file_name);

    GRAPH_DUMP_TREE(&l_elems->syntax_tree);

    return kTreeSuccess;
}

//==============================================================================

#define CUR_TOKEN table_tokens.lines_ptr[i].str

#define GO_TO_NEXT_TOKEN i++

//==============================================================================

static int ReadNameTablesOutOfFile(LanguageElems *l_elems,
                                   const char    *tables_file_name)
{
    CHECK(l_elems);
    CHECK(tables_file_name);

    Text table_tokens;

    ReadWordsFromFile(&table_tokens, tables_file_name);

    size_t i = 0;

    l_elems->vars.var_count = atoi(CUR_TOKEN);

    l_elems->vars.var_array = (Identificator *) calloc(l_elems->vars.var_count,
                                                       sizeof(Identificator));
    GO_TO_NEXT_TOKEN;

    for (size_t j = 0; j < l_elems->vars.var_count; j++)
    {
        l_elems->vars.var_array[j].id = strdup(CUR_TOKEN);

        GO_TO_NEXT_TOKEN;
    }

    l_elems->tables.tables_count = atoi(CUR_TOKEN);
    l_elems->tables.name_tables  = (TableOfNames **) calloc(l_elems->tables.tables_count,
                                                            sizeof(TableOfNames *));

    GO_TO_NEXT_TOKEN;

    for (size_t j = 0; j < l_elems->tables.tables_count; j++)
    {
        l_elems->tables.name_tables[j] = (TableOfNames *) calloc(1, sizeof(TableOfNames));

        l_elems->tables.name_tables[j]->name_count = atoi(CUR_TOKEN);

        l_elems->tables.name_tables[j]->capacity   = atoi(CUR_TOKEN);

        l_elems->tables.name_tables[j]->names = (Name *) calloc(l_elems->tables.name_tables[j]->capacity,
                                                                 sizeof(Name));

        GO_TO_NEXT_TOKEN;

        l_elems->tables.name_tables[j]->func_code = atoi(CUR_TOKEN);

        GO_TO_NEXT_TOKEN;

        for (size_t z = 0; z < l_elems->tables.name_tables[j]->capacity; z++)
        {

            l_elems->tables.name_tables[j]->names[z].pos = atoi(CUR_TOKEN);

            GO_TO_NEXT_TOKEN;

            l_elems->tables.name_tables[j]->names[z].type = (IdType_t) atoi(CUR_TOKEN);

            GO_TO_NEXT_TOKEN;
        }
    }

    return 0;
}

//==============================================================================

#undef CUR_TOKEN

#undef GO_TO_NEXT_TOKEN

//==============================================================================

#define CUR_TOKEN text->lines_ptr[*iterator].str

#define GO_TO_NEXT_TOKEN ++(*iterator)

//==============================================================================

static TreeNode* CreateNodeFromText(Identificators *vars,
                                    Text           *text,
                                    size_t         *iterator)
{
    CHECK(iterator);
    CHECK(text);
    TreeNode *node = nullptr;

    if (*CUR_TOKEN == '(')
    {
        GO_TO_NEXT_TOKEN;

        ExpressionType_t type = (ExpressionType_t) atoi(text->lines_ptr[*iterator].str);

        switch (type)
        {
            case kConstNumber:
            {
                GO_TO_NEXT_TOKEN;

                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kConstNumber,
                                strtod(CUR_TOKEN, nullptr));
                break;
            }

            case kOperator:
            {
                GO_TO_NEXT_TOKEN;

                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kOperator,
                                atoi(CUR_TOKEN));
                break;
            }

            case kIdentificator:
            {
                GO_TO_NEXT_TOKEN;

                int pos = atoi(CUR_TOKEN);

                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kIdentificator,
                                pos);

                break;
            }
            case kFuncDef:
            {
                GO_TO_NEXT_TOKEN;

                int pos = atoi(CUR_TOKEN);

                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kFuncDef,
                                pos);

                break;
            }

            case kParamsNode:
            {
                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kParamsNode,
                                0);
                break;
            }

            case kVarDecl:
            {
                GO_TO_NEXT_TOKEN;

                int pos = atoi(CUR_TOKEN);

                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kVarDecl,
                                pos);

                break;
            }

            case kCall:
            {
                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kCall,
                                0);

                break;
            }
            default:
            {
                printf("CreateNodeFromText() -> KAVO? 1000-7 ???");

                break;
            }
        }
    }

    GO_TO_NEXT_TOKEN;

    node->left =  CreateNodeFromBrackets(vars, text, iterator);

    node->right = CreateNodeFromBrackets(vars, text, iterator);

    if (*CUR_TOKEN != ')')
    {
        TreeDtor(node);

        return nullptr;;
    }

    return node;
}

//==============================================================================

static TreeNode *CreateNodeFromBrackets(Identificators *vars,
                                        Text           *text,
                                        size_t         *iterator)
{
    TreeNode *node = nullptr;

    if (*CUR_TOKEN == '(')
    {
        node = CreateNodeFromText(vars, text, iterator);

        GO_TO_NEXT_TOKEN;

        if (node == nullptr)
        {
            return nullptr;
        }
    }
    else if (*CUR_TOKEN != ')')
    {
        if (*CUR_TOKEN == '_')
        {
            GO_TO_NEXT_TOKEN;

            return nullptr;
        }
    }

    return node;
}
//==============================================================================

#undef CUR_TOKEN
#undef GO_TO_NEXT_TOKEN

//==============================================================================

TreeNode *CopyNode(const TreeNode *src_node,
                         TreeNode *parent_node)
{
    if (src_node == nullptr)
    {
        return nullptr;
    }

    TreeNode *node = (TreeNode *) calloc(1, sizeof(TreeNode));

    node->data = src_node->data;

    node->type = src_node->type;

    node->left   = CopyNode(src_node->left, node);
    node->right  = CopyNode(src_node->right, node);
    node->parent = parent_node;

    return node;
}

//==============================================================================

TreeErrs_t SetParents(TreeNode *parent_node)
{
    if (parent_node == nullptr)
    {
        return kTreeSuccess;
    }

    if (parent_node->left != nullptr)
    {
        parent_node->left->parent = parent_node;

        SetParents(parent_node->left);
    }
    if (parent_node->right != nullptr)
    {
        parent_node->right->parent = parent_node;

        SetParents(parent_node->right);
    }

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t LanguageElemsInit(LanguageElems *l_elems)
{
    TreeCtor(&l_elems->syntax_tree);

    VarArrayInit(&l_elems->vars);

    NameTablesInit(&l_elems->tables);

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t LanguageElemsDtor(LanguageElems *l_elems)
{
    TreeDtor(l_elems->syntax_tree.root);
    VarArrayDtor(&l_elems->vars);

    return kTreeSuccess;
}

//==============================================================================
