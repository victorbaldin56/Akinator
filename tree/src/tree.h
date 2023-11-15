#ifndef AKINATOR_TREE_TREE_H_
#define AKINATOR_TREE_TREE_H_

#include <stddef.h>

// Type-dependent things
typedef int TreeType;
#define PRI_TREE_TYPE "%d"

struct TreeNode {
    TreeType data;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode *TreeNodeCtor();

void TreeNodeDtor(struct TreeNode *tnode);

struct TreeNode *LeftCtor(struct TreeNode *tnode);

struct TreeNode *RightCtor(struct TreeNode *tnode);

void PostfixPrintTree(const struct TreeNode *tnode);

void PrefixPrintTree(const struct TreeNode *tnode);

void InfixPrintTree(const struct TreeNode *tnode);

#endif
