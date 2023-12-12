#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "debug/debug.h"
#include "trees.h"
#include "tree_dump.h"
#include "time.h"

#define SVG

static size_t call_count = 0;

static FILE *log_file = nullptr;

static const char * const log_file_name = "tree.dmp.html";

static void LogPrintTree(TreeNode *node,
                         FILE     *dot_file);

//================================================================================================

void InitTreeGraphDump()
{
    system("rm -f *.svg");
    system("rm -f *.png");

    log_file = fopen(log_file_name, "w");

    if (log_file == nullptr)
    {
        perror("BeginListGraphDump() failed to open file");

        return;
    }

    fprintf(log_file, "<pre>\n");
}

//================================================================================================

void EndTreeGraphDump()
{
    if (log_file == nullptr)
    {
        perror("EndLostGrapDump() failed to write an info");

        return;
    }

    fclose(log_file);
}

//================================================================================================

TreeErrs_t GraphDumpTree(Tree *tree,
                         const char *file,
                         const char *func,
                         const int line)
{
    FILE *dot_file = fopen("tree.dmp.dot", "w");

    #define LOG_PRINT(...) fprintf(dot_file, __VA_ARGS__)

    if (dot_file == nullptr)
    {
        perror("GraphDumpList() failed to open dump file");

        return kFailedToOpenFile;
    }

    static char cmd_command[200] = {0};

    assert(dot_file);

    LOG_PRINT("digraph List\n{\n"
              "\trankdir = TB;\n"
              "\tgraph [bgcolor = \"black\"]\n"
              "\tnode[color =\"black\", fontsize=14, shape = Mrecord];\n"
              "\tedge[color = \"red\", fontcolor = \"blue\",fontsize = 12];\n\n\n");


    LogPrintTree(tree->root, dot_file);

    LogPrintEdges(tree->root, dot_file);


    LOG_PRINT("\n\n}");

    fclose(dot_file);

    //system("iconv -f CP1251 -t UTF-8 tree.dmp.dot > ctree.dmp.dot");

    sprintf(cmd_command, "dot -Tsvg tree.dmp.dot -o graphdump%d.svg"
                         , call_count);

    system(cmd_command);


    fprintf(log_file, "DATE : %s \nTIME : %s\n"
                      "Called from file: %s\n"
                      "Called from function: %s\n"
                      "Line: %d\n"
                      "<img height=\"150px\" src=\"graphdump%d.svg\">\n"
                      "-----------------------------------------------------------------\n",
                      __DATE__,
                      __TIME__,
                      file,
                      func,
                      line,
                      call_count);


    ++call_count;

    return kTreeSuccess;
}

//================================================================================================

static void LogPrintTree(TreeNode *node,
                         FILE     *dot_file)
{
    if (node->type == kOperator)
    {
        LOG_PRINT("node%p [style = filled, fillcolor = \"lightgreen\", shape = Mrecord, label = "
                  "\"data: %s | {type : operator | op_code : %d} | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
                  NameTable[node->data.key_word_code].key_word,
                  node->data.key_word_code,
                  node->parent,
                  node,
                  node->left,
                  node->right);
    }
    else if (node->type == kConstNumber)
    {
        LOG_PRINT("node%p [style = filled, fillcolor = \"lightblue\", shape = Mrecord, label = "
                  "\"data: %lg | type : const number | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
                  node->data.const_val,
                  node->parent,
                  node,
                  node->left,
                  node->right);
    }
    else if (node->type == kIdentificator)
    {
        LOG_PRINT("node%p [style = filled, fillcolor = \"pink\", shape = Mrecord, label = "
                  "\"data: %d | type : variable | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
                  node->data.variable_pos,
                  node->parent,
                  node,
                  node->left,
                  node->right);
    }

    if (node->left != nullptr)
    {
        LogPrintTree(node->left, dot_file);
    }

    if (node->right != nullptr)
    {
        LogPrintTree(node->right, dot_file);
    }
}

//================================================================================================

void LogPrintEdges(TreeNode *node,
                   FILE     *dot_file)
{
    if (node->left != nullptr)
    {
        LOG_PRINT("node%p->node%p\n",
                  node,
                  node->left);
    }

    if (node->parent != nullptr)
    {
        LOG_PRINT("node%p->node%p[color = \"yellow\"]\n",
                  node,
                  node->parent);
    }

    if (node->right != nullptr)
    {
        LOG_PRINT("node%p->node%p\n",
                  node,
                  node->right);
    }

    if (node->left != nullptr)
    {
        LogPrintEdges(node->left, dot_file);
    }

    if (node->right != nullptr)
    {
        LogPrintEdges(node->right, dot_file);
    }
}

#undef LOG_PRINT

#undef PRINT_BR
#undef TEX_PRINT


