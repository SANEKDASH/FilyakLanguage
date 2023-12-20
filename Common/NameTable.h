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
    kLeftBracket      = 24, //�� � ���������, �� � � ������ �� ���
    kRightBracket     = 25, //�� � ���������, �� � � ������ �� ���
    kLeftZoneBracket  = 26, //�� � ���������, �� � � ������ �� ���
    kRightZoneBracket = 27, //�� � ���������, �� � � ������ �� ���
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
    "��_�����"              ,kEqual,            8,
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
};

static const size_t kKeyWordCount = sizeof(NameTable) / sizeof(KeyWord);

#endif
