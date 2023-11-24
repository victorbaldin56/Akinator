#include "tree.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

struct Tree *TreeCtor(const char *name, struct Tree *left,
                      struct Tree *right)
{
    struct Tree *node = (struct Tree *)calloc(1, sizeof(*node));
    if (!node)
        return NULL;

    *node = {name, left, right};
    return node;
}

void TreeDtor(struct Tree *tree)
{
    TREE_ASSERT(tree);
    if (!tree)
        return;

    TreeDtor(tree->left);
    TreeDtor(tree->right);
    free(tree);
}

void PrintTree(FILE *output, struct Tree *tree)
{
    assert(output);
    TREE_ASSERT(tree);
    if (!tree) {
        fprintf(output, "()\n");
        return;
    }
    fprintf(output, "(%s\n", tree->data);
    PrintTree(output, tree->left);
    PrintTree(output, tree->right);
    fprintf(output, "\n)");
}

static struct ReadTreeResult ParseBaseBuffer(char **bufp);

struct ReadTreeResult ReadTree(char *buf)
{
    return ParseBaseBuffer(&buf);
}

// FIXME
static struct ReadTreeResult ParseBaseBuffer(char **bufp)
{
    assert(bufp && *bufp);
    *bufp = strchr(*bufp, '(');
    if (!*bufp)
        return {RT_BAD_SYNTAX};

    **bufp = '\0';
    if (*++*bufp == ')')
        return {RT_OK};

    struct Tree *tree = TreeCtor(*bufp, NULL, NULL);
    if (!tree)
        return {RT_BAD_ALLOC};

    struct ReadTreeResult left_res = ParseBaseBuffer(bufp);
    tree->left = left_res.tree;
    if (left_res.error) {
        TreeDtor(tree);
        return left_res;
    }
    struct ReadTreeResult right_res = ParseBaseBuffer(bufp);
    tree->right = right_res.tree;
    if (right_res.error) {
        TreeDtor(tree);
        return right_res;
    }
    return {RT_OK, tree};
}
