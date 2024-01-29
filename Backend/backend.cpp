#include "stdio.h"

#include "backend.h"

static const char *main_func_name = "аганим";

static TreeErrs_t AssembleOp(const TreeNode  *node,
                             const Variables *vars,
                             FILE            *output_file);


static TreeErrs_t AsmFuncs(TreeNode      *cur_func,
                           LanguageElems *l_elems,
                           FILE          *output_file);

static size_t condition_count = 0;

//==============================================================================

TreeErrs_t MakeAsmCode(LanguageElems *l_elems,
                       const char    *output_file_name)
{
    FILE *output_file = fopen(output_file_name, "w");

    if (output_file == nullptr)
    {
        perror("MakeAsmCode() failed to open output_file");

        return kFailedToOpenFile;
    }

    fprintf(output_file, "jmp %s\n", main_func_name);

    TreeNode *curr_op = l_elems->syntax_tree.root;

    AsmFuncs(l_elems->syntax_tree.root, l_elems, output_file);



    fclose(output_file);

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AsmMain(LanguageElems *l_elems,
                          FILE          *output_file)
{
    #define ASM_PRINT(...) fprintf(output_file, __VA_ARGS__)
    #define ASM_NODE(node) AssembleOp(node, vars, output_file)

    TreeNode *func = l_elems->syntax_tree.root->left;

    ASM_PRINT("jmp %s\n", l_elems->vars.var_array[func->data.variable_pos].id);
    ASM_PRINT("%s:\n");

    func = func->right->right;
//
    while (func != nullptr)
    {
        AssembleOp(func->left, &l_elems->vars, output_file);

        func = func->right;
    }

    ASM_PRINT("\nhlt\n\n");

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AsmFuncs(TreeNode      *cur_func,
                           LanguageElems *l_elems,
                           FILE          *output_file)

{
    TreeNode *func      = cur_func;
    TreeNode *next_func = func->right;

    if (func != nullptr)
    {
    printf("HUY %d\n", __LINE__);
        func = func->left;

        const char *func_name = l_elems->vars.var_array[func->data.variable_pos].id;

        ASM_PRINT("\n\n\n%s:\n", func_name);

        func = func->right->right;

        while (func != nullptr)
        {
            AssembleOp(func->left, &l_elems->vars, output_file);

            func = func->right;
        }

        if (strcmp(func_name, main_func_name) == 0)
        {
            ASM_PRINT("\n\nhlt\n");
        }
        else
        {
            ASM_PRINT("\nret\n");
        }
    }

    if (next_func != nullptr)
    {
        AsmFuncs(next_func, l_elems, output_file);
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AssembleOp(const TreeNode  *node,
                             const Variables *vars,
                             FILE            *output_file)
{
    if (node == nullptr)
    {
        return kNullTree;
    }
    else if (node->type == kConstNumber)
    {
        ASM_PRINT("push %lg\n", node->data.const_val);

        return kTreeSuccess;
    }
    else if (node->type == kIdentificator)
    {
        ASM_PRINT("push [%d]\n", node->data.variable_pos);

        return kTreeSuccess;
    }
    else if (node->type == kVarDecl)
    {
        if (node->right->type               == kOperator &&
            node->right->data.key_word_code == kAssign)
        {
            ASM_NODE(node->right);
        }

        return kTreeSuccess;
    }
    else if (node->type == kCall)
    {
        ASM_PRINT("call %s\n"
                  "push rax\n",
                  vars->var_array[node->right->data.variable_pos].id);

        return kTreeSuccess;
    }

    if (node->type == kOperator)
    {
        switch (node->data.key_word_code)
        {
            case kAdd:
            {
                ASM_NODE(node->left);
                ASM_NODE(node->right);

                ASM_PRINT("add\n\n");

                break;
            }

            case kIf:
            {
                ASM_PRINT("\n");

                ASM_NODE(node->left);

                if (node->left->data.key_word_code == kEqual)
                {
                    ASM_NODE(node->left->left);
                    ASM_NODE(node->left->right);

                    ASM_PRINT("jne end%d\n", condition_count);
                }
                else
                {
                    ASM_PRINT("push 0\n"
                              "jbe end%d\n", condition_count);
                }

                TreeNode *cur_line = node->right;

                while (cur_line != nullptr)
                {
                    AssembleOp(cur_line->left, vars, output_file);

                    cur_line = cur_line->right;
                }

                ASM_PRINT("end%d:\n", condition_count++);

                ++condition_count;

                return kTreeSuccess;

                break;
            }

            case kScan:
            {
                ASM_PRINT("in\n");

                break;
            }

            case kPrint:
            {
                ASM_NODE(node->right);

                ASM_PRINT("out\n");

                break;
            }
            case kSub:
            {
                ASM_NODE(node->left);
                ASM_NODE(node->right);

                ASM_PRINT("sub\n\n");

                break;
            }

            case kAssign:
            {
                ASM_NODE(node->left);

                ASM_PRINT("pop [%d]\n", node->right->data.variable_pos);

                break;
            }

            case kMult:
            {
                ASM_NODE(node->left);
                ASM_NODE(node->right);

                ASM_PRINT("mult\n\n");

                break;
            }

            case kSqrt:
            {
                ASM_NODE(node->right);

                ASM_PRINT("sqrt\n\n");

                break;
            }

            case kDiv:
            {
                ASM_NODE(node->left);
                ASM_NODE(node->right);

                ASM_PRINT("\ndiv\n");

                break;
            }

            case kReturn:
            {
                ASM_NODE(node->right);

                ASM_PRINT("pop rax\n");

                break;
            }

            default:
            {
                printf("AssembleOp() KAVO OP_CODE : %d\n", node->data.key_word_code);

                /*whaaaa? кого ты блять ищешь?
                 +------------------*/
                return kFailedToFind;

                break;
            }
        }
    }

    return kTreeSuccess;
}
