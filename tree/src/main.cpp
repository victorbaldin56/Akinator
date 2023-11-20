#include <stdio.h>

#include <assert.h>

#include "debug.h"
#include "tree.h"

int main()
{
	FILE *input = fopen("tests/input.txt", "r");
	if (!input) {
		perror("");
		return -1;
	}

	struct ReadTreeRes read_res = PrefixReadTree(input);
	switch (read_res.error_state) {
		case RT_BAD_ALLOC: {
			fprintf(stderr, "allocation failed\n");
			return -1;
		}
		case RT_SYNTAX_ERROR: {
			fprintf(stderr, "syntax error\n");
			return -1;
		}
		case RT_SUCCESS: {
			fprintf(stderr, "success\n");
			break;
		}
		default: {
			assert(0);
		}
	}
	FILE *output = fopen("tests/output.txt", "w");
	if (!output) {
		perror("");
		return -1;
	}
	PrefixPrintTree(output, read_res.tnode);
	TreeNodeDtor(read_res.tnode);
	return 0;
}

