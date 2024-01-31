#ifndef TEXT_PARSE_HEADER
#define TEXT_PARSE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <sys/stat.h>

typedef enum
{
    kSuccess        = 0,
    kOpenError      = 1,
    kAllocError     = 2,
    kFreeError      = 3,
    kCloseError     = 4,
    kReadingError   = 5,
    kReallocError   = 6,
    kBufferOverflow = 7,
    kEOF            = 8,
} TextErrs_t;

struct Line
{
    char   *str;
    size_t  real_line_number;
};

struct Text
{
    Line    *lines_ptr;
    size_t   lines_count;
    char    *buf;
    size_t   buf_size;
};

void SkipSpaces(char **line);

size_t GetFileSize(FILE *ptr_file);

TextErrs_t ReadTextFromFile(Text       *text,
                            const char *file_name);

TextErrs_t ReadWordsFromFile(Text       *text,
                             const char *file_name);

size_t SplitBufIntoWords(char *buf);

size_t SplitBufIntoLines(char *buf);

void FillText(Text *text);

void TextDtor(Text *text);

void PrintTextInFile(FILE *output_file,
                     Text *text);

TextErrs_t GetStr(char   *command_string,
                  size_t  max_size);

#endif
