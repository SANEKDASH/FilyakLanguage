#ifndef PARSE_HEADER
#define PARSE_HEADER

#include "../Common/trees.h"
#include "../Stack/stack.h"

TreeNode *GetG(Variables *vars, const char *file_name);

TreeNode *GetL(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetE(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetP(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetT(Variables *vars, Stack *stk, size_t *iter);

TreeNode* GetN(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetId(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetA(Variables *vars, Stack *stk, size_t *iter);

TreeNode *GetCode(Variables *vars, Stack *stk, size_t *iter);

#endif
