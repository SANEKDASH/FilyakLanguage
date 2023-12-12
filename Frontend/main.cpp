#include <stdio.h>

#include "parse.h"
#include "../trees.h"
#include "../tree_dump.h"

int main(int argc, char *argv[])
{
    InitTreeGraphDump();

    LanguageElems l_elems = {0};

    Expr expr ;
    expr.string = "pidr + sin(huy)";
    expr.pos = 0;
    l_elems.syntax_tree.root = GetG(&l_elems.vars, &expr);

    GRAPH_DUMP_TREE(&l_elems.syntax_tree);

    //ReadLanguageElemsOutOfFile(&l_elems, argv[1]);
    LanguageElemsDtor(&l_elems);

    EndTreeGraphDump();

    return 0;
}
