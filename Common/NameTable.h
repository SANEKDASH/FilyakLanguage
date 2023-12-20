#ifndef NAME_TABLE_HEADER
#define NAME_TABLE_HEADER

typedef enum
{
    kOperator       = 1,
    kConstNumber    = 2,
    kIdentificator  = 3,
    kFuncDef        = 4,
    kFuncParameters = 5,
    kVarDecl        = 6,
} ExpressionType_t;


typedef enum
{
    kNotAnOperation   = 0,
    kAdd              = 1,  // +
    kSub              = 2,  // -
    kMult             = 3,  // *
    kDiv              = 4,  // /
    kSin              = 5,  // sin
    kCos              = 6,  // cos
    kFloor            = 7,  // floor
    kDiff             = 8,  // diff
    kSqrt             = 9,  // sqrt
    kIf               = 10, // if
    kWhile            = 11, // while
    kAssign           = 12, // =
    kMore             = 13, // >
    kLess             = 14, // <
    kAnd              = 15, // &&
    kOr               = 16, // ||
    kMoreOrEqual      = 17, // >=
    kLessOrEqual      = 18, // <=
    kEqual            = 19, // ==
    kNotEqual         = 20, // !=
    kBreak            = 21, // break
    kContinue         = 22, // continue
    kReturn           = 23, // return
    kLeftBracket      = 24, //не в стандарте, но и в дереве их нет
    kRightBracket     = 25, //не в стандарте, но и в дереве их нет
    kLeftZoneBracket  = 26, //не в стандарте, но и в дереве их нет
    kRightZoneBracket = 27, //не в стандарте, но и в дереве их нет
    kEnumOp           = 28,
    kEndOfLine        = 29,
    kDoubleType       = 30,
    kNextTransUnit    = 41,

} KeyCode_t;

struct KeyWord
{
    const char *key_word;
    KeyCode_t   key_code;
    size_t      word_len;
};

static const KeyWord NameTable[]=
{
    "лежать+сосать"         ,kAdd,              13,
    "потерял_ранг"          ,kSub,              12,
    "посадить_на_zxc"       ,kMult,             15,
    "тычка"                 ,kDiv,              5,
    "углы_вымеряет"         ,kSin,              13,
    "это_преломления_лучей" ,kCos,              21,
    "убей_себя_блять"       ,kFloor,            15,
    "реквием"               ,kDiff,             7,
    "трент_ультанул"        ,kSqrt,             14,
    "???"                   ,kIf,               3,
    "while"                 ,kWhile,            5,
    "ты"                    ,kAssign,           2,
    "больше"                ,kMore,             6,
    "меньше"                ,kLess,             6,
    "и"                     ,kAnd,              1,
    "или"                   ,kOr,               3,
    "не_больше"             ,kMoreOrEqual,      9,
    "не_меньше"             ,kLessOrEqual,      9,
    "ты_точно"              ,kEqual,            8,
    "ты_не"                 ,kNotEqual,         5,
    "съеби_нахуй!"          ,kBreak,            12,
    "иди_блять!"            ,kContinue,         10,
    "верни_курьера_блять"   ,kReturn,           19,
    "мать"                  ,kLeftBracket,      4,
    "ебал"                  ,kRightBracket,     4,
    "стань"                 ,kLeftZoneBracket,  5,
    "мид"                   ,kRightZoneBracket, 3,
    ","                     ,kEnumOp,           1,
    ";"                     ,kEndOfLine,        1,
    "долбоеб"               ,kDoubleType,       7,
};

static const size_t kKeyWordCount = sizeof(NameTable) / sizeof(KeyWord);

#endif
