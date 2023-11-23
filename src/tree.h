#ifndef AKINATOR_TREE_TREE_H_
#define AKINATOR_TREE_TREE_H_

#include <stddef.h>
#include <stdio.h>

struct Tree {
    const char *data;
    struct Tree *left;
    struct Tree *right;
};

struct Tree *TreeCtor(const char *name, struct Tree *left, struct Tree *right);

void TreeDtor(struct Tree *tree);

void PrintTree(FILE *output, const struct Tree *tree);

enum ReadErrors {
    RT_OK = 0,
    RT_BAD_ALLOC,
    RT_BAD_SYNTAX,
};

char *LoadFile(FILE *input);

struct ReadResult {
    ReadErrors error;
    struct Tree *tree;
};

struct ReadResult ReadTree(char **buf);

#endif
