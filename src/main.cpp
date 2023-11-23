#include <stdio.h>

#include <assert.h>
#include <stdlib.h>

#include "akinator.h"
#include "debug.h"
#include "tree.h"

enum CmdArgs {
    ARGS_NO,
    ARGS_FILE,
};

static CmdArgs ParseArgs(int argc, char **argv);

int main(int argc, char **argv)
{
    switch (ParseArgs(argc, argv)) {
        case ARGS_NO: {
            fprintf(stderr, "Not supported yet, please pass a file\n"); // FIXME
            return EXIT_FAILURE;
        }
        case ARGS_FILE: {
            return ExecProcess(argv[1]);
        }
        default: {
            assert(0 && "Unhandled enum value in switch");
        }
    }
}

// TODO help
// TODO version
static CmdArgs ParseArgs(int argc, char **argv)
{
    assert(argv);
    assert(argc > 0);
    if (argc == 1) {
        return ARGS_NO;
    }
    return ARGS_FILE;
}
