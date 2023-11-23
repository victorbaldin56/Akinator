#include "akinator.h"

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "debug.h"
#include "tree.h"

static LoadFileErrors LoadFile(const char *pathname, char **bufptr);

static void PrintLoadFileError(LoadFileErrors lf_error);

static void PrintReadTreeError(ReadTreeErrors rt_error);

static void TraverseTree(const struct Tree *tree);

static inline struct Tree *SelectNextNode(const struct Tree *tree);

// TODO guess
// TODO defintion
// TODO comparison
int ExecProcess(const char *pathname)
{
    assert(pathname);
    char *buf = NULL;
    LoadFileErrors lfres = LoadFile(pathname, &buf);
    if (lfres != LF_OK) {
        PrintLoadFileError(lfres);
        return lfres;
    }
    struct ReadTreeResult rtres = ReadTree(buf);
    if (rtres.error) {
        PrintReadTreeError(rtres.error);
        goto ret;
    }
    TraverseTree(rtres.tree);
    DUMP_TREE(rtres.tree);
    TreeDtor(rtres.tree);
ret:
    free(buf);
    return rtres.error;
}

static LoadFileErrors LoadFile(const char *pathname, char **bufptr)
{
    assert(pathname);
    assert(bufptr);
    struct stat statbuf = {};
    if (stat(pathname, &statbuf) == -1)
        return LF_FILE_NOT_FOUND;

    FILE *input = fopen(pathname, "r");
    if (!input)
        return LF_FILE_PERM_DENIED;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    *bufptr = (char *)calloc(statbuf.st_size + 1, sizeof(**bufptr));
    if (!*bufptr)
        return LF_BAD_ALLOC;

    fread(*bufptr, statbuf.st_size, sizeof(**bufptr), input);
#pragma GCC diagnostic pop
    return LF_OK;
}

static void PrintLoadFileError(LoadFileErrors lf_error)
{
    switch (lf_error) {
        case LF_OK: {
            return;
        }
        case LF_FILE_NOT_FOUND:
        case LF_FILE_PERM_DENIED: {
            perror("LoadFile");
            return;
        }
        case LF_BAD_ALLOC: {
            fprintf(stderr, "Could not load file: out of memory\n");
            return;
        }
        default: {
            assert(0 && "Unhandled enum value in switch statement");
        }
    }
}

static void PrintReadTreeError(ReadTreeErrors rt_error)
{
    switch (rt_error) {
        case RT_OK: {
            return;
        }
        case RT_BAD_ALLOC: {
            fprintf(stderr, "Could allocate tree: out of memory\n");
            return;
        }
        case RT_BAD_SYNTAX: {
            fprintf(stderr, "Syntax error in database file\n");
            return;
        }
        default: {
            assert(0 && "Unhandled enum value in switch\n");
        }
    }
}

static void TraverseTree(const struct Tree *tree)
{
    TREE_ASSERT(tree);
    if (!tree) {
        printf("I guessed successfully\n");
        return;
    }
    TraverseTree(SelectNextNode(tree));
}

static inline struct Tree *SelectNextNode(const struct Tree *tree)
{
    TREE_ASSERT(tree);
    if (!tree)
        return NULL;

    assert(tree->data);
    printf("%s? [Y/n] ", tree->data);
    switch (getchar()) {
        case 'Y':
        case 'y': {
            getchar();
            return tree->left;
        }
        default: {
            getchar();
            return tree->right;
        }
    }
}
