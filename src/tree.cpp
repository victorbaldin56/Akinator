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

void PrintTree(FILE *output, const struct Tree *tree)
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

char *LoadFile(FILE *input)
{
    assert(input);
    fseek(input, 0L, SEEK_END);
    long size = ftell(input);
    fseek(input, 0L, SEEK_SET);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    char *buf = (char *)calloc(size + 1, sizeof(*buf));
    if (!buf)
        return NULL;

    fread(buf, size, sizeof(*buf), input);
#pragma GCC diagnostic pop
    return buf;
}

struct ReadResult ReadTree(char **buf)
{
    assert(buf && *buf);
    *buf = strchr(*buf, '(');
    if (!buf)
        return {RT_BAD_SYNTAX};

    **buf = '\0';
    if (*++*buf == ')')
        return {RT_OK};

    struct Tree *tree = TreeCtor(*buf, NULL, NULL);
    if (!tree)
        return {RT_BAD_ALLOC};

    struct ReadResult left_res = ReadTree(buf);
    tree->left = left_res.tree;
    if (left_res.error)
        return left_res;

    struct ReadResult right_res = ReadTree(buf);
    tree->right = right_res.tree;
    if (right_res.error)
        return right_res;

    return {RT_OK, tree};
}
