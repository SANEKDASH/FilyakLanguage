#ifndef TREE_DUMP_HEADER
#define TREE_DUMP_HEADER

#define DEBUG

#include "trees.h"
#include "../Frontend/parse.h"

#ifdef DEBUG
#define GRAPH_DUMP_TREE(tree) GraphDumpTree(tree, __FILE__, __func__, __LINE__)
#else
#define GRAPH_DUMP_TREE ;
#endif



TreeErrs_t GraphDumpTree(Tree *tree,
                         const char *file,
                         const char *func,
                         const int line);

void EndTreeGraphDump();

void InitTreeGraphDump();

void LogPrintEdges(TreeNode *node,
                   FILE     *dot_file);

#endif
