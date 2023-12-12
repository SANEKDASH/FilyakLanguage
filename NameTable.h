#ifndef NAME_TABLE_HEADER
#define NAME_TABLE_HEADER

typedef enum
{
    kOperator      = 1,
    kConstNumber   = 2,
    kIdentificator = 3,
} ExpressionType_t;


typedef enum
{
    kNotAnOperation = 0,
    kAdd            = 1,
    kSub            = 2,
    kMult           = 3,
    kDiv            = 4,
    kSin            = 5,
    kCos            = 6,
    kFloor          = 7,
    kDiff           = 8,
    kSqrt           = 9,
    kIf             = 10,
    kWhile          = 11,
    kAssign         = 12,
    kMore           = 13,
    kLess           = 14,
    kAnd            = 15,
    kOr             = 16,
    kMoreOrEqual    = 17,
    kLessOrEqual    = 18,
    kEqual          = 19,
    kNotEqual       = 20,
    kBreak          = 21,
    kContinue       = 22,
    kReturn         = 23,
} KeyCode_t;

struct KeyWord
{
    const char *key_word;
    KeyCode_t   key_code;
};

static const KeyWord NameTable[]=
{
    "+"         ,kAdd,
    "-"         ,kSub,
    "*"         ,kMult,
    "/"         ,kDiv,
    "sin"       ,kSin,
    "cos"       ,kCos,
    "floor"     ,kFloor,
    "diff"      ,kDiff,
    "sqrt"      ,kSqrt,
    "if"        ,kIf,
    "while"     ,kWhile,
    "="         ,kAssign,
    ">"         ,kMore,
    "<"         ,kLess,
    "and"       ,kAnd,
    "or"        ,kOr,
    ">="        ,kMoreOrEqual,
    "<="        ,kLessOrEqual,
    "=="        ,kEqual,
    "!="        ,kNotEqual,
    "break"     ,kBreak,
    "continue"  ,kContinue,
    "return"    ,kReturn
};

#endif
