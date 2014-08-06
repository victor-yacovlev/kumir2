"""
Kumir constants defining code highlighting

LxTypeEmpty         -- typically used for spaces and other non-significant stuff
LxTypeComment       -- comments starts with #
LxTypeName          -- a 'generic' name lexem in case if there is not possible to
                       get name meaning
LxTypeSecondaryKwd  -- 'secondary' keyword, which do not define statement type
                       Examples: in, as, is, and, or
LxTypePrimaryKwd    -- 'primary' keyword, which defines statement type
                       Examples: def, for, import, from, with, global
LxTypeConstant      -- a 'generic' constant in case if there is not possible to
                       get constant type
LxTypeOperator      -- operator lexem
LxTypeError         -- error lexem; it is optional flag


LxNameClass         -- type or class name
LxNameVar           -- local or global variable name
LxNameAlg           -- function or method name
LxNameModule        -- module name

LxConstInteger      -- integer constant
LxConstReal         -- floating-point constant
LxConstBoolTrue     -- boolean True constant
LxConstBoolFalse    -- boolean False constant
LxConstLiteral      -- literal (char of string) constant

"""

LxTypeEmpty = 0x00000000  # empty mask
LxTypeComment = 0x00000001  # bit 0
LxTypeName = 0x0000001C  # bits [2..4]
LxTypeSecondaryKwd = 0x000001E0  # bits [5..8]
LxTypePrimaryKwd = 0x0000F800  # bits [11..15]
LxTypeConstant = 0x001E0000  # bits [17..20]
LxTypeOperator = 0x01F00000  # bits [21..25]
LxTypeError = 0x80000000   # bit 31

# LxTypeName values

LxNameClass = 0x4
LxNameVar = 0x8
LxNameAlg = 0xc
LxNameModule = 0x10

# LxTypeConstant values

LxConstInteger = 0x20000
LxConstReal = 0x40000
LxConstBoolTrue = 0x60000
LxConstBoolFalse = 0x80000
LxConstLiteral = 0xa0000