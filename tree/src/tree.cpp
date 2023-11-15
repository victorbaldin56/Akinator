#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

#ifndef NDEBUG
#define TREE_ASSERT(tnode) \
    do { \
        TreeErrors error_state = CheckTree(tnode); \
        if (error_state != TREE_OK) { \
            TreeDump(tnode, error_state, __FILE__, __func__, __LINE__); \
            abort(); \
		} \
    } while (0)
#else
#define TREE_ASSERT(tnode)
#endif

struct TreeNode *TreeNodeCtor()
{
    return (struct TreeNode *)calloc(1, sizeof(struct TreeNode));
}

void TreeNodeDtor(struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    if (!tnode)
        return;

    TreeNodeDtor(tnode->left);
    TreeNodeDtor(tnode->right);
    tnode->left = tnode->right;
    free(tnode);
}

struct TreeNode *LeftCtor(struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    if (!tnode)
        return NULL;

    struct TreeNode *left = TreeNodeCtor();
    tnode->left = left;
    return tnode->left;
}

struct TreeNode *RightCtor(struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    if (!tnode) {
        printf("nil ");
        return NULL;
    }
    struct TreeNode *right = TreeNodeCtor();
    tnode->right = right;
    return tnode->right;
}

void PostfixPrintTree(const struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    if (!tnode) {
        printf("nil ");
        return;
    }
    printf("(");
    PostfixPrintTree(tnode->left);
    PostfixPrintTree(tnode->right);
    printf(PRI_TREE_TYPE, tnode->data);
    printf(")");
	return;
}

void PrefixPrintTree(const struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    if (!tnode) {
        printf(" nil");
        return;
    }
    printf(PRI_TREE_TYPE, tnode->data);
    printf("(");
    PrefixPrintTree(tnode->left);
    PrefixPrintTree(tnode->right);
    printf(")");
	return;
}

void InfixPrintTree(const struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    if (!tnode) {
        printf("nul");
        return;
    }
    printf("(");
    InfixPrintTree(tnode->left);
    printf(")");
    printf(PRI_TREE_TYPE, tnode->data);
    printf("(");
    InfixPrintTree(tnode->right);
    printf(")");
    return;
}

#undef TREE_ASSERT
