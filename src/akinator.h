#ifndef AKINATOR_AKINATOR_H_
#define AKINATOR_AKINATOR_H_

enum LoadFileErrors {
    LF_OK               =  0,
    LF_FILE_NOT_FOUND   = -1,
    LF_FILE_PERM_DENIED = -2,
    LF_BAD_ALLOC        = -3,
};

enum AkinatorModes {
    AK_GUESS,
    AK_DEFINE,
    AK_COMPARE,
    AK_ABORT,
};

int ExecProcess(const char *filename);

#endif
