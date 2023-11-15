#include <stdio.h>

#include "debug.h"
#include "tree.h"

int main()
{
    TreeNode *tnode = TreeNodeCtor();
	LeftCtor(tnode);
	RightCtor(tnode);
	tnode->left->data = 10;
	tnode->right->data = 20;
	PostfixPrintTree(tnode);
	TreeNodeDtor(tnode);
	return 0;
}

