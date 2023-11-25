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

const size_t MAX_NAME_SIZE = 1024;

static AkinatorModes AskMode();

const int GO_DIDNT_GUESS = -1;

static int GuessObject(struct Tree *root, struct Tree *tree,
                       const char *pathname);

static bool IsGuessed(struct Tree *tree);

static void AddNewObject(struct Tree *root, struct Tree *tree,
                         struct Tree *old_obj, const char *pathname);

static ssize_t ReadInput(char *str, size_t size, FILE *input);

static void AddToTree(struct Tree *tree, struct Tree *old_obj,
                      const char *new_obj_name, const char *obj_diff_name);

static void EnterDefinitionMode(struct Tree *tree);

static void AskObjectName(char *obj_name);

const int DO_OBJ_NOT_FOUND = -1;

static int DefineObject(struct Tree *tree, const char *obj_name);

static bool IsFound(struct Tree *tree, const char *obj_name, Stack *stk);

static void PrintObjectDefinition(struct Tree *tree, const char *obj_name,
                                  const Stack *stk);

static void PrintObjectStack(struct Tree *tree, const Stack *stk, ssize_t count);

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
    switch (AskMode()) {
        case AK_GUESS: {
            GuessObject(rtres.tree, rtres.tree, pathname);
            break;
        }
        case AK_DEFINE: {
            EnterDefinitionMode(rtres.tree);
            break;
        }
        case AK_ABORT: {
            break;
        }
        default: {
            assert(0 && "Unhandled enum value");
        }
    }
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

static AkinatorModes AskMode()
{
    printf("Выберите режим работы: определить [d], угадать [g]: ");
    int ch = getchar();
    getchar();
    switch(ch) {
        case 'd': {
            return AK_DEFINE;
        }
        case 'g': {
            return AK_GUESS;
        }
        default: {
            printf("Неизвестная команда.\n");
            return AK_ABORT;
        }
    }
}

static int GuessObject(struct Tree *root, struct Tree *tree,
                       const char *pathname)
{
    TREE_ASSERT(root);
    assert(pathname);
    if (!tree) {
        printf("Легчайше\n");
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
        case EOF:
            return false;
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
    char new_obj_name[MAX_NAME_SIZE] = {};
    if (ReadInput(new_obj_name, sizeof(new_obj_name) - 1, stdin) == EOF) // FIXME
        return;

    printf("Чем %s отличается от %s? ", new_obj_name, old_obj->data);
    char obj_diff_name[MAX_NAME_SIZE] = {};
    if (ReadInput(obj_diff_name, sizeof(obj_diff_name) - 1, stdin) == EOF) // FIXME
        return;

    AddToTree(tree, old_obj, new_obj_name, obj_diff_name);
    FILE *output = fopen(pathname, "w");
    if (!output) {
        perror(""); // FIXME error code
        return;
    }
    printf("База пополнена, спасибо за предоставленные сведения!\n");
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

static void AskObjectName(char *obj_name)
{
    assert(obj_name);
    printf("Введите имя объекта: ");
    if (ReadInput(obj_name, MAX_NAME_SIZE, stdin) == EOF)
        return;
}

static void EnterDefinitionMode(struct Tree *tree)
{
    TREE_ASSERT(tree);
    char obj[MAX_NAME_SIZE] = {};
    AskObjectName(obj);
    DefineObject(tree, obj);
}

static int DefineObject(struct Tree *tree, const char *obj_name)
{
    TREE_ASSERT(tree);
    assert(obj_name);

    Stack stk = {};
    StackCtor(&stk);
    if (!IsFound(tree, obj_name, &stk)) {
        StackDtor(&stk);
        return DO_OBJ_NOT_FOUND;
    }
    PrintObjectDefinition(tree, obj_name, &stk);
    StackDtor(&stk);
    return 0;
}

static bool IsFound(struct Tree *tree, const char *obj_name, Stack *stk)
{
    TREE_ASSERT(tree);
    STACK_ASS(stk);
    assert(obj_name);
    if (!tree)
        return false;

    if (strncmp(tree->data, obj_name, MAX_NAME_SIZE) == 0)
        return true;

    if (IsFound(tree->left, obj_name, stk)) {
        if (Push(stk, tree) == REALLOC_FAILED)
            return false;
        return true;
    }
    if (IsFound(tree->right, obj_name, stk)) {
        if (Push(stk, NULL) == REALLOC_FAILED)
            return false;
        return true;
    }
    return false;
}

static void PrintObjectDefinition(struct Tree *tree, const char *obj_name,
                                  const Stack *stk)
{
    STACK_ASS(stk);
    TREE_ASSERT(tree);
    assert(obj_name);
    printf("%s -- это: ", obj_name);
    PrintObjectStack(tree, stk, stk->size - 1);
}

static void PrintObjectStack(struct Tree *tree, const Stack *stk, ssize_t count)
{
    assert(count >= 0);
    assert(tree);
    if (count == 0) {
        if (stk->data[count])
            printf("%s\n", tree->data);
        else
            printf("не %s\n", tree->data);
        return;
    }
    if (stk->data[count]) {
        printf("%s, ", tree->data);
        PrintObjectStack(tree->left, stk, count - 1);
    } else {
        printf("не %s, ", tree->data);
        PrintObjectStack(tree->right, stk, count - 1);
    }
}
