#include <stdio.h>

#include <assert.h>
#include <stdlib.h>

#include "debug.h"
#include "tree.h"

int main()
{
	struct Tree *tree = TreeCtor("root", NULL, NULL);
	struct Tree *left = TreeCtor("left child", NULL, NULL);
	struct Tree *right = TreeCtor("right child", NULL, NULL);
	tree->left = left;
	tree->right = right;

	struct Tree *leftleft = TreeCtor("left child of left child", NULL, NULL);
	tree->left->left = leftleft;
	struct Tree *leftright = TreeCtor("right child of left child", NULL, NULL);
	tree->left->right = leftright;

	struct Tree *rightleft = TreeCtor("aboba", NULL, NULL);
	struct Tree *rightright = TreeCtor("ded", NULL, NULL);
	tree->right->right = rightright;
	tree->right->left = rightleft;

	struct Tree *subtree = TreeCtor("some shit here", NULL, NULL);
	leftleft->right = subtree;
	DUMP_TREE(tree);
	FILE *fp = fopen("tests/base.txt", "w");
	PrintTree(fp, tree);
	fclose(fp);
	TreeDtor(tree);

	fp = fopen("tests/base.txt", "r");
	char *buf = LoadFile(fp);
    char *origbuf = buf;
	struct ReadResult res = ReadTree(&buf);
	struct Tree *new_tree = res.tree;
	DUMP_TREE(new_tree);
	new_tree->left->right = new_tree;
	TreeDtor(new_tree);
	free(origbuf);
	return 0;
}
