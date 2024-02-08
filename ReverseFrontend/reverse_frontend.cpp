#include "reverse_frontend.h"

static TreeErrs_t PrintExternalDeclarations(TreeNode      *node,
                                            LanguageElems *l_elems,
                                            FILE          *output_file);

static TreeErrs_t PrintFuncDefinition(TreeNode      *node,
                                      LanguageElems *l_elems,
                                      FILE          *output_file);

static TreeErrs_t PrintDeclaration(TreeNode      *node,
                                   LanguageElems *l_elems,
                                   FILE          *output_file);

static const char *FindKeyword(KeyCode_t code);

static TreeErrs_t PrintFuncDefinition(TreeNode      *node,
                                      LanguageElems *l_elems,
                                      FILE          *output_file);

static TreeErrs_t PrintDefParams(TreeNode      *node,
                                 LanguageElems *l_elems,
                                 FILE          *output_file);

static TreeErrs_t PrintInstructions(TreeNode      *node,
                                    LanguageElems *l_elems,
                                    FILE          *output_file);

static TreeErrs_t PrintOperator(TreeNode      *node,
                                LanguageElems *l_elems,
                                FILE          *output_file);

static TreeErrs_t PrintFuncCall(TreeNode      *node,
                                LanguageElems *l_elems,
                                FILE          *output_file);

static TreeErrs_t PrintParams(TreeNode       *node,
                              LanguageElems  *l_elems,
                              FILE           *output_file);

static TreeErrs_t PrintVarDeclaration(TreeNode      *node,
                                      LanguageElems *l_elems,
                                      FILE          *output_file);

static TreeErrs_t PrintNode(TreeNode      *node,
                            LanguageElems *l_elems,
                            FILE          *output_file);


//==============================================================================

TreeErrs_t ReverseFrontend(LanguageElems *l_elems,
                           const char    *output_file_name)
{
    FILE *output_file = fopen(output_file_name, "w");

    if (output_file == nullptr)
    {
        printf("ReverseFrontend() failed to open file");

        return kFailedToOpenFile;
    }

    PrintExternalDeclarations(l_elems->syntax_tree.root, l_elems, output_file);

    fclose(output_file);

    return kTreeSuccess;
}

//==============================================================================

static const char *FindKeyword(KeyCode_t code)
{
    for (size_t i = 0; i < kKeyWordCount; i++)
    {
        if (NameTable[i].key_code == code)
        {
            return NameTable[i].key_word;
        }
    }

    return nullptr;
}

//==============================================================================

#define FRONT_PRINT(...)    fprintf(output_file, __VA_ARGS__)
#define PRINT_END_OF_LINE() fprintf(output_file, "%s\n", FindKeyword(kEndOfLine));

//==============================================================================

static TreeErrs_t PrintExternalDeclarations(TreeNode      *node,
                                            LanguageElems *l_elems,
                                            FILE          *output_file)
{
    TreeNode *decl_node = node;

    while (decl_node != nullptr)
    {
        PrintDeclaration(decl_node->left, l_elems, output_file);

        FRONT_PRINT("\n");

        decl_node = decl_node->right;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintDeclaration(TreeNode      *node,
                                   LanguageElems *l_elems,
                                   FILE          *output_file)
{
    if (node->type == kVarDecl)
    {
        PrintVarDeclaration(node, l_elems, output_file);
    }
    else if (node->type == kFuncDef)
    {
        PrintFuncDefinition(node, l_elems, output_file);
    }
    else
    {
        printf("What the fuck?\n");

        return kUnknownType;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintVarDeclaration(TreeNode      *node,
                                      LanguageElems *l_elems,
                                      FILE          *output_file)
{
    FRONT_PRINT("%s ", FindKeyword(node->left->data.key_word_code));

    PrintNode(node->right, l_elems, output_file);

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintFuncDefinition(TreeNode      *node,
                                      LanguageElems *l_elems,
                                      FILE          *output_file)
{
    FRONT_PRINT("%s %s ", FindKeyword(node->left->data.key_word_code),
                          l_elems->vars.var_array[node->data.variable_pos]);

    TreeNode *params_node = node->right;

    PrintDefParams(params_node->left, l_elems, output_file);

    FRONT_PRINT("%s\n", FindKeyword(kLeftZoneBracket));

    PrintInstructions(params_node->right, l_elems, output_file);

    FRONT_PRINT("%s\n", FindKeyword(kRightZoneBracket));

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintDefParams(TreeNode      *node,
                                 LanguageElems *l_elems,
                                 FILE          *output_file)
{
    FRONT_PRINT("%s ", FindKeyword(kLeftBracket));

    TreeNode *params_node = node;

    if (params_node->left != nullptr)
    {
        PrintDeclaration(params_node->left, l_elems, output_file);
    }

    params_node = params_node->right;

    while (params_node != nullptr)
    {
        FRONT_PRINT("%s ", FindKeyword(kEnumOp));

        PrintDeclaration(params_node->left, l_elems, output_file);

        params_node = params_node->right;
    }

    FRONT_PRINT("%s\n", FindKeyword(kRightBracket));

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintInstructions(TreeNode      *node,
                                    LanguageElems *l_elems,
                                    FILE          *output_file)
{
    TreeNode *instruction_node = node;

    while (instruction_node != nullptr)
    {
        PrintNode(instruction_node->left, l_elems, output_file);

        if ((instruction_node->left->type == kOperator &&
             instruction_node->left->data.key_word_code != kIf) ||
            (instruction_node->left->type == kCall)     ||
            (instruction_node->left->type == kVarDecl))
        {
            FRONT_PRINT("%s", FindKeyword(kEndOfLine));
        }

        FRONT_PRINT("\n");

        instruction_node = instruction_node->right;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintNode(TreeNode      *node,
                            LanguageElems *l_elems,
                            FILE          *output_file)
{
    if (node == nullptr)
    {
        return kNullTree;
    }

    switch (node->type)
    {
        case kConstNumber:
        {
            FRONT_PRINT("%lg ", node->data.const_val);

            break;
        }

        case kIdentificator:
        {
            FRONT_PRINT("%s ", l_elems->vars.var_array[node->data.variable_pos].id);

            break;
        }

        case kOperator:
        {
            PrintOperator(node, l_elems, output_file);

            break;
        }


        case kVarDecl:
        {
            PrintVarDeclaration(node, l_elems, output_file);

            break;
        }

        case kCall:
        {
            PrintFuncCall(node, l_elems, output_file);

            break;
        }

        case kFuncDef:

        default:
        {
            printf("LUCHSHE BE BOSS THEN OBSOS\n");

            break;
        }
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintOperator(TreeNode      *node,
                                LanguageElems *l_elems,
                                FILE          *output_file)
{
    if (node == nullptr)
    {
        return kNullTree;
    }

    switch (node->data.key_word_code)
    {
        case kAdd:
        case kSub:
        case kMult:
        case kDiv:
        case kMore:
        case kLess:
        case kAnd:
        case kOr:
        case kMoreOrEqual:
        case kLessOrEqual:
        case kEqual:
        {
            PrintNode(node->left, l_elems, output_file);

            FRONT_PRINT("%s ", FindKeyword(node->data.key_word_code));

            PrintNode(node->right, l_elems, output_file);

            break;
        }

        case kFloor:
        case kDiff:
        case kSqrt:
        case kReturn:
        case kContinue:
        case kBreak:
        case kPrint:
        case kScan:
        case kAbort:
        {
            FRONT_PRINT("%s %s ", FindKeyword(node->data.key_word_code),
                                  FindKeyword(kLeftBracket));

            PrintNode(node->right, l_elems, output_file);

            FRONT_PRINT("%s ", FindKeyword(kRightBracket));

            break;
        }

        case kWhile:
        {
            FRONT_PRINT("%s %s ", FindKeyword(kWhile), FindKeyword(kLeftBracket));

            PrintNode(node->left, l_elems, output_file);

            FRONT_PRINT("%s\n%s\n", FindKeyword(kRightBracket), FindKeyword(kLeftZoneBracket));

            PrintInstructions(node->right, l_elems, output_file);

            FRONT_PRINT("%s", FindKeyword(kRightZoneBracket));

            break;
        }

        case kIf:
        {
            FRONT_PRINT("%s %s ", FindKeyword(kIf), FindKeyword(kLeftBracket));

            PrintNode(node->left, l_elems, output_file);

            FRONT_PRINT("%s\n%s\n", FindKeyword(kRightBracket), FindKeyword(kLeftZoneBracket));

            PrintInstructions(node->right, l_elems, output_file);

            FRONT_PRINT("%s", FindKeyword(kRightZoneBracket));

            break;
        }

        case kAssign:
        {
            PrintNode(node->right, l_elems, output_file);

            FRONT_PRINT("%s ", FindKeyword(kAssign));

            PrintNode(node->left, l_elems, output_file);

            break;
        }

        default:
        {
            printf("UNKNOWN OP_CODE!!! %d; node pointer = %p\n", node->data.key_word_code, node);

            return kUnknownKeyCode;

            break;
        }
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintFuncCall(TreeNode      *node,
                                LanguageElems *l_elems,
                                FILE          *output_file)
{
    FRONT_PRINT("%s ", l_elems->vars.var_array[node->right->data.variable_pos].id);

    PrintParams(node->left, l_elems, output_file);

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t PrintParams(TreeNode       *node,
                              LanguageElems  *l_elems,
                              FILE           *output_file)
{
    FRONT_PRINT("%s ", FindKeyword(kLeftBracket));

    TreeNode *params_node = node;

    if (params_node->left != nullptr)
    {
        PrintNode(params_node->left, l_elems, output_file);
    }

    params_node = params_node->right;

    while (params_node != nullptr)
    {
        FRONT_PRINT("%s ", FindKeyword(kEnumOp));

        PrintNode(params_node->left, l_elems, output_file);

        params_node = params_node->right;
    }

    FRONT_PRINT("%s ", FindKeyword(kRightBracket));

    return kTreeSuccess;
}

