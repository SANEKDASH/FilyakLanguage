#include <stdio.h>

#include "parse.h"
#include "../Common/trees.h"
#include "../Common/tree_dump.h"

int main(int argc, char *argv[])
{
    InitTreeGraphDump();

    LanguageElems l_elems = {0};

    LanguageElemsInit(&l_elems);

    if (argc < 2)
    {
        printf(">>FRONTEND: you must put an arg \"Frontend <file_name>\"\n");

        return 0;
    }

    l_elems.syntax_tree.root = GetG(&l_elems.vars, argv[1]);


    GRAPH_DUMP_TREE(&l_elems.syntax_tree);
    printf("TREE ROOT %p\n", l_elems.syntax_tree.root);

    EndTreeGraphDump();

    PrintTreeInFile(&l_elems.syntax_tree,
                    &l_elems.vars,
                    "tree_save.txt");

    LanguageElemsDtor(&l_elems);

    return 0;
}
