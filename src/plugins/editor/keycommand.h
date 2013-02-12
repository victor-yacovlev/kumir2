#ifndef EDITOR_KEYCOMMAND_H
#define EDITOR_KEYCOMMAND_H

#include <QString>

namespace Editor {

struct KeyCommand
{
    enum Type {

        MoveToNextChar          = 0x0001,
        SelectNextChar          = 0x0002,
        SelectNextColumn        = 0x0003,
        MoveToPreviousChar      = 0x0004,
        SelectPreviousChar      = 0x0005,
        SelectPreviousColumn    = 0x0006,
        MoveToNextLine          = 0x0007,
        SelectNextLine          = 0x0008,
        SelectNextRow           = 0x0009,
        MoveToPreviousLine      = 0x000a,
        SelectPreviousLine      = 0x000b,
        SelectPreviousRow       = 0x000c,
        MoveToStartOfLine       = 0x000d,
        SelectStartOfLine       = 0x000e,
        MoveToEndOfLine         = 0x000f,
        SelectEndOfLine         = 0x0010,
        MoveToPreviousPage      = 0x0011,
        SelectPreviousPage      = 0x0012,
        MoveToNextPage          = 0x0013,
        SelectNextPage          = 0x0014,
        MoveToStartOfDocument   = 0x0015,
        SelectStartOfDocument   = 0x0016,
        MoveToEndOfDocument     = 0x0017,
        SelectEndOfDocument     = 0x0018,
        SelectAll               = 0x0019,
        Copy                    = 0x001a,
        MoveToNextLexem         = 0x001b,
        MoveToPreviousLexem     = 0x001c,
        SelectNextLexem         = 0x001d,
        SelectPreviousLexem     = 0x001e,


        InsertText              = 0x1001,
        Backspace               = 0x1002,
        Delete                  = 0x1003,
        RemoveLine              = 0x1004,
        RemoveTail              = 0x1005,
        Cut                     = 0x1006,
        Paste                   = 0x1007,
        ToggleComment           = 0x1008

    } type;
    inline KeyCommand(KeyCommand::Type t) { type = t; text = ""; }
    inline KeyCommand(KeyCommand::Type t, const QString & s) { type=t; text=s; }
    inline KeyCommand(const QString & t) { type = InsertText, text = t; }
    inline KeyCommand(const char * s) { type = InsertText; text = QString::fromAscii(s); }
    static unsigned int CommandModifiesTextMask; // = 0x1000
    QString text;
};

extern QString dumpKeyCommandType(KeyCommand::Type type);
extern KeyCommand::Type loadKeyCommandType(const QString & s);

} // namespace Editor

#endif // EDITOR_KEYCOMMAND_H
