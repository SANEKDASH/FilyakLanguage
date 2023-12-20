#ifndef BACKEND_HEADER
#define BACKEND_HEADER

#include "../Common/trees.h"
#include "../Common/NameTable.h"

typedef enum
{
    kBackendSuccess,
    kMissingKeywordCode,
} BackendErrs_t;

//доработать

TreeErrs_t MakeAsmCode(LanguageElems *l_elems,
                       const char    *output_file_name);

#endif
