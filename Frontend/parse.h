#ifndef PARSE_HEADER
#define PARSE_HEADER

#include "../trees.h"

TreeNode *GetG(Variables *vars, Expr *expr);

TreeNode *GetE(Variables *vars, Expr *expr);

TreeNode *GetP(Variables *vars, Expr *expr);

TreeNode *GetT(Variables *vars, Expr *expr);

TreeNode* GetN(Variables *vars, Expr *expr);

TreeNode *GetId(Variables *vars, Expr *expr);

TreeNode *GetA(Variables *vars, Expr *expr);

TreeNode *GetO(Variables *vars, Expr *expr);


#endif
