#include "dot.h"

#include <assert.h>

void DotBegin(FILE *stream)
{
    assert(stream);
    fprintf(stream, "digraph G {\nnode [shape = Mrecord];\n");
}

void DotEnd(FILE *stream)
{
    assert(stream);
    fprintf(stream, "}\n");
    fclose(stream);
}
