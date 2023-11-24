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

const size_t MAX_OBJECT_NAME_SIZE = 1024;
const int GO_DIDNT_GUESS = -1;

static int GuessObject(struct Tree *root, struct Tree *tree,
                       const char *pathname);

static bool IsGuessed(struct Tree *tree);

static void AddNewObject(struct Tree *root, struct Tree *tree,
                         struct Tree *old_obj, const char *pathname);

static ssize_t ReadInput(char *str, size_t size, FILE *input);

static void AddToTree(struct Tree *tree, struct Tree *old_obj,
                      const char *new_obj_name, const char *obj_diff_name);

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
    DUMP_TREE(rtres.tree);
    GuessObject(rtres.tree, rtres.tree, pathname);
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
    fclose(input);
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

static int GuessObject(struct Tree *root, struct Tree *tree,
                       const char *pathname)
{
    TREE_ASSERT(root);
    assert(pathname);
    if (!tree) {
        printf("Это было тривиально\n");
        return 0;
    }
    bool is_guessed = IsGuessed(tree);
    struct Tree *next = NULL;
    if (is_guessed)
        next = tree->left;
    else
        next = tree->right;

    if (!next && !is_guessed)
        return GO_DIDNT_GUESS;

    if (GuessObject(root, next, pathname) == GO_DIDNT_GUESS)
        AddNewObject(root, tree, next, pathname);

    return 0;
}

static bool IsGuessed(struct Tree *tree)
{
    TREE_ASSERT(tree);
    if (!tree)
        return false;

    assert(tree->data);
    printf("Это %s? [Y/n] ", tree->data);
    int ch = getchar();
    getchar();
    switch (ch) {
        case 'Y':
        case 'y':
            return true;
        default:
            return false;
    }
}

static void AddNewObject(struct Tree *root, struct Tree *tree,
                         struct Tree *old_obj, const char *pathname)
{
    assert(tree && old_obj);
    TREE_ASSERT(root);
    assert(pathname);

    printf("Что это? ");
    char new_obj_name[MAX_OBJECT_NAME_SIZE] = {};
    if (ReadInput(new_obj_name, sizeof(new_obj_name) - 1, stdin) == EOF) // FIXME
        return;

    printf("Чем %s отличается от %s? ", new_obj_name, old_obj->data);
    char obj_diff_name[MAX_OBJECT_NAME_SIZE] = {};
    if (ReadInput(obj_diff_name, sizeof(obj_diff_name) - 1, stdin) == EOF) // FIXME
        return;

    AddToTree(tree, old_obj, new_obj_name, obj_diff_name);
    FILE *output = fopen(pathname, "w");
    if (!output) {
        perror(""); // FIXME error code
        return;
    }
    DUMP_TREE(root);
    PrintTree(output, root);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

static ssize_t ReadInput(char *str, size_t size, FILE *input)
{
    assert(str);
    assert(input);
    size_t i = 0;
    for ( ; i < size; i++) {
        int ch = fgetc(input);
        if (ch == '\n') {
            str[i] = '\0';
            return i;
        }
        if (ch == EOF)
            return EOF;
        str[i] = (char)ch;
    }
    str[size - 1] = '\0';
    return size;
}

#pragma GCC diagnostic pop

static void AddToTree(struct Tree *tree, struct Tree *old_obj,
                      const char *new_obj_name, const char *obj_diff_name)
{
    assert(tree && old_obj && new_obj_name && obj_diff_name);
    struct Tree *new_obj  = TreeCtor(new_obj_name, NULL, NULL); // FIXME alloc
    struct Tree *obj_diff = TreeCtor(obj_diff_name, new_obj, old_obj);
    if (old_obj == tree->left)
        tree->left = obj_diff;
    else
        tree->right = obj_diff;
}
