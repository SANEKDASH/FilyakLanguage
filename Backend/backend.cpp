#include "stdio.h"

#include "backend.h"


static TreeErrs_t AssembleOp(const TreeNode  *node,
                             const Variables *vars,
                             FILE            *output_file);

TreeErrs_t MakeAsmCode(LanguageElems *l_elems,
                       const char    *output_file_name)
{
    FILE *output_file = fopen(output_file_name, "w");

    if (output_file == nullptr)
    {
        perror("MakeAsmCode() failed to open output_file");

        return kFailedToOpenFile;
    }

    TreeNode *curr_op = l_elems->syntax_tree.root;

    while (curr_op != nullptr)
    {
        AssembleOp(curr_op->left, &l_elems->vars, output_file);
        curr_op = curr_op->right;
    }

    fclose(output_file);

    return kTreeSuccess;
}

//что-то сделать со строками, т.к. они не должны просто так висеть
//в плане push pop и т.д.

static TreeErrs_t AssembleOp(const TreeNode  *node,
                             const Variables *vars,
                             FILE            *output_file)
{
    #define ASM_PRINT(...) fprintf(output_file, __VA_ARGS__)
    #define ASM_NODE(node) AssembleOp(node, vars, output_file)
    if (node == nullptr)
    {
        return kNullTree;
    }

    if (node->type == kConstNumber)
    {
        printf("HUY num\n");
        ASM_PRINT("push %lg\n", node->data.const_val);

        return kTreeSuccess;
    }

    if (node->type == kOperator)
    {
        printf("HUY OPERATOR\n");
        switch (node->data.key_word_code)
        {
            case kAdd:
            {
                ASM_NODE(node->left);
                ASM_NODE(node->right);

                ASM_PRINT("add\n");

                break;
            }

            case kSub:
            {
                ASM_NODE(node->left);
                ASM_NODE(node->right);

                ASM_PRINT("sub\n");

                break;
            }

            default:
            {
                printf("AssembleOp() KAVO OP_CODE : %d", node->data.key_word_code);

                /*whaaaa? кого ты блять ищешь?
                 +------------------*/
                return kFailedToFind;

                break;
            }
        }
    }

    return kTreeSuccess;
}
