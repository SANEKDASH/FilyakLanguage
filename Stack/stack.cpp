#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "stack.h"
#include "../debug/debug.h"
#include "../Common/NameTable.h"

#define DEBUG

#ifdef DEBUG
#define CHECKSTACK(stk) StackVerify(stk);STACKDUMP(stk);
#define DEBUG_ON(...) __VA_ARGS__
#else
#define CHECKSTACK(stk)
#define DEBUG_ON(...)
#endif


#ifdef DEBUG
#endif
static FILE *LogFile = nullptr;
#define LOGNAME "log_file.html"

static const int kBaseStackSize = 8;

static const int kMultiplier = 2;

static const StackElemType_t kPoisonVal = (TreeNode*) 0xBADBABA;
static const int kDestrVal   = 0xDEAD;
static const StackElemType_t *kDestrPtr  = (StackElemType_t*)0xDEADBEEF;

CANARY_ON(static const CanaryType_t kCanaryVal = 0xDEADB14D;)

static void PoisonMem(Stack *stk, StackElemType_t poison_value);

static StackErr_t ChangeStackCapacity(Stack *stk, size_t multiplier, char mode);

static void HashStack(Stack *stk);

static unsigned long long HashFunc(const void *data, size_t count);

CANARY_ON(
          static CanaryType_t *GetLeftDataCanaryPtr(const Stack *stk)
          {
              CHECK(stk);

              return (CanaryType_t *)((char *)stk->stack_data.data - sizeof(CanaryType_t)); // shift
          }

          static CanaryType_t *GetRightDataCanaryPtr(const Stack *stk)
          {
              CHECK(stk);

              return (CanaryType_t *)((char *)stk->stack_data.data + stk->stack_data.capacity * sizeof(StackElemType_t));
          }
         )

StackErr_t StackVerify(Stack *stk)
{
    CHECK(stk);

    if (stk == nullptr)
    {
        return kWrongSize;
    }

    if (stk->stack_data.data == nullptr)
    {
        stk->stack_data.status |= kNullData;
    }

    if (stk->stack_data.size < 0)
    {
        stk->stack_data.status |= kWrongPos;
    }

    if (stk->stack_data.capacity <= 0)
    {
        stk->stack_data.status |= kWrongSize;
    }

    if (stk->stack_data.size > stk->stack_data.capacity)
    {
        stk->stack_data.status |= kPosHigherSize;
    }

    if  (  stk->stack_data.data == kDestrPtr
        || stk->stack_data.size == kDestrVal
        || stk->stack_data.capacity == kDestrVal
        HASH_ON(
                || stk->hash.data_hash == kDestrVal
                || stk->hash.struct_hash == kDestrVal
               )
        CANARY_ON(
                  || stk->left_canary  == kDestrVal
                  || stk->right_canary == kDestrVal
                 )
        )
    {
        stk->stack_data.status |= kDestroyedStack;
    }

    HASH_ON(
            if (stk->stack_data.data != nullptr && stk->stack_data.capacity > 0)
            {
                if (HashFunc(stk->stack_data.data, sizeof(StackElemType_t) * stk->stack_data.capacity) != stk->hash.data_hash)
                {
                stk->stack_data.status |= kDataHashError;
                }
            }

            if (HashFunc((char *)stk CANARY_ON(+ sizeof(CanaryType_t)) , sizeof(StackData)) != stk->hash.struct_hash)
            {
                stk->stack_data.status |= kStructHashError;
            }
           )

    CANARY_ON(
              if (stk->left_canary != kCanaryVal)
              {
                  stk->stack_data.status |= kLeftCanaryScreams;
              }

              if (stk->right_canary != kCanaryVal)
              {
                  stk->stack_data.status |= kRightCanaryScreams;
              }

              if (stk->stack_data.status == kStackClear)
              {
                  if (*GetRightDataCanaryPtr(stk) != kCanaryVal)
                  {
                      stk->stack_data.status |= kRightDataCanaryFuck;
                  }

                  if (*GetLeftDataCanaryPtr(stk) != kCanaryVal)
                  {
                      stk->stack_data.status |= kLeftDataCanaryFuck;
                  }
              }
             )

    return (StackStatus) stk->stack_data.status;
}

StackErr_t StackInit(Stack *stk)
{
    CHECK(stk);

    stk->stack_data.capacity = kBaseStackSize;
    stk->stack_data.size = 0;
    stk->stack_data.status = kStackClear;

    size_t size = kBaseStackSize * sizeof(StackElemType_t);

    CANARY_ON(
              stk->left_canary = stk->right_canary = kCanaryVal;
              size += 2 * sizeof(CanaryType_t);
             )

    stk->stack_data.data = (StackElemType_t *) calloc(size, sizeof(char));

    int errcode = errno;

    if (stk->stack_data.data == nullptr)
    {
        DEBUG_ON(fprintf(LogFile, "Got an error while getting memory for stack : %s\n", strerror(errcode));)

        return kNullData;
    }

    CANARY_ON(
              stk->stack_data.data = (StackElemType_t *)((char *)stk->stack_data.data + sizeof(CanaryType_t));

              *GetLeftDataCanaryPtr(stk)  = kCanaryVal;
              *GetRightDataCanaryPtr(stk) = kCanaryVal;
             )

    PoisonMem(stk, kPoisonVal);

    HASHSTACK(stk);

    return kStackClear;
}

StackErr_t StackDtor(Stack *stk)
{
    CHECKSTACK(stk);

    if (stk->stack_data.status != kStackClear)
    {
        DEBUG_ON(
                   fprintf(LogFile, "\nBroken stack got into destructor."
                                    "\n Something went wrong so we must abort programm.:(\n");
               )
        abort();
    }

    stk->stack_data.size  = kDestrVal;
    stk->stack_data.capacity = kDestrVal;

    CANARY_ON(stk ->left_canary = stk->right_canary = kDestrVal;)

    if (stk->stack_data.data != nullptr)
    {
        CANARY_ON(stk->stack_data.data = (StackElemType_t *)((char *)stk->stack_data.data - sizeof(CanaryType_t));)

        free(stk->stack_data.data);
    }

    stk->stack_data.data = nullptr;

    HASH_ON(stk->hash.data_hash = stk->hash.struct_hash = kDestrVal;)

    return kStackClear;
}

static const char kExpand = 'e';

StackErr_t Push(Stack *stk, StackElemType_t in_value)
{
    CHECKSTACK(stk);

    if (stk->stack_data.status == kStackClear)
    {
        StackErr_t check_expand = kStackClear;

        if (stk->stack_data.size >= stk->stack_data.capacity)
        {
            check_expand = ChangeStackCapacity(stk, kMultiplier, kExpand);
        }

        if (check_expand == kStackClear)
        {
            stk->stack_data.data[(stk->stack_data.size)++] = in_value;
        }

        HASHSTACK(stk);
    }

    CHECKSTACK(stk);

    return stk->stack_data.status;
}

static const char kDivide = 'd';

StackErr_t Pop(Stack *stk, StackElemType_t *ret_value)
{
    CHECKSTACK(stk);

    if (stk->stack_data.status != kStackClear)
    {
        return stk->stack_data.status;
    }

    if (stk->stack_data.size == 0)
    {
        return kWrongUsingOfStack;
    }

    *ret_value = stk->stack_data.data[--(stk->stack_data.size)];
    stk->stack_data.data[stk->stack_data.size] = kPoisonVal;

    if (stk->stack_data.size < (stk->stack_data.capacity / (kMultiplier * kMultiplier))
        && (stk->stack_data.capacity / kMultiplier) >= kBaseStackSize)
    {
        ChangeStackCapacity(stk, kMultiplier, kDivide);
    }

    CHECKSTACK(stk);

    HASHSTACK(stk);

    return stk->stack_data.status;
}

void StackDump(const Stack *stk, LogInfo info)
{
    CHECK(stk);
    fprintf(LogFile, "[%s, %s]\nstack[%p] \"%s\" from line[%d], func[%s], file [%s]\n"
                     "{\n\tpos = %d\n\tsize = %d \n\tdata[%p]\n\tstack status = %d\n\n",
                                                                                        info.time,
                                                                                        info.date,
                                                                                        stk,
                                                                                        info.stack_name,
                                                                                        info.line,
                                                                                        info.func,
                                                                                        info.file,
                                                                                        stk->stack_data.size,
                                                                                        stk->stack_data.capacity,
                                                                                        stk->stack_data.data,
                                                                                        stk->stack_data.status);

    if (stk->stack_data.status == 0)
    {
        fprintf(LogFile, "<font color =\"green\">\terror: no errors.\n\n</font>");
    }
    else
    {
        for (size_t pos = 0; pos < kStatusNumber; ++pos)
        {
            if (GetBit(stk->stack_data.status, pos) > 0)
            {
                fprintf(LogFile, "<font color = \"red\">\t*error: %s\n\n</font>", ErrorArray[pos + 1].error_expl);
            }
        }
    }

    CANARY_ON(
                fprintf(LogFile, "\tleft structure canary val = 0x%x\n", stk->left_canary);
                fprintf(LogFile, "\tright structure canary val = 0x%x\n\n", stk->right_canary);
                if (stk->stack_data.status == kStackClear)
                {
                    fprintf(LogFile, "\tleft data canary val = 0x%x\n", *GetLeftDataCanaryPtr(stk));
                    fprintf(LogFile, "\tright data canary val = 0x%x\n\n", *GetRightDataCanaryPtr(stk));
                }
             )

    HASH_ON(
                fprintf(LogFile, "\tcurrent structure hash value = %llu\n", stk->hash.struct_hash);
                fprintf(LogFile, "\tcurrent data hash value = %llu\n\n\t{\n", stk->hash.data_hash);
           )

    if (stk->stack_data.status == kStackClear)
    {
        for (size_t i = 0; i < stk->stack_data.capacity; ++i)
        {
            if (stk->stack_data.data[i] == kPoisonVal)
            {
                fprintf(LogFile, "\t\t[%d] = PZN\n", i);
            }
            else
            {
                if (stk->stack_data.data[i] != nullptr)
                {
                    fprintf(LogFile, "\t\t[%d] = [pointer - %p]\n", i, stk->stack_data.data[i]);
                    fprintf(LogFile,"\t\t |\n");
                    switch (stk->stack_data.data[i]->type)
                    {
                        case kOperator:
                        {
                            fprintf(LogFile, "\t\t +---->op - \"%s\"\n\n", NameTable[stk->stack_data.data[i]->data.key_word_code - 1].key_word);

                            break;
                        }

                        case kIdentificator:
                        {
                            fprintf(LogFile, "\t\t +---->variable - %d\n\n", stk->stack_data.data[i]->data.variable_pos);

                            break;
                        }

                        case kConstNumber:
                        {
                            fprintf(LogFile, "\t\t +---->const number - %lg\n\n", stk->stack_data.data[i]->data.const_val);

                            break;
                        }

                        default:
                        {
                            printf("KAVO (stack dump)\n");

                            return;

                            break;
                        }
                    }
                }
            }
        }
    }
    fprintf(LogFile, "\t}\n}\n\n");
}

int GetBit(int num, size_t pos)
{
    CHECK(pos < kStatusNumber);

    return num & (1 << pos);
}

static void PoisonMem(Stack *stk, StackElemType_t poison_value)
{
    for (size_t i = stk->stack_data.size; i < stk->stack_data.capacity; i++)
    {
        stk->stack_data.data[i] = poison_value;
    }

    CHECKSTACK(stk);
}

static StackErr_t ChangeStackCapacity(Stack *stk, size_t multiplier, char mode)
{
    CHECKSTACK(stk);

    if (stk->stack_data.status == kStackClear)
    {
        if (mode == kDivide)
        {
            stk->stack_data.capacity /= multiplier;
        }
        else if (mode == kExpand)
        {
            stk->stack_data.capacity *= multiplier;
        }
        else
        {
            printf("Wrong change mode\n");

            return kWrongSize;
        }

        size_t size = stk->stack_data.capacity * sizeof(StackElemType_t) CANARY_ON(+ 2 * sizeof(CanaryType_t));

        CANARY_ON(stk->stack_data.data = (StackElemType_t *)((char *)stk->stack_data.data - sizeof(CanaryType_t));)

        StackElemType_t *check_ptr = (StackElemType_t *)realloc(stk->stack_data.data, size);

        if (check_ptr != nullptr)
        {
            stk->stack_data.data = check_ptr;
        }
        else
        {
            DEBUG_ON(fprintf(LogFile, "realloc error: %s\n", strerror(errno));)

            return kWrongSize;
        }

        CANARY_ON(
                  stk->stack_data.data = (StackElemType_t *)((char *)stk->stack_data.data + sizeof(CanaryType_t));

                  *GetRightDataCanaryPtr(stk) = kCanaryVal;
                  *GetLeftDataCanaryPtr(stk)  = kCanaryVal;
                 )

        PoisonMem(stk, kPoisonVal);
    }

    CHECKSTACK(stk);

    return stk->stack_data.status;
}

static unsigned long long HashFunc(const void *data, size_t count)
{
    CHECK(data);
    unsigned long long hash_sum = 0;

    if (count <= 0)
    {
        return 0;
    }

    for (size_t i = 1; i <= count; i++)
    {
        hash_sum += i * *((char *)data + (i - 1));
    }

    return hash_sum;
}

int InitLog()
{
    int errcode = 0;
    if (LogFile == nullptr)
    {
        LogFile = fopen(LOGNAME, "w");

        errcode = errno;

        if (LogFile == nullptr)
        {
            perror("failed to init log");

            return errcode;
        }

        fprintf(LogFile, "<pre>\n");
    }

    return errcode;
}

int CloseLog()
{

    fprintf(LogFile, "<pre>");
    fclose(LogFile);

    int errcode = errno;
    if (errcode != 0)
    {
        printf("failed to close log (error: %s)\n", strerror(errcode));

        return errcode;
    }

    LogFile = nullptr;

    return errcode;
}


#ifdef HASH_PROTECTION
static void HashStack(Stack *stk)
{
    CHECKSTACK(stk);

    if (stk->stack_data.status == kStackClear)
    {
        stk->hash.data_hash   = HashFunc(stk->stack_data.data, stk->stack_data.capacity * sizeof(StackElemType_t));

        stk->hash.struct_hash = HashFunc((char *)stk CANARY_ON(+ sizeof(CanaryType_t)), sizeof(StackData));
    }

    CHECKSTACK(stk);
}
#else
void HashStack(Stack *stk) {}
#endif
