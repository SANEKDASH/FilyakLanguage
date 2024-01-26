#include "text_parse.h"

static const int kSysCmdLen = 64;

//==============================================================================

void SkipSpaces(char **line)
{
    while ((**line == ' ' || **line == '\t') && **line != '\0')
    {
        *((*line)++) = '\0';
    }
}

//==============================================================================

void TextDtor(Text *text)
{
    free(text->buf);
    text->buf = nullptr;

    free(text->lines_ptr);
    text->lines_ptr = nullptr;

    text->lines_count = 0;
}

//==============================================================================

size_t GetFileSize(FILE *ptr_file)
{
    struct _stat file_info = {};

    _fstat(fileno(ptr_file), &file_info);

    return  file_info.st_size;
}

//==============================================================================

TextErrs_t ReadTextFromFile(Text       *text,
                            const char *file_name,
                            size_t      ParseFunc(char *))
{
    /*char sys_cmd[kSysCmdLen] = {0};
    char new_file_name[kSysCmdLen] = {0};

    sprintf(new_file_name, "converted_%s", file_name);
    sprintf(sys_cmd, "iconv -f CP1251 -t UTF-8 %s > %s", file_name, new_file_name);

    system(sys_cmd);*/

    FILE *input_file = fopen(file_name, "rb");

    if (input_file == nullptr)
    {
        perror("\nReadTextFromFile() failed to open input file\n");

        return kOpenError;
    }

    text->buf_size = GetFileSize(input_file) + 1;
    text->buf      = (char *) calloc(text->buf_size, sizeof(char));

    if (!text->buf)
    {
        perror("\n>>ReadTextFromFile() failed to allocate memory for buf");

        return kAllocError;
    }

    size_t check_read = fread(text->buf, sizeof(char), text->buf_size - 1, input_file);

    if (check_read != (text->buf_size - 1))
    {
        perror("\n>>ReadTextFromFile() failed to read from file");

        return kReadingError;
    }

    text->lines_count = ParseFunc(text->buf);

    FillText(text);

    if (fclose(input_file))
    {
        perror("\n>>ReadTextFromFile() failed to close input file");

        return kCloseError;
    }

    return kSuccess;
}

//==============================================================================

size_t SplitBufIntoWords(char *buf)
{
    size_t lines_count = 0;

    while (*buf != '\0')
    {
        if (*buf == '\"')
        {
            *(buf++) = '\0';

            while (*buf != '\"')
            {
                ++buf;
            }

            *(buf++) = '\0';

            ++lines_count;
        }

        if ((*buf) == '\n' || (*buf) == '\r' || (*buf) == ' ')
        {
            while (*buf == '\n' || *buf == '\r' || *buf == ' ')
            {
                *(buf++) = '\0';
            }
        }
        else
        {
            while ((*buf) != '\n' && *buf != '\0' && *buf != '\r' && *buf != ' ')
            {
                ++buf;
            }

            ++lines_count;
        }
    }

    return lines_count;
}

//==============================================================================

size_t SplitBufIntoLines(char *buf)
{
    char *comment = nullptr;

    if ((comment = strchr(buf, '#')) != nullptr)
    {
        *comment = '\0';
    }

    size_t lines_count = 0;

    while (*buf != '\0')
    {
        if ((*buf) == '\n' || (*buf) == '\r')
        {
            while (*buf == '\n' || *buf == '\r')
            {
                *(buf++) = '\0';
            }
        }
        else
        {
            while ((*buf) != '\n' && *buf != '\0' && *buf != '\r')
            {
                ++buf;
            }

            ++lines_count;
        }
    }

    return lines_count;
}

//==============================================================================

void FillText(Text *text)
{
    text->lines_ptr = (char **) calloc(text->lines_count, sizeof(char *));

    char *cur_word = text->buf;

    size_t i = 0;

    size_t words_pos = 0;

    while (i < text->buf_size)
    {
        if (*(text->buf + i) == '\0')
        {
            while (*(text->buf + i) == '\0')
            {
                ++i;
            }

            *(text->lines_ptr + words_pos++) = cur_word;

            cur_word = text->buf + i;
        }

        ++i;
    }

}

//==============================================================================

void PrintTextInFile(FILE *output_file,
                     Text *text)
{
    for (size_t i = 0; i < text->lines_count; i++)
    {
        fprintf(output_file, "%s\n", *(text->lines_ptr + i));
    }
}

//==============================================================================

TextErrs_t GetStr(char   *string,
                  size_t  max_size)
{
    int i = 0;

    int c = 0;

    bool BufferOverflowStatus = false;

    while ((c = getchar()) != '\n')
    {
        if (i < max_size - 1)
        {
            if (c != EOF)
            {
                string[i++] = (char) c;
            }
            else
            {
                return kEOF;
            }
        }
        else
        {
            BufferOverflowStatus = true;
        }
    }

    if (BufferOverflowStatus)
    {
        return kBufferOverflow;
    }

    if (i > 0)
    {
        string[i] = '\0';
    }

    return kSuccess;;
}

//==============================================================================
