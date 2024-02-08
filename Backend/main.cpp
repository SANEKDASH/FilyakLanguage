#include <stdio.h>

#include "backend.h"
#include "../Common/tree_dump.h"
#include "../Common/trees.h"

int main(int argc, char *argv[])
{
    InitTreeGraphDump();

    LanguageElems l_elems = {0};

    LanguageElemsInit(&l_elems);

    ReadLanguageElemsOutOfFile(&l_elems, argv[1], argv[2]);

    MakeAsmCode(&l_elems, "assembled.asm");

    EndTreeGraphDump();

    return 0;
}
