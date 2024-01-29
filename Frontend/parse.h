#ifndef PARSE_HEADER
#define PARSE_HEADER

#include "../Common/trees.h"
#include "../Stack/stack.h"

TreeNode *GetSyntaxTree(Variables *vars, const char *file_name);

TreeNode *GetAddExpression(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetExpression(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetMultExpression(Variables *vars, Stack *stk, size_t *iter);

TreeNode* GetConstant(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetIdentificator(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetPrimaryExpression(Variables *vars, Stack *stk, size_t *iter);

#endif
