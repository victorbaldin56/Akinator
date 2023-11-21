#include "debug.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "dot.h"
#include "tree.h"

const size_t TIMESTAMP_MAX = 16;

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
    char image_filename[PATH_MAX] = {};
    ImageDump(tnode, image_filename);
    fclose(fp);
}

void ImageDump(const struct TreeNode *tnode, char *image_filename)
{
    assert(image_filename);
    char filename[PATH_MAX] = {};
    time_t now = time(NULL);
    char timestamp[TIMESTAMP_MAX] = {};
    strftime(timestamp, sizeof(timestamp), "%Y_%m_%d_%H:%M:%S", gmtime(&now));
    snprintf(filename, sizeof(filename), "%s.dot", timestamp);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("ImageDump");
        return;
    }
    DotBegin(fp);
    DotDump(tnode, fp);
    DotEnd(fp);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    char *cmd = (char *)calloc(sysconf(_SC_ARG_MAX), sizeof(*cmd));

    // Creates output file name and command for GraphViz
    snprintf(image_filename, PATH_MAX, "%s.png", timestamp);
    snprintf(cmd, sysconf(_SC_ARG_MAX), "dot -T png %s -o %s",
                                        filename, image_filename);
#pragma GCC diagnostic pop
    system(cmd);
    free(cmd);
}

void DotDump(const struct TreeNode *tnode, FILE *stream)
{
    assert(stream);
    if (!tnode)
        return;

    fprintf(stream, "tnode%p [shape = Mrecord, label = \"" TREE_TYPE_FMT
                    " {%p | %p} \"];\n",
                    tnode, tnode->data, tnode->left, tnode->right);
    DotDump(tnode->left, stream);
    DotDump(tnode->right, stream);
    fprintf(stream, "tnode%p -> tnode%p; tnode%p -> tnode%p;\n",
                    tnode, tnode->left, tnode, tnode->right);
}
