#ifndef NAME_TABLE_HEADER
#define NAME_TABLE_HEADER

static const char *kMainFuncName = "аганим";

typedef enum
{
    kConstNumber    = 1,
    kIdentificator  = 2,
    kOperator       = 3,
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

    kDoubleType       = 51,

    kPrint            = 62,
    kScan             = 61,

    kReturn           = 71, // return
    kBreak            = 72, // break
    kContinue         = 73, // continue
    kAbort            = 74,

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
    "сплитим_на"            ,kDiv,              10,
    "углы_вымер€ет"         ,kSin,              13,
    "это_преломлени€_бл€ть" ,kCos,              21,
    "целуй_пол_сука"        ,kFloor,            14,
    "реквием"               ,kDiff,             7,
    "трент_ультует"         ,kSqrt,             13,
    "???"                   ,kIf,               3,
    "пока"                  ,kWhile,            4,
    "ты"                    ,kAssign,           2,
    "больше"                ,kMore,             6,
    "меньше"                ,kLess,             6,
    "и"                     ,kAnd,              1,
    "или"                   ,kOr,               3,
    "не_больше"             ,kMoreOrEqual,      9,
    "не_меньше"             ,kLessOrEqual,      9,
    "точно"                 ,kEqual,            5,
    "ты_не"                 ,kNotEqual,         5,
    "стой_бл€ть!"           ,kBreak,            11,
    "иди_бл€ть!"            ,kContinue,         10,
    "верни_курьера_бл€ть"   ,kReturn,           19,
    "мать"                  ,kLeftBracket,      4,
    "ебал"                  ,kRightBracket,     4,
    "стань"                 ,kLeftZoneBracket,  5,
    "мид"                   ,kRightZoneBracket, 3,
    ","                     ,kEnumOp,           1,
    "?"                     ,kEndOfLine,        1,
    "долбоеб"               ,kDoubleType,       7,
    "пишу_твоей_матери"     ,kPrint,            17,
    "скажи_мне"             ,kScan,             9,
    "иди_нахуй"             ,kAbort,            9
};

static const size_t EndOfLinePos = 28;

static const size_t kKeyWordCount = sizeof(NameTable) / sizeof(KeyWord);

#endif
