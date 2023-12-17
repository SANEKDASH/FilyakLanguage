#ifndef NAME_TABLE_HEADER
#define NAME_TABLE_HEADER

typedef enum
{
    kOperator      = 1,
    kConstNumber   = 2,
    kIdentificator = 3,
    kEndOfLine     = 4
} ExpressionType_t;


typedef enum
{
    kNotAnOperation = 0,
    kAdd            = 1,  // +
    kSub            = 2,  // -
    kMult           = 3,  // *
    kDiv            = 4,  // /
    kSin            = 5,  // sin
    kCos            = 6,  // cos
    kFloor          = 7,  // floor
    kDiff           = 8,  // diff
    kSqrt           = 9,  // sqrt
    kIf             = 10, // if
    kWhile          = 11, // while
    kAssign         = 12, // =
    kMore           = 13, // >
    kLess           = 14, // <
    kAnd            = 15, // &&
    kOr             = 16, // ||
    kMoreOrEqual    = 17, // >=
    kLessOrEqual    = 18, // <=
    kEqual          = 19, // ==
    kNotEqual       = 20, // !=
    kBreak          = 21, // break
    kContinue       = 22, // continue
    kReturn         = 23, // return
    kLeftBracket    = 24,
    kRightBracket   = 25,
} KeyCode_t;

struct KeyWord
{
    const char *key_word;
    KeyCode_t   key_code;
    size_t      word_len;
};

static const KeyWord NameTable[]=
{
    "plus"      ,kAdd,         4,
    "minus"     ,kSub,         5,
    "mult"      ,kMult,        4,
    "div"       ,kDiv,         3,
    "sin"       ,kSin,         3,
    "cos"       ,kCos,         3,
    "floor"     ,kFloor,       5,
    "diff"      ,kDiff,        4,
    "sqrt"      ,kSqrt,        4,
    "if"        ,kIf,          2,
    "while"     ,kWhile,       5,
    "="         ,kAssign,      1,
    ">"         ,kMore,        1,
    "<"         ,kLess,        1,
    "and"       ,kAnd,         3,
    "or"        ,kOr,          2,
    ">="        ,kMoreOrEqual, 2,
    "<="        ,kLessOrEqual, 2,
    "=="        ,kEqual,       2,
    "!="        ,kNotEqual,    2,
    "break"     ,kBreak,       5,
    "continue"  ,kContinue,    8,
    "return"    ,kReturn,      6,
    "("         ,kLeftBracket, 1,
    ")"         ,kRightBracket,1,
};

static const size_t kKeyWordCount = sizeof(NameTable) / sizeof(KeyWord);

#endif
