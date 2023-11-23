#ifndef AKINATOR_DEBUG_H_
#define AKINATOR_DEBUG_H_

#include <limits.h>

#include "stack.h"
#include "tree.h"

enum TreeErrors {
    TREE_OK = 0,
    TREE_CHILD_OVERLAP,
    TREE_HAS_CYCLE,
    TREE_CHECK_FAILURE,
};

struct TreeState {
    TreeErrors error;
    const struct Tree *wrong_parent;
    const struct Tree *wrong_child;
};

struct TreeState CheckNode(const struct Tree *tree, Stack *stk);

inline struct TreeState CheckTree(const struct Tree *tree)
{
    Stack stk = {};
    StackCtor(&stk);
    TreeState state = CheckNode(tree, &stk);
    StackDtor(&stk);
    return state;
}

struct CallPosition {
    const char *file;
    const char *func;
    size_t line;
    const char *varname;
};

const size_t MAX_LOG_PATH_SIZE = 1024;

struct DotFile {
    FILE *stream;
    char name[MAX_LOG_PATH_SIZE];
};

struct DotFile CreateDotFile();

void RunDot(struct DotFile dot);

void DumpTree(const struct Tree *tree, struct TreeState state,
              const char *filename, struct CallPosition pos);

#ifndef NDEBUG
#define DUMP_TREE(tree) \
    do { \
        TreeState state = CheckTree(tree); \
        DumpTree(tree, state, "log.html", {__FILE__, __func__, \
                 __LINE__, #tree}); \
    } while (0)

#define TREE_ASSERT(tree) \
    do { \
        TreeState state = CheckTree(tree); \
        if (state.error != TREE_OK) { \
            DumpTree(tree, state, "log.html", \
                     {__FILE__, __func__, __LINE__, #tree}); \
            fprintf(stderr, "CheckTree failed: %s:%s:%d\n", \
                            __FILE__, __func__, __LINE__); \
            abort(); \
		} \
    } while (0)
#else
#define DUMP_TREE(tree)

#define TREE_ASSERT(tree)
#endif

#endif
