#ifndef LEXEMTYPE_H
#define LEXEMTYPE_H

#include <QtCore>

#define LX_OFFSET_NAME          = 0x00000003
#define LX_OFFSET_SECONDARY_KWD = 0x0000001F
#define LX_OFFSET_PRIMARY_KWD   = 0x000007FF
#define LX_OFFSET_CONSTANT      = 0x0001FFFF
#define LX_OFFSET_OPERATOR      = 0x00100000

namespace Shared {
    enum LexemType {

        // Lexem type by bit mask

        LxTypeEmpty         = 0x00000000, // empty mask
        LxTypeComment       = 0x00000001, // bit 0
        LxTypeDoc           = 0x00000002, // bit 1
        LxTypeName          = 0x0000001C, // bits [2..4]
        LxTypeSecondaryKwd  = 0x000001E0, // bits [5..8]
        LxTypePrimaryKwd    = 0x0000F800, // bits [11..15]
        LxTypeConstant      = 0x001E0000, // bits [17..20]
        LxTypeOperator      = 0x01F00000, // bits [21..25]
        LxTypeError         = 0x80000000,  // bit 31

        // LxTypeName values

        LxNameClass         = 0x00000004,
        LxNameVar           = 0x00000005,
        LxNameAlg           = 0x00000006,
        LxNameModule        = 0x00000007,

        // LxTypeSecondaryKwd values

        LxSecFor            = 0x00000020,
        LxSecTimes          = 0x00000021,
        LxSecFrom           = 0x00000022,
        LxSecTo             = 0x00000023,
        LxSecStep           = 0x00000024,
        LxSecWhile          = 0x00000025,
        LxSecNewline        = 0x00000026,
        LxSecIf             = 0x00000027,
        LxSecAnd            = 0x00000028,
        LxSecOr             = 0x00000029,
        LxSecNot            = 0x0000002A,
        LxSecOut            = 0x0000002B,
        LxSecIn             = 0x0000002C,
        LxSecInout          = 0x0000002D,

        // LxTypePrimaryKwd values

        LxPriModule         = 0x00000800,
        LxPriEndModule      = 0x00000801,
        LxPriAlgHeader      = 0x00000802,
        LxPriAlgBegin       = 0x00000803,
        LxPriAlgEnd         = 0x00000804,
        LxPriPre            = 0x00000805,
        LxPriPost           = 0x00000806,
        LxPriIf             = 0x00000807,
        LxPriThen           = 0x00000808,
        LxPriElse           = 0x00000809,
        LxPriFi             = 0x0000080A,
        LxPriSwitch         = 0x0000080B,
        LxPriCase           = 0x0000080C,
        LxPriLoop           = 0x0000080D,
        LxPriEndLoop        = 0x0000080E,
        LxPriInput          = 0x0000080F,
        LxPriOutput         = 0x00000810,
        LxPriFinput         = 0x00000811,
        LxPriFoutput        = 0x00000812,
        LxPriAssign         = 0x00000813,
        LxPriAssert         = 0x00000814,
        LxPriImport         = 0x00000815,
        LxPriExit           = 0x00000817,

        // LxTypeConstant values

        LxConstInteger      = 0x00020000,
        LxConstReal         = 0x00020001,
        LxConstBoolTrue     = 0x00020002,
        LxConstBoolFalse    = 0x00020003,
        LxConstLiteral      = 0x00020004,

        // LxTypeOperator values

        LxOperPlus          = 0x00100000,
        LxOperMinus         = 0x00100001,
        LxOperEqual         = 0x00100002,
        LxOperAsterisk      = 0x00100003,
        LxOperSlash         = 0x00100004,
        LxOperPower         = 0x00100005,
        LxOperLeftBr        = 0x00100006,
        LxOperRightBr       = 0x00100007,
        LxOperLeftSqBr      = 0x00100008,
        LxOperRightSqBr     = 0x00100009,
        LxOperColon         = 0x0010000A,
        LxOperComa          = 0x0010000B,
        LxOperLess          = 0x0010000C,
        LxOperGreater       = 0x0010000D,
        LxOperLessOrEqual   = 0x0010000E,
        LxOperGreaterOrEqual= 0x0010000F,
        LxOperNotEqual      = 0x00100010,
        LxOperSemicolon     = 0x00100011
    };
}




#endif // LEXEMTYPE_H
