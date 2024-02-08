#include <stdio.h>
#include "parse.h"
#include "../Common/trees.h"
#include "../Common/tree_dump.h"
#include "TXlib.h"

int main(int argc, char *argv[])
{
    txSetLocale();
    txDisableAutoPause();

    InitTreeGraphDump();

    LanguageElems l_elems = {0};

    LanguageElemsInit(&l_elems);

    if (argc < 2)
    {
        printf(">>FRONTEND: you must put an arg \"Frontend <file_name>\"\n");

        return 0;
    }

    l_elems.syntax_tree.root = GetSyntaxTree(&l_elems.vars, argv[1]);

    GRAPH_DUMP_TREE(&l_elems.syntax_tree);

    EndTreeGraphDump();

    if (PrintTreeInFile(&l_elems, "tree_save.txt") != kTreeSuccess)
    {
        printf(">>Я не буду компилить, долбооеоооб.\n");

        LanguageElemsDtor(&l_elems);

        return -1;
    }

    LanguageElemsDtor(&l_elems);

    printf(">>Так уж и быть, скомпилю тебе это дерьмо: \"%s\".\n", argv[1]);

    return 0;
}
