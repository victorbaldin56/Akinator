#ifndef AKINATOR_TREE_TREE_H_
#define AKINATOR_TREE_TREE_H_

#include <stddef.h>
#include <stdio.h>

struct Tree {
    const char *data;
    struct Tree *left;
    struct Tree *right;
#ifndef NDEBUG
    bool is_visited; ///< For verificator
#endif
};

struct Tree *TreeCtor(const char *name, struct Tree *left, struct Tree *right);

void TreeDtor(struct Tree *tree);

void PrintTree(FILE *output, struct Tree *tree);

enum ReadTreeErrors {
    RT_OK         = 0,
    RT_BAD_ALLOC  = 1,
    RT_BAD_SYNTAX = 2,
};

struct ReadTreeResult {
    ReadTreeErrors error;
    struct Tree *tree;
};

struct ReadTreeResult ReadTree(char *buf);

#endif
