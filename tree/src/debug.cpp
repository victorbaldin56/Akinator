#include "debug.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tree.h"

TreeErrors CheckTree(const struct TreeNode *tnode)
{
    if (!tnode)
        return TREE_OK;
    if (tnode->left != NULL && tnode->left == tnode->right)
        return TREE_LEFT_RIGHT_EQUAL;
    if (tnode->left == tnode || tnode->right == tnode)
        return TREE_SELF_REFERENCED;

    return TREE_OK;
}

void TreeDump(const struct TreeNode *tnode, TreeErrors error_state,
              const char *filename, const char *func, size_t line)
{
    system("mkdir -p logs");
    FILE *fp = fopen("logs/dump.html", "w+");
    if (!fp) {
        perror("TreeDump");
        return;
    }
    fprintf(fp, "TreeDump called from function %s, file %s:%zu",
                func, filename, line);
    fprintf(fp, "ERROR %d", error_state);
    char png_filename[PATH_MAX] = {};
    TreePNGDump(tnode, png_filename);
    fclose(fp);
}

void TreePNGDump(const struct TreeNode *tnode, char *png_filename)
{
    assert(png_filename);
    char filename[PATH_MAX] = {};
    time_t now = time(NULL);
    strftime(filename, sizeof(filename), "%Y-%M-%D_%H:%M:%S.dot", gmtime(&now));
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("TreePNGDump");
        return;
    }
    DotDump(tnode, fp);
    fclose(fp);
    char *cmd = (char *)calloc(sysconf(_SC_ARG_MAX), sizeof(*cmd));

    // Creates output name and command for GraphViz
    snprintf(png_filename, PATH_MAX, "%s.png", filename);
    snprintf(cmd, sysconf(_SC_ARG_MAX), "dot -T png %s -o %s", filename, png_filename);
    system(cmd);
    free(cmd);
}

void DotDump(const struct TreeNode *tnode, FILE *stream)
{
    assert(stream);
    if (!tnode)
        return;

    fprintf(stream, "tnode%p [shape = Mrecord, label = \"" PRI_TREE_TYPE
                    " | %p | %p \"];\n", tnode, tnode->data, tnode->left, tnode->right);
    DotDump(tnode->left, stream);
    DotDump(tnode->right, stream);
}
