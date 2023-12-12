#include <stdio.h>
#include <stdlib.h>

#include "debug/color_print.h"
#include "debug/debug.h"
#include "TextParse/text_parse.h"
#include "trees.h"
#include "tree_dump.h"
#include "NameTable.h"
//#include "diff.h"

static const char *kTreeSaveFileName = "tree_save.txt";

static const int kPoisonVal = 0xBADBABA;

static const char *kPreCtored = "*";

static TreeNode* CreateNodeFromText(Variables     *vars,
                                    Text          *text,
                                    size_t        *iterator);

static TreeNode *CreateNodeFromBrackets(Variables     *vars,
                                        Text          *text,
                                        size_t        *iterator);

static TreeErrs_t PrintTree(const TreeNode *root,
                            FILE           *output_file);

static TreeErrs_t ReallocVarArray(Variables *vars,
                                  size_t     new_size);


static const size_t kBaseVarCount = 16;

//==============================================================================

int VarArrayInit(Variables *vars)
{
    vars->var_array = (Variable *) calloc(kBaseVarCount, sizeof(Variable));

    if (vars->var_array == nullptr)
    {
        return -1;
    }

    vars->size = kBaseVarCount;

    vars->var_count = 0;

    return 0;
}

//==============================================================================

int SeekVariable(Variables *vars, const char *var_name)
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

int AddVar(Variables *vars, const char *var_name)
{
    if (vars->var_count == vars->size)
    {
        ReallocVarArray(vars, vars->size * 2);
    }

    vars->var_array[vars->var_count].id    = var_name;

    ++vars->var_count;

    return vars->var_count - 1;
}

//==============================================================================

int VarArrayDtor(Variables *vars)
{
    free(vars->var_array);
    vars->var_array = nullptr;

    vars->size = vars->var_count = 0;

    return 0;
}

//==============================================================================

static TreeErrs_t ReallocVarArray(Variables *vars,
                                  size_t     new_size)
{
    vars->size = new_size;
    vars->var_array = (Variable *) realloc(vars->var_array, vars->size);

    if (vars->var_array == nullptr)
    {
        perror("ReallocVarArray() failed to realloc variable array");

        return kFailedRealloc;
    }

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t TreeCtor(Tree *tree)
{
    CHECK(tree);

    tree->root = nullptr;

    tree->root->left  = nullptr;
    tree->root->right = nullptr;

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

    if (type == kOperator)
    {
        node->data.key_word_code = (KeyCode_t) data;
    }
    else if (type == kIdentificator)
    {
        node->data.variable_pos = data;
    }
    else if (type == kConstNumber)
    {
        node->data.const_val = data;
    }
    else
    {
        printf("NodeCtor() unknown type %d\n", type);

        free(node);

        return nullptr;
    }

    node->left  = left;
    node->right = right;
    node->parent = parent_node;

    return node;
}

//==============================================================================

static TreeErrs_t PrintTree(const TreeNode *root,
                            FILE           *output_file)
{
    CHECK(output_file);

    if (root == nullptr)
    {
        fprintf(output_file,"nil ");

        return kTreeSuccess;
    }

    if (root->type != kConstNumber)
    {
        fprintf(output_file, "( ");
    }

    if (root->left != nullptr)
    {
        PrintTree(root->left, output_file);
    }

    if (root->type == kOperator)
    {
        fprintf(output_file, "%s ", NameTable[root->data.key_word_code].key_word);
    }
    else if (root->type == kConstNumber)
    {
        fprintf(output_file, "%lg ", root->data.const_val);
    }

    if (root->right != nullptr)
    {
        PrintTree(root->right, output_file);
    }

    if (root->type != kConstNumber)
    {
        fprintf(output_file, ") ");
    }

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t PrintTreeInFile(Tree       *tree,
                           const char *file_name)
{
    CHECK(tree);

    FILE *output_file = fopen(file_name, "wb");

    if (output_file == nullptr)
    {
        return kFailedToOpenFile;
    }

    PrintTree(tree->root, output_file);

    fclose(output_file);

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t ReadLanguageElemsOutOfFile(LanguageElems *l_elems,
                                      const char    *file_name)
{
    CHECK(l_elems);

    Text tree_text = {0};

    if (ReadTextFromFile(&tree_text, file_name) != kSuccess)
    {
        printf("\nReadTreeOutOfFile() failed to read text from file\n");

        return kFailedToReadText;

        TextDtor(&tree_text);
    }

    size_t iterator = 0;

    l_elems->syntax_tree.root = CreateNodeFromText(&l_elems->vars, &tree_text, &iterator);

    if (l_elems->syntax_tree.root == nullptr)
    {
        printf("ReatTreeOutOfFile() failed to read tree");

        return kFailedToReadTree;
    }

    GRAPH_DUMP_TREE(&l_elems->syntax_tree);

    return kTreeSuccess;
}

//==============================================================================

static TreeNode* CreateNodeFromText(Variables     *vars,
                                    Text          *text,
                                    size_t        *iterator)
{
    CHECK(iterator);
    CHECK(text);

    TreeNode *node = nullptr;

    if (*text->lines_ptr[*iterator] == '(')
    {
        ++(*iterator);

        ExpressionType_t type = (ExpressionType_t) atoi(text->lines_ptr[*iterator]);

        ++(*iterator);

        switch (type)
        {
            case kConstNumber:
            {
                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kConstNumber,
                                strtod(text->lines_ptr[*iterator], nullptr));
                break;
            }

            case kOperator:
            {
                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kOperator,
                                atoi(text->lines_ptr[*iterator]));
                break;
            }

            case kIdentificator:
            {
                int pos = SeekVariable(vars, text->lines_ptr[*iterator]);

                if (pos < 0)
                {
                    pos = AddVar(vars, text->lines_ptr[*iterator]);
                }
                node = NodeCtor(nullptr,
                                nullptr,
                                nullptr,
                                kIdentificator,
                                pos);

                break;
            }

            default:
            {
                printf("CreateNodeFromText() -> KAVO? 1000-7 ???");

                break;
            }
        }
    }

    ++(*iterator);

    node->left =  CreateNodeFromBrackets(vars, text, iterator);
    node->right = CreateNodeFromBrackets(vars, text, iterator);

    if (*text->lines_ptr[*iterator] != ')')
    {
        TreeDtor(node);

        return nullptr;;
    }

    return node;
}

//==============================================================================

static TreeNode *CreateNodeFromBrackets(Variables     *vars,
                                        Text          *text,
                                        size_t        *iterator)
{
    TreeNode *node = nullptr;

    if (*text->lines_ptr[*iterator] == '(')
    {
        node = CreateNodeFromText(vars, text, iterator);

        if (node == nullptr)
        {
            return nullptr;
        }

        ++(*iterator);
    }
    else if (*text->lines_ptr[*iterator] != ')')
    {
        if (strcmp(text->lines_ptr[*iterator], "null") == 0)
        {
            ++(*iterator);

            return nullptr;
        }
    }

    return node;
}
//==============================================================================

TreeNode *CopyNode(const TreeNode *src_node,
                   TreeNode       *parent_node)
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

TreeErrs_t GetDepth(const TreeNode *node, int *depth)
{

}

//==============================================================================

TreeErrs_t LanguageElemsInit(LanguageElems *l_elems)
{
    TreeCtor(&l_elems->syntax_tree);
    VarArrayInit(&l_elems->vars);

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
