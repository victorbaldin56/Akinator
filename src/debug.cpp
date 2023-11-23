#include "debug.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "tree.h"

#define PATH_TO_LOGS "logs/"

// Linear search of tree node in stack to check for cycling in tree
static inline bool InStack(Elem_t value, const Stack *stk);

struct TreeState CheckNode(const struct Tree *tree, Stack *stk)
{
    STACK_ASS(stk);
    if (!tree) {
        return {TREE_OK};
    }

    if (Push(stk, tree) == REALLOC_FAILED) {
        return {TREE_CHECK_FAILURE};
    }

    if (InStack(tree->left, stk)) {
        return {TREE_HAS_CYCLE, tree, tree->left};
    }

    if (InStack(tree->right, stk)) {
        return {TREE_HAS_CYCLE, tree, tree->right};
    }

    if (tree->left != NULL && tree->left == tree->right) {
        return {TREE_CHILD_OVERLAP, tree, tree->left};
    }

    TreeState left_state  = CheckNode(tree->left, stk);
    TreeState right_state = CheckNode(tree->right, stk);
    if (left_state.error != TREE_OK)
        return left_state;
    if (right_state.error != TREE_OK)
        return right_state;

    return {TREE_OK, NULL};
}

static inline bool InStack(Elem_t value, const Stack *stk)
{
    for (ssize_t i = 0; i < stk->size; i++) {
        if (value == stk->data[i])
            return true;
    }
    return false;
}

static inline void DumpHeader(struct TreeState state, FILE *output,
                               struct CallPosition pos);

static void DumpState(struct TreeState state, FILE *output);

static inline void DumpToDot(const struct Tree *tree, struct TreeState state,
                             struct DotFile dot);

static void DumpNode(const struct Tree *tree, TreeState state,
                     struct DotFile dot);

// TODO release
void DumpTree(const struct Tree *tree, struct TreeState state,
              const char *filename, struct CallPosition pos)
{
    assert(filename);
    assert(pos.file && pos.func);
    system("mkdir -p " PATH_TO_LOGS);

    char full_path[PATH_MAX] = {};
    snprintf(full_path, sizeof(full_path), PATH_TO_LOGS "%s", filename);
    FILE *output = fopen(full_path, "a+");
    if (!output) {
        perror("DumpTree");
        return;
    }
    DumpHeader(state, output, pos);
    struct DotFile dot = CreateDotFile();
    if (!dot.stream) {
        perror("CreateLogFile");
        return;
    }
    DumpToDot(tree, state, dot);
    RunDot(dot);
    fprintf(output, "<img\n"
                    "   src = %s.png\n"
                    "/>\n", dot.name + sizeof(PATH_TO_LOGS) - 1);
    fclose(output);
}

const size_t MAX_TIMESTAMP_SIZE = 30;

// FIXME direct copying
struct DotFile CreateDotFile()
{
    DotFile dot = {};
    time_t now = time(NULL);
    char timestamp[MAX_TIMESTAMP_SIZE] = {};
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", gmtime(&now));
    struct timespec ts = {};
    timespec_get(&ts, TIME_UTC);
    snprintf(dot.name, sizeof(dot.name), PATH_TO_LOGS "%s.%ld",
                                         timestamp, ts.tv_nsec);
    dot.stream = fopen(dot.name, "w");
    return dot;
}

static inline void DumpHeader(struct TreeState state, FILE *output,
                               struct CallPosition pos)
{
    assert(output);
    assert(pos.file && pos.func && pos.varname);
    fprintf(output, "<h2>Tree <code>%s</code> from file"
                    " %s, function <code>%s</code>, line %zu</h2>\n",
                    pos.varname, pos.file, pos.func, pos.line);
    DumpState(state, output);
}

static void DumpState(struct TreeState state, FILE *output)
{
    assert(output);
    switch (state.error) {
        case TREE_OK: {
            fprintf(output, "<font color = green>"
                            "No errors detected, tree is OK!\n"
                            "</font>");
            return;
        }
        case TREE_CHILD_OVERLAP: {
            fprintf(output, "<font color = red>"
                            "ERROR in node %p: left = right\n"
                            "</font>", state.wrong_parent);
            return;
        }
        case TREE_HAS_CYCLE: {
            fprintf(output, "<font color = red>"
                            "ERROR in node %p: tree has cycle\n"
                            "</font>", state.wrong_parent);
            return;
        }
        case TREE_CHECK_FAILURE: {
            fprintf(output, "Could not check tree due to memory limit\n");
            return;
        }
        default: {
            assert(0 && "Unhandled enum value");
        }
    }
}

static inline void DumpToDot(const struct Tree *tree, struct TreeState state,
                             struct DotFile dot)
{
    assert(dot.stream && dot.name);
    if (!tree)
        return;

    fprintf(dot.stream, "digraph G {\n");
    DumpNode(tree, state, dot);
    fprintf(dot.stream, "}\n");
}

static inline void PrintDotNode(const struct Tree *tree, struct DotFile dot,
                                const char *color)
{
    assert(dot.stream && dot.name);
    fprintf(dot.stream, "    t%p[shape = Mrecord, style = filled, "
                        "fillcolor = %s, label = \"%s\"];\n",
                        tree, color, tree->data);
}

static void DumpNode(const struct Tree *tree, struct TreeState state,
                     struct DotFile dot)
{
    assert(dot.stream && dot.name);
    if (!tree)
        return;

    if (tree == state.wrong_parent) {
        PrintDotNode(tree, dot, "red");
        if (state.error == TREE_HAS_CYCLE) {
            fprintf(dot.stream, "    t%p->t%p\n", tree, state.wrong_child);
            return;
        }
    } else {
        PrintDotNode(tree, dot, "cyan");
    }

    DumpNode(tree->left, state, dot);
    DumpNode(tree->right, state, dot);
    if (tree->left)
        fprintf(dot.stream, "    t%p->t%p[weight = 1000]\n", tree, tree->left);
    if (tree->right)
        fprintf(dot.stream, "    t%p->t%p[weight = 1000]\n", tree, tree->right);
}

const size_t MAX_COMMAND_SIZE = 10000;

void RunDot(struct DotFile dot)
{
    assert(dot.stream && dot.name);
    fclose(dot.stream);
    char *cmd = (char *)calloc(MAX_COMMAND_SIZE, sizeof(*cmd));
    snprintf(cmd, MAX_COMMAND_SIZE, "dot -T png %s -o %s.png",
                                    dot.name, dot.name);
    system(cmd);
    free(cmd);
}
