#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (!tnode)
        return NULL;

    struct TreeNode *right = TreeNodeCtor();
    tnode->right = right;
    return tnode->right;
}

void PostfixPrintTree(FILE *input, const struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    assert(input);
    if (!tnode) {
        fprintf(input, "nil ");
        return;
    }
    fprintf(input, "(");
    PostfixPrintTree(input, tnode->left);
    PostfixPrintTree(input, tnode->right);
    fprintf(input, TREE_TYPE_FMT, tnode->data);
    fprintf(input, ")");
	return;
}

void PrefixPrintTree(FILE *input, const struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    assert(input);
    if (!tnode) {
        fprintf(input, "(nil)");
        return;
    }
    fprintf(input, "(");
    fprintf(input, TREE_TYPE_FMT, tnode->data);
    PrefixPrintTree(input, tnode->left);
    PrefixPrintTree(input, tnode->right);
    fprintf(input, ")");
	return;
}

void InfixPrintTree(FILE *input, const struct TreeNode *tnode)
{
    TREE_ASSERT(tnode);
    assert(input);
    if (!tnode) {
        fprintf(input, "nil");
        return;
    }
    fprintf(input, "(");
    InfixPrintTree(input, tnode->left);
    fprintf(input, ")");
    fprintf(input, TREE_TYPE_FMT, tnode->data);
    fprintf(input, "(");
    InfixPrintTree(input, tnode->right);
    fprintf(input, ")");
    return;
}

struct ReadTreeRes PrefixReadTree(FILE *input)
{
    assert(input);
    int nread = 0;
    fscanf(input, "(%n", &nread);
    if (nread == 0)
        return {RT_SYNTAX_ERROR, NULL};

    nread = 0;
    fscanf(input, "nil)%n", &nread);
    if (nread > 0)
        return {RT_SUCCESS, NULL};

    struct TreeNode *tnode = TreeNodeCtor();
    if (fscanf(input, TREE_TYPE_FMT, &tnode->data) <= 0)
        return {RT_SYNTAX_ERROR, NULL};

    struct ReadTreeRes left_res  = PrefixReadTree(input);
    struct ReadTreeRes right_res = PrefixReadTree(input);
    if (left_res.error_state != RT_SUCCESS)
        return left_res;
    if (right_res.error_state != RT_SUCCESS)
        return right_res;

    tnode->left  = left_res.tnode;
    tnode->right = right_res.tnode;
    return {RT_SUCCESS, tnode};
}

#undef TREE_ASSERT
