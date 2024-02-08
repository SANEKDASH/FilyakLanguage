#ifndef PARSE_HEADER
#define PARSE_HEADER

#include "../Common/trees.h"
#include "../Stack/stack.h"

TreeNode *GetSyntaxTree(Identificators *vars,
                        const char     *file_name);

TreeNode *GetAddExpression(Identificators *vars,
                           Stack          *stk,
                           size_t         *iter);

TreeNode *GetExpression(Identificators *vars,
                        Stack          *stk,
                        size_t         *iter);

TreeNode *GetMultExpression(Identificators *vars,
                            Stack          *stk,
                            size_t         *iter);

TreeNode* GetConstant(Identificators *vars,
                      Stack          *stk,
                      size_t         *iter);

TreeNode *GetIdentificator(Identificators *vars,
                           Stack          *stk,
                           size_t         *iter);

TreeNode *GetPrimaryExpression(Identificators *vars,
                               Stack          *stk,
                               size_t         *iter);

#endif
