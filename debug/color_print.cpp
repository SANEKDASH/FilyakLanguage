#include <stdarg.h>
#include <windows.h>
#include <stdio.h>

#include "color_print.h"


void SetColor(Colors color_code)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (short unsigned int) color_code);
}

void ColorPrintf(Colors color_code, const char *fmt, ...)
{
    va_list arg_list;
    va_start(arg_list, fmt);

    SetColor(color_code);

    vprintf(fmt, arg_list);

    SetColor(kWhite);

    va_end(arg_list);
}

