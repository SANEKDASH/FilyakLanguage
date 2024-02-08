#include <stdio.h>
#include <TXLib.h>

#include "reverse_frontend.h"

int main(int argc, char *argv[])
{
    txSetLocale();
    txDisableAutoPause();

    LanguageElems l_elems = {0};

    if (argc < 3)
    {
        printf(">>Никак же вы блять не научитесь\n"
               "  ReverseFrontend call looks like: ReverseFrontend <name_of_tree_file> <name_of_id_table_file>\n");

        return -1;
    }

    ReadLanguageElemsOutOfFile(&l_elems, argv[1], argv[2]);

    if (ReverseFrontend(&l_elems, argv[3]) != 0)
    {
        printf("You are dumb\n");

        return -1;
    }

    return 0;
}
