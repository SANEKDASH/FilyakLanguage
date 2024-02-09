//code generation for logical operators

ASM_CMD(kEqual,       equal_op_count,         "je",  "equal_op_end", "==")
ASM_CMD(kLess,        less_op_count,          "jb",  "less_op_end", "<")
ASM_CMD(kMore,        more_op_count,          "ja",  "more_op_end", ">")
ASM_CMD(kLessOrEqual, less_or_euqal_op_count, "jbe", "less_or_equal_op_end", "<=")
ASM_CMD(kMoreOrEqual, more_or_equal_op_count, "jae", "more_or_equal_op_end", ">=")
ASM_CMD(kNotEqual,    not_equal_op_count,     "jne", "not_equal_op_end", "!=")
ASM_CMD(kAnd,         and_op_count,           "jb",  "and_op_end", "&&")
ASM_CMD(kOr,          or_op_count,            "jb",  "or_op_end", "||")
