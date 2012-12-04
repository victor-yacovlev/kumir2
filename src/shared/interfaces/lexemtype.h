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

        LxNameClass         = 0x4,
        LxNameVar           = 0x8,
        LxNameAlg           = 0xc,
        LxNameModule        = 0x10,

        // LxTypeSecondaryKwd values

        LxSecFor	= 0x20,
        LxSecTimes	= 0x40,
        LxSecFrom	= 0x60,
        LxSecTo         = 0x80,
        LxSecStep	= 0xa0,
        LxSecWhile	= 0xc0,
        LxSecNewline	= 0xe0,
        LxSecIf         = 0x100,
        LxSecAnd	= 0x120,
        LxSecOr         = 0x140,
        LxSecNot	= 0x160,
        LxSecOut	= 0x180,
        LxSecIn         = 0x1a0,
        LxSecInout	= 0x1c0,


        // LxTypePrimaryKwd values

        LxPriModule	= 0x800,
        LxPriEndModule	= 0x1000,
        LxPriAlgHeader	= 0x1800,
        LxPriAlgBegin	= 0x2000,
        LxPriAlgEnd	= 0x2800,
        LxPriPre	= 0x3000,
        LxPriPost	= 0x3800,
        LxPriIf         = 0x4000,
        LxPriThen	= 0x4800,
        LxPriElse	= 0x5000,
        LxPriFi         = 0x5800,
        LxPriSwitch	= 0x6000,
        LxPriCase	= 0x6800,
        LxPriLoop	= 0x7000,
        LxPriEndLoop	= 0x7800,
        LxPriInput	= 0x8000,
        LxPriOutput	= 0x8800,
        LxPriFinput	= 0x9000,
        LxPriFoutput	= 0x9800,
        LxPriAssign	= 0xa000,
        LxPriAssert	= 0xa800,
        LxPriImport	= 0xb000,
        LxPriAssignFile = 0xb800,
        LxPriExit	= 0xc000,
        LxPriPause      = 0xc800,
        LxPriHalt       = 0xd000,


        // LxTypeConstant values

        LxConstInteger      = 0x20000,
        LxConstReal         = 0x40000,
        LxConstBoolTrue     = 0x60000,
        LxConstBoolFalse    = 0x80000,
        LxConstLiteral      = 0xa0000,

        // LxTypeOperator values

        LxOperPlus              = 0x200000,
        LxOperMinus             = 0x400000,
        LxOperEqual             = 0x600000,
        LxOperAsterisk          = 0x800000,
        LxOperSlash             = 0xa00000,
        LxOperPower             = 0xc00000,
        LxOperLeftBr            = 0xe00000,
        LxOperRightBr           = 0x1000000,
        LxOperLeftSqBr          = 0x1200000,
        LxOperRightSqBr         = 0x1400000,
        LxOperColon             = 0x1600000,
        LxOperComa              = 0x1800000,
        LxOperLess              = 0x1a00000,
        LxOperGreater           = 0x1c00000,
        LxOperLessOrEqual	= 0x1e00000,
        LxOperGreaterOrEqual	= 0x2000000,
        LxOperNotEqual          = 0x2200000,
        LxOperSemicolon         = 0x2400000,
        LxOperLeftBrace         = 0x2600000,
        LxOperRightBrace       	= 0x2800000

    };
}




#endif // LEXEMTYPE_H
