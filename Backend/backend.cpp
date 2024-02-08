#include "stdio.h"

#include "backend.h"

static const char *main_func_name = "аганим";

static TreeErrs_t AssembleOp(const TreeNode      *node,
                             const LanguageElems *l_elems,
                             TableOfNames        *cur_table,
                             FILE                *output_file);


static TreeErrs_t AsmFuncs(TreeNode      *cur_func,
                           LanguageElems *l_elems,
                           TableOfNames  *cur_table,
                           FILE          *output_file);

static TreeErrs_t AsmOperator(const TreeNode      *node,
                              const LanguageElems *l_elems,
                              TableOfNames        *cur_table,
                              FILE                *output_file);

static TreeErrs_t AsmParams(const TreeNode      *node,
                            const LanguageElems *l_elems,
                            TableOfNames        *cur_table,
                            FILE                *output_file);

static TreeErrs_t AsmExternalDeclarations(TreeNode      *cur_node,
                                          LanguageElems *l_elems,
                                          TableOfNames  *cur_table,
                                          FILE          *output_file);

static size_t GetVarPos(TableOfNames *table,
                        size_t        var_id_pos);

static size_t if_count         = 0;
static size_t logical_op_count = 0;
static size_t cycle_op_count   = 0;


//==============================================================================

static size_t GetVarPos(TableOfNames *table,
                        size_t        var_id_pos)
{
    for (size_t i = 0; i < table->name_count; i++)
    {
        if (var_id_pos == table->names[i].pos)
        {
            return i;
        }
    }
    return 0;
}

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

    AsmExternalDeclarations(l_elems->syntax_tree.root, l_elems, l_elems->tables.name_tables[0], output_file);

    fclose(output_file);

    return kTreeSuccess;
}

//==============================================================================

#define ASM_PRINT(...) fprintf(output_file, __VA_ARGS__)
#define ASM_NODE(node) AssembleOp(node, l_elems, cur_table, output_file)

//==============================================================================

static TreeErrs_t AsmExternalDeclarations(TreeNode      *cur_node,
                                          LanguageElems *l_elems,
                                          TableOfNames  *cur_table,
                                          FILE          *output_file)

{
    TreeNode *decl_node = cur_node;

    while (decl_node != nullptr)
    {
        ASM_NODE(decl_node->left);

        decl_node = decl_node->right;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AssembleFuncDef(const TreeNode      *node,
                                  const LanguageElems *l_elems,
                                  TableOfNames        *cur_table,
                                  FILE                *output_file)
{
    TreeNode *func = (TreeNode *) node;

    TreeNode *params = func->right;

    ASM_PRINT("%s:\n", l_elems->vars.var_array[func->data.variable_pos].id);

    TableOfNames *func_table = nullptr;

    for (size_t i = 0; i < l_elems->tables.tables_count; i++)
    {
        if (l_elems->tables.name_tables[i]->func_code == func->data.variable_pos)
        {
            func_table = l_elems->tables.name_tables[i];

            break;
        }
    }

    if (func_table == nullptr)
    {
        printf(">>AsmFuncs() failed to find table\n");

        return kFailedToFind;
    }

    TreeNode *cur_instruction = params->right;

    while (cur_instruction != nullptr)
    {
        AssembleOp(cur_instruction->left, l_elems, func_table, output_file);

        cur_instruction = cur_instruction->right;
    }

    if (func->data.variable_pos == l_elems->tables.main_id_pos)
    {
        ASM_PRINT("hlt\n");
    }
    else
    {
        ASM_PRINT("ret\n");
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AssembleOp(const TreeNode      *node,
                             const LanguageElems *l_elems,
                             TableOfNames        *cur_table,
                             FILE                *output_file)
{
    if (node == nullptr)
    {
        return kNullTree;
    }

    switch (node->type)
    {
        case kConstNumber:
        {
            ASM_PRINT("\tpush %lg\n", node->data.const_val);

            break;
        }

        case kIdentificator:
        {
            size_t var_pos = GetVarPos(cur_table, node->data.variable_pos) + 1;

            ASM_PRINT(";PUSH VAR '%s'\n",
                      l_elems->vars.var_array[node->data.variable_pos].id);

            ASM_PRINT("\tpush [rbx+%d]\n", var_pos);

            break;
        }

        case kFuncDef:
        {
            AssembleFuncDef(node, l_elems, cur_table, output_file);

            break;
        }

        case kVarDecl:
        {
            if (node->right != nullptr)
            {
                ASM_PRINT("\t\t;VAR '%s' DECLARATION\n ",
                          l_elems->vars.var_array[node->data.variable_pos].id);

                if (node->right->type == kOperator && node->right->data.key_word_code == kAssign)
                {
                    ASM_NODE(node->right);
                }
            }

            break;
        }

        case kCall:
        {
            ASM_PRINT(";FUNC CALL\n");

            size_t var_count = cur_table->name_count + 1;

            AsmParams(node->left, l_elems, cur_table, output_file);

            ASM_PRINT("\tpush rbx\n"
                      "\tpush rbx\n"
                      "\tpush %d\n"
                      "\tadd\n"
                      "\tpop rbx\n", var_count);

            ASM_PRINT("\tcall %s\n"
                      "\tpop rbx\n"
                      "\tpush [rbx+%d]\t\t\t;ret_value\n",
                      l_elems->vars.var_array[node->right->data.variable_pos].id,
                      var_count);

            return kTreeSuccess;
        }

        case kOperator:
        {
            AsmOperator(node, l_elems, cur_table, output_file);

            break;
        }

        default:
        {
            printf("AssembleOp() unknown type\n");

            return kUnknownType;
        }
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AsmParams(const TreeNode      *node,
                            const LanguageElems *l_elems,
                            TableOfNames        *cur_table,
                            FILE                *output_file)
{
    TreeNode *cur_param = (TreeNode *)node;

    size_t var_count = cur_table->name_count + 1;


    for (size_t i = 0; cur_param != nullptr; i++)
    {
        ASM_NODE(cur_param->left);

        ASM_PRINT("\tpop [rbx+%d]\n", var_count + i + 1);

        cur_param = cur_param->right;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t AsmOperator(const TreeNode      *node,
                              const LanguageElems *l_elems,
                              TableOfNames        *cur_table,
                              FILE                *output_file)
{
    switch (node->data.key_word_code)
    {
        case kAdd:
        {
            ASM_PRINT(";ADD\n");
            ASM_NODE(node->left);
            ASM_NODE(node->right);

            ASM_PRINT("\tadd\n\n");

            break;
        }

        case kIf:
        {
            ASM_PRINT(";IF\n");

            ASM_PRINT("\tpush 0\n");
            ASM_NODE(node->left);

            ASM_PRINT("\tjbe end_if_%d\n", if_count);


            TreeNode *cur_op = node->right;

            while (cur_op != nullptr)
            {
                AssembleOp(cur_op->left, l_elems, cur_table, output_file);

                cur_op = cur_op->right;
            }

            ASM_PRINT("end_if_%d:\n", if_count++);

            ++if_count;

            return kTreeSuccess;

            break;
        }

        case kWhile:
        {
            ASM_PRINT("cycle_start_%d:\n"
                      "\tpush 0\n", cycle_op_count);

            ASM_NODE(node->left);

            size_t cur_cycle_op_count = cycle_op_count++;

            ASM_PRINT("\tjbe while_end_%d\n", cycle_op_count);

            TreeNode *cur_op = node->right;

            while (cur_op != nullptr)
            {
                AssembleOp(cur_op->left, l_elems, cur_table, output_file);

                cur_op = cur_op->right;
            }

            ASM_PRINT("\tjmp cycle_start_%d\n"
                      "cycle_end_%d:\n", cur_cycle_op_count, cur_cycle_op_count);

            break;
        }

        case kBreak:
        {
            ASM_PRINT("jmp cycle_end_%d\n", cycle_op_count);

            break;
        }

        case kContinue:
        {
            ASM_PRINT("jmp cycle_start_%d\n", cycle_op_count);

            break;
        }

        case kScan:
        {
            ASM_PRINT("\tin\n");

            break;
        }

        #define ASM_CMD(const, static_var_name, jmp_str, label_str, ...)            \
            case const:                                                             \
            {                                                                       \
                static size_t static_var_name = 0;                                  \
                                                                                    \
                ASM_PRINT("\tpush 0\n");                                            \
                                                                                    \
                ASM_NODE(node->left);                                               \
                ASM_NODE(node->right);                                              \
                ASM_PRINT("\tsub\n"                                                 \
                          "\t%s %s_%d\n"                                            \
                          "\tpush 0\n"                                              \
                          "\tjmp logical_op_end_%d\n"                               \
                          "%s_%d:\n"                                                \
                          "\tpush 1\n"                                              \
                          "logical_op_end_%d:\n", jmp_str, label_str,               \
                                                    static_var_name,                \
                                                    logical_op_count,               \
                                                    label_str, static_var_name,     \
                                                    logical_op_count);              \
                ++logical_op_count;                                                 \
                ++static_var_name;                                                  \
                                                                                    \
                break;                                                              \
            }

        #include "logical_op.gen.h"

        #undef ASM_CMD

        case kPrint:
        {
            ASM_PRINT(";PRINT\n");
            ASM_NODE(node->right);

            ASM_PRINT("\tout\n");

            break;
        }
        case kSub:
        {
            ASM_PRINT(";SUB\n");
            ASM_NODE(node->left);
            ASM_NODE(node->right);

            ASM_PRINT("\tsub\n");

            break;
        }

        case kAssign:
        {
            ASM_PRINT(";ASSIGN\n");
            ASM_NODE(node->left);

            size_t var_pos = GetVarPos(cur_table, node->right->data.variable_pos) + 1;

            ASM_PRINT("\tpop [rbx+%d]\n", var_pos);

            break;
        }

        case kMult:
        {
            ASM_PRINT(";MULT\n");
            ASM_NODE(node->left);
            ASM_NODE(node->right);

            ASM_PRINT("\tmult\n");

            break;
        }

        case kSqrt:
        {
            ASM_PRINT(";SQRT\n");
            ASM_NODE(node->right);

            ASM_PRINT("\tsqrt\n");

            break;
        }

        case kDiv:
        {
            ASM_PRINT(";DIV\n");
            ASM_NODE(node->left);
            ASM_NODE(node->right);

            ASM_PRINT("\tdiv\n");

            break;
        }

        case kReturn:
        {
            ASM_PRINT(";RETURN\n");
            ASM_NODE(node->right);

            ASM_PRINT("\tpop [rbx]\n"
                      "ret\n");

            break;
        }

        case kAbort:
        {
            ASM_PRINT(";ABORT\n");
            ASM_PRINT("\thlt\n\n");

            break;
        }

        default:
        {
            printf("AssembleOperator() KAVO TYPE : %d, OP_CODE : %d\n, node_pointer: %p", node->type,
                                                                                          node->data.key_word_code,
                                                                                          node);

            return kUnknownKeyCode;

            break;
        }
    }

    return kTreeSuccess;
}

//==============================================================================
