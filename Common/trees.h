#ifndef TREES_HEADER
#define TREES_HEADER

#include "../TextParse/text_parse.h"
#include "NameTable.h"

typedef double VarType_t;

struct Expr
{
    const char *string = nullptr;
    size_t pos;
    size_t line_number;
};

typedef enum
{
    kVar,
    kFunc,
    kUndefined,
} IdType_t;

struct Variable
{
    const char *id = nullptr;
    bool        declaration_state;
    IdType_t    id_type;
};

struct Variables
{
    VarType_t type;
    Variable *var_array;
    size_t size;
    size_t var_count;
};

typedef double NumType_t;

typedef enum
{
    kTreeSuccess,
    kFailedAllocation,
    kFailedToOpenFile,
    kFailedRealloc,
    kFailedToFind,
    kFailedToReadText,
    kFailedToReadTree,
    kTreeOptimized,
    kTreeNotOptimized,
    kNullTree,
} TreeErrs_t;

union NodeData
{
    NumType_t   const_val;
    KeyCode_t   key_word_code;
    size_t      variable_pos;
    size_t      line_number;
    const char *id;
};

struct TreeNode
{
    size_t line_number;

    NodeData data;

    ExpressionType_t type;

    TreeNode *parent;
    TreeNode *left;
    TreeNode *right;
};

struct Tree
{
    TreeNode *root;
};

struct LanguageElems
{
    Variables vars;
    Tree      syntax_tree;
};

int VarArrayDtor(Variables *vars);

int AddVar(Variables *vars, const char *var_name);

int SeekVariable(Variables *vars, const char *var_name);

int VarArrayInit(Variables *vars);

TreeErrs_t LanguageElemsInit(LanguageElems *l_elems);

TreeErrs_t LanguageElemsDtor(LanguageElems *l_elems);

TreeErrs_t TreeVerify(Tree *tree);

TreeErrs_t TreeCtor(Tree *tree);

TreeNode *NodeCtor(TreeNode         *parent_node,
                   TreeNode         *left,
                   TreeNode         *right,
                   ExpressionType_t  type,
                   double            data);

TreeErrs_t TreeDtor(TreeNode *root);

TreeErrs_t PrintTreeInFile(Tree       *tree,
                           Variables  *vars,
                           const char *file_name);

TreeErrs_t ReadLanguageElemsOutOfFile(LanguageElems *l_elems,
                                      const char    *file_name);

TreeNode *CopyNode(const TreeNode *src_node,
                   TreeNode       *parent_node);

TreeErrs_t SetParents(TreeNode *parent_node);

TreeErrs_t GetDepth(const TreeNode *node, int *depth);

#endif
