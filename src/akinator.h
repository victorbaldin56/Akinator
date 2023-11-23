#ifndef AKINATOR_AKINATOR_H_
#define AKINATOR_AKINATOR_H_

#include "tree.h"

enum LoadBaseErrors {
    LB_OK = 0,
    LB_FILE_NOT_FOUND,
    LB_BAD_ALLOC,
    LB_BAD_SYNTAX,
};

LoadBaseErrors LoadBase(char *filename);



#endif
