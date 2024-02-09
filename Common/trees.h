#ifndef TREES_HEADER
#define TREES_HEADER

#include "../TextParse/text_parse.h"
#include "NameTable.h"

#ifdef TABLES_DEBUG
#define TABLES_DUMP(tables) DumpNameTables(tables, __func__, __LINE__)
#else
#define TABLES_DUMP(tables)
#endif

typedef double VarType_t;

typedef enum
{
    kVar       = 1,
    kFunc      = 2,
    kType      = 4,
    kUndefined = 3,
} IdType_t;

struct Expr
{
    char *string = nullptr;
    size_t pos;
    size_t line_number;
};

struct Name
{
    size_t   pos;
    IdType_t type;
};

struct TableOfNames
{
    Name *names;

    size_t capacity;
    size_t name_count;

    int func_code;

    size_t pos;
};

struct NameTables
{
    TableOfNames **name_tables;

    size_t main_id_pos;

    size_t capacity;
    size_t tables_count;
};


struct Identificator
{
    char *id =  nullptr;
    bool        declaration_state;
    IdType_t    id_type;
};

struct Identificators
{
    VarType_t type;
    Identificator *var_array;
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
    kMissingMain,
    kUnknownType,
    kUnknownKeyCode
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
    Identificators vars;

    NameTables tables;

    Tree syntax_tree;
};

int NameTablesInit(NameTables *name_tables);

int DumpNameTables(NameTables *tables,
                   const char *func,
                   int         line);

int CloseNamesLog();

int InitNamesLog();

TableOfNames *AddTableOfNames(NameTables *tables,
                              int         func_code);

int TablesOfNamesInit(TableOfNames *table,
                      int           func_code);
int AddName(TableOfNames *table,
            size_t        id,
            IdType_t      type);

int VarArrayDtor(Identificators *vars);

int AddVar(Identificators *vars,
           char           *var_name);

int SeekVariable(Identificators *vars,
                 const char     *var_name);

int VarArrayInit(Identificators *vars);

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

TreeErrs_t PrintTreeInFile(LanguageElems *l_elems,
                           const char    *file_name);

TreeErrs_t ReadLanguageElemsOutOfFile(LanguageElems *l_elems,
                                      const char    *tree_file_name,
                                      const char    *tables_file);

TreeNode *CopyNode(const TreeNode *src_node,
                         TreeNode *parent_node);

TreeErrs_t SetParents(TreeNode *parent_node);

TreeErrs_t GetDepth(const TreeNode *node, int *depth);

#endif
