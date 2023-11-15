#ifndef AKINATOR_TREE_DEBUG_H_
#define AKINATOR_TREE_DEBUG_H_

#include <stdio.h>

enum TreeErrors {
    TREE_OK = 0,
    TREE_LEFT_RIGHT_EQUAL,
    TREE_SELF_REFERENCED,
};

void TreeDump(const struct TreeNode *tnode, TreeErrors error_state,
              const char *filename, const char *func, size_t line);

TreeErrors CheckTree(const struct TreeNode *tnode);

void TreePNGDump(const struct TreeNode *tnode, char *png_filename);

void DotDump(const struct TreeNode *tnode, FILE *stream);

#endif
