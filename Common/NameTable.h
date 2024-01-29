#ifndef NAME_TABLE_HEADER
#define NAME_TABLE_HEADER

typedef enum
{
    kOperator       = 1,
    kConstNumber    = 2,
    kIdentificator  = 3,
    kFuncDef        = 4,
    kParamsNode     = 5,
    kVarDecl        = 6,
    kCall           = 7,
} ExpressionType_t;


typedef enum
{
    kNotAnOperation   = 0,
    kLeftBracket      = 1, //ШШ Ш ШШШШШШШШШ, ШШ Ш Ш ШШШШШШ ШШ ШШШ
    kRightBracket     = 2, //ШШ Ш ШШШШШШШШШ, ШШ Ш Ш ШШШШШШ ШШ ШШШ
    kLeftZoneBracket  = 3, //ШШ Ш ШШШШШШШШШ, ШШ Ш Ш ШШШШШШ ШШ ШШШ
    kRightZoneBracket = 4, //ШШ Ш ШШШШШШШШШ, ШШ Ш Ш ШШШШШШ ШШ ШШШ
    kDoubleType       = 5,

    kIf               = 11, // if
    kWhile            = 12, // while
    kAssign           = 13, // =

    kSin              = 21,  // sin
    kCos              = 22,  // cos
    kFloor            = 23,  // floor
    kAdd              = 24,  // +
    kSub              = 25,  // -
    kMult             = 26,  // *
    kDiv              = 27,  // /
    kDiff             = 28,  // diff
    kSqrt             = 29,  // sqrt

    kEqual            = 31, // ==
    kLess             = 32, // <
    kMore             = 33, // >
    kLessOrEqual      = 34, // <=
    kMoreOrEqual      = 35, // >=
    kNotEqual         = 36, // !=
    kAnd              = 37, // &&
    kOr               = 38, // ||

    kEndOfLine        = 41,
    kEnumOp           = 42,

    kPrint            = 62,
    kScan             = 61,

    kBreak            = 72, // break
    kContinue         = 73, // continue
    kReturn           = 71, // return
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
    "потер€л_птсы"          ,kSub,              12,
    "посадить_на_zxc"       ,kMult,             15,
    "тычка"                 ,kDiv,              5,
    "углы_вымер€ет"         ,kSin,              13,
    "это_преломлени€_лучей" ,kCos,              21,
    "убей_себ€_бл€ть"       ,kFloor,            15,
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
    "точно"                 ,kEqual,            5,
    "ты_не"                 ,kNotEqual,         5,
    "съеби_нахуй!"          ,kBreak,            12,
    "иди_бл€ть!"            ,kContinue,         10,
    "верни_курьера_бл€ть"   ,kReturn,           19,
    "мать"                  ,kLeftBracket,      4,
    "ебал"                  ,kRightBracket,     4,
    "стань"                 ,kLeftZoneBracket,  5,
    "мид"                   ,kRightZoneBracket, 3,
    ","                     ,kEnumOp,           1,
    ";"                     ,kEndOfLine,        1,
    "долбоеб"               ,kDoubleType,       7,
    "пишу_твоей_матери"     ,kPrint,            17,
    "скажи_мне"             ,kScan,             9
};


static const size_t kKeyWordCount = sizeof(NameTable) / sizeof(KeyWord);

#endif
