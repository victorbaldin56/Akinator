#ifndef AKINATOR_TREE_TREE_H_
#define AKINATOR_TREE_TREE_H_

#include <stddef.h>
#include <stdio.h>

// Type-dependent things
typedef int TreeType;
#define TREE_TYPE_FMT "%d"

struct TreeNode {
    TreeType data;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode *TreeNodeCtor();

void TreeNodeDtor(struct TreeNode *tnode);

struct TreeNode *LeftCtor(struct TreeNode *tnode);

struct TreeNode *RightCtor(struct TreeNode *tnode);

void PostfixPrintTree(FILE *stream, const struct TreeNode *tnode);

void PrefixPrintTree(FILE *stream, const struct TreeNode *tnode);

void InfixPrintTree(FILE *stream, const struct TreeNode *tnode);

enum ReadTreeErrors {
    RT_BAD_ALLOC    = -2,
    RT_SYNTAX_ERROR = -1,
    RT_SUCCESS      =  0,
};

struct ReadTreeRes {
    ReadTreeErrors error_state;
    struct TreeNode *tnode;
};

struct ReadTreeRes PrefixReadTree(FILE *input);

#endif
