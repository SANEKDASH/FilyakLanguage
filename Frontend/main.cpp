#include <stdio.h>

#include "parse.h"
#include "../Common/trees.h"
#include "../Common/tree_dump.h"

int main(int argc, char *argv[])
{
    InitTreeGraphDump();

    LanguageElems l_elems = {0};

    l_elems.syntax_tree.root = GetG(&l_elems.vars, argv[1]);

    GRAPH_DUMP_TREE(&l_elems.syntax_tree);

    LanguageElemsDtor(&l_elems);
    EndTreeGraphDump();

    return 0;
}
