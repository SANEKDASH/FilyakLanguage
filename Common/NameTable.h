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
    kLeftBracket      = 1, //�� � ���������, �� � � ������ �� ���
    kRightBracket     = 2, //�� � ���������, �� � � ������ �� ���
    kLeftZoneBracket  = 3, //�� � ���������, �� � � ������ �� ���
    kRightZoneBracket = 4, //�� � ���������, �� � � ������ �� ���
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
    "������+������"         ,kAdd,              13,
    "�������_����"          ,kSub,              12,
    "��������_��_zxc"       ,kMult,             15,
    "�����"                 ,kDiv,              5,
    "����_��������"         ,kSin,              13,
    "���_�����������_�����" ,kCos,              21,
    "����_����_�����"       ,kFloor,            15,
    "�������"               ,kDiff,             7,
    "�����_��������"        ,kSqrt,             14,
    "???"                   ,kIf,               3,
    "while"                 ,kWhile,            5,
    "��"                    ,kAssign,           2,
    "������"                ,kMore,             6,
    "������"                ,kLess,             6,
    "�"                     ,kAnd,              1,
    "���"                   ,kOr,               3,
    "��_������"             ,kMoreOrEqual,      9,
    "��_������"             ,kLessOrEqual,      9,
    "�����"                 ,kEqual,            5,
    "��_��"                 ,kNotEqual,         5,
    "�����_�����!"          ,kBreak,            12,
    "���_�����!"            ,kContinue,         10,
    "�����_�������_�����"   ,kReturn,           19,
    "����"                  ,kLeftBracket,      4,
    "����"                  ,kRightBracket,     4,
    "�����"                 ,kLeftZoneBracket,  5,
    "���"                   ,kRightZoneBracket, 3,
    ","                     ,kEnumOp,           1,
    ";"                     ,kEndOfLine,        1,
    "�������"               ,kDoubleType,       7,
    "����_�����_������"     ,kPrint,            17,
    "�����_���"             ,kScan,             9
};


static const size_t kKeyWordCount = sizeof(NameTable) / sizeof(KeyWord);

#endif
