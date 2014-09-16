#include "keycommand.h"

namespace Editor {

unsigned int KeyCommand::CommandModifiesTextMask = 0x1000;

static const QString sMoveToNextChar = "MoveToNextChar";
static const QString sSelectNextChar = "SelectNextChar";
static const QString sSelectNextColumn = "SelectNextColumn";
static const QString sMoveToPreviousChar = "MoveToPreviousChar";
static const QString sSelectPreviousChar = "SelectPreviousChar";
static const QString sSelectPreviousColumn = "SelectPreviousColumn";
static const QString sMoveToNextLine = "MoveToNextLine";
static const QString sSelectNextLine = "SelectNextLine";
static const QString sSelectNextRow = "SelectNextRow";
static const QString sMoveToPreviousLine = "MoveToPreviousLine";
static const QString sSelectPreviousLine = "SelectPreviousLine";
static const QString sSelectPreviousRow = "SelectPreviousRow";
static const QString sMoveToStartOfLine = "MoveToStartOfLine";
static const QString sSelectStartOfLine = "SelectStartOfLine";
static const QString sMoveToEndOfLine = "MoveToEndOfLine";
static const QString sSelectEndOfLine = "SelectEndOfLine";
static const QString sMoveToPreviousPage = "MoveToPreviousPage";
static const QString sSelectPreviousPage = "SelectPreviousPage";
static const QString sMoveToNextPage = "MoveToNextPage";
static const QString sSelectNextPage = "SelectNextPage";
static const QString sMoveToStartOfDocument = "MoveToStartOfDocument";
static const QString sSelectStartOfDocument = "SelectStartOfDocument";
static const QString sMoveToEndOfDocument = "MoveToEndOfDocument";
static const QString sSelectEndOfDocument = "SelectEndOfDocument";
static const QString sSelectAll = "SelectAll";
static const QString sCopy = "Copy";

static const QString sInsertText = "InsertText";
static const QString sInsertImport = "InsertImport";
static const QString sBackspace = "Backspace";
static const QString sDelete = "Delete";
static const QString sRemoveLine = "RemoveLine";
static const QString sRemoveTail = "RemoveTail";
static const QString sCut = "Cut";
static const QString sPaste = "Paste";

extern QString dumpKeyCommandType(KeyCommand::Type type)
{
    switch (type) {
    case KeyCommand::MoveToNextChar: return sMoveToNextChar;
    case KeyCommand::SelectNextChar: return sSelectNextChar;
    case KeyCommand::SelectNextColumn: return sSelectNextColumn;
    case KeyCommand::MoveToPreviousChar: return sMoveToPreviousChar;
    case KeyCommand::SelectPreviousChar: return sSelectPreviousChar;
    case KeyCommand::SelectPreviousColumn: return sSelectPreviousColumn;
    case KeyCommand::MoveToNextLine: return sMoveToNextLine;
    case KeyCommand::SelectNextLine: return sSelectNextLine;
    case KeyCommand::SelectNextRow: return sSelectNextRow;
    case KeyCommand::MoveToPreviousLine: return sMoveToPreviousLine;
    case KeyCommand::SelectPreviousLine: return sSelectPreviousLine;
    case KeyCommand::SelectPreviousRow: return sSelectPreviousRow;
    case KeyCommand::MoveToStartOfLine: return sMoveToStartOfLine;
    case KeyCommand::SelectStartOfLine: return sSelectStartOfLine;
    case KeyCommand::MoveToEndOfLine: return sMoveToEndOfLine;
    case KeyCommand::SelectEndOfLine: return sSelectEndOfLine;
    case KeyCommand::MoveToPreviousPage: return sMoveToPreviousPage;
    case KeyCommand::SelectPreviousPage: return sSelectPreviousPage;
    case KeyCommand::MoveToNextPage: return sMoveToNextPage;
    case KeyCommand::SelectNextPage: return sSelectNextPage;
    case KeyCommand::MoveToStartOfDocument: return sMoveToStartOfDocument;
    case KeyCommand::SelectStartOfDocument: return sSelectStartOfDocument;
    case KeyCommand::MoveToEndOfDocument: return sMoveToEndOfDocument;
    case KeyCommand::SelectEndOfDocument: return sSelectEndOfDocument;
    case KeyCommand::SelectAll: return sSelectAll;
    case KeyCommand::Copy: return sCopy;

    case KeyCommand::InsertText: return sInsertText;
    case KeyCommand::InsertImport: return sInsertImport;
    case KeyCommand::Backspace: return sBackspace;
    case KeyCommand::Delete: return sDelete;
    case KeyCommand::RemoveLine: return sRemoveLine;
    case KeyCommand::RemoveTail: return sRemoveTail;
    case KeyCommand::Cut: return sCut;
    case KeyCommand::Paste: return sPaste;
    default: return "";
    }
}

KeyCommand::Type loadKeyCommandType(const QString &s)
{
    if (s.toLower()==sMoveToNextChar.toLower()) return KeyCommand::MoveToNextChar;
    else if (s.toLower()==sSelectNextChar.toLower()) return KeyCommand::SelectNextChar;
    else if (s.toLower()==sSelectNextColumn.toLower()) return KeyCommand::SelectNextColumn;
    else if (s.toLower()==sMoveToPreviousChar.toLower()) return KeyCommand::MoveToPreviousChar;
    else if (s.toLower()==sSelectPreviousChar.toLower()) return KeyCommand::SelectPreviousChar;
    else if (s.toLower()==sSelectPreviousColumn.toLower()) return KeyCommand::SelectPreviousColumn;
    else if (s.toLower()==sMoveToNextLine.toLower()) return KeyCommand::MoveToNextLine;
    else if (s.toLower()==sSelectNextLine.toLower()) return KeyCommand::SelectNextLine;
    else if (s.toLower()==sSelectNextRow.toLower()) return KeyCommand::SelectNextRow;
    else if (s.toLower()==sMoveToPreviousLine.toLower()) return KeyCommand::MoveToPreviousLine;
    else if (s.toLower()==sSelectPreviousLine.toLower()) return KeyCommand::SelectPreviousLine;
    else if (s.toLower()==sSelectPreviousRow.toLower()) return KeyCommand::SelectPreviousRow;
    else if (s.toLower()==sMoveToStartOfLine.toLower()) return KeyCommand::MoveToStartOfLine;
    else if (s.toLower()==sSelectStartOfLine.toLower()) return KeyCommand::SelectStartOfLine;
    else if (s.toLower()==sMoveToEndOfLine.toLower()) return KeyCommand::MoveToEndOfLine;
    else if (s.toLower()==sSelectEndOfLine.toLower()) return KeyCommand::SelectEndOfLine;
    else if (s.toLower()==sMoveToPreviousPage.toLower()) return KeyCommand::MoveToPreviousPage;
    else if (s.toLower()==sSelectPreviousPage.toLower()) return KeyCommand::SelectPreviousPage;
    else if (s.toLower()==sMoveToNextPage.toLower()) return KeyCommand::MoveToNextPage;
    else if (s.toLower()==sSelectNextPage.toLower()) return KeyCommand::SelectNextPage;
    else if (s.toLower()==sMoveToStartOfDocument.toLower()) return KeyCommand::MoveToStartOfDocument;
    else if (s.toLower()==sSelectStartOfDocument.toLower()) return KeyCommand::SelectStartOfDocument;
    else if (s.toLower()==sMoveToEndOfDocument.toLower()) return KeyCommand::MoveToEndOfDocument;
    else if (s.toLower()==sSelectEndOfDocument.toLower()) return KeyCommand::SelectEndOfDocument;
    else if (s.toLower()==sSelectAll.toLower()) return KeyCommand::SelectAll;
    else if (s.toLower()==sCopy.toLower()) return KeyCommand::Copy;

    else if (s.toLower()==sInsertText.toLower()) return KeyCommand::InsertText;
    else if (s.toLower()==sInsertImport.toLower()) return KeyCommand::InsertImport;
    else if (s.toLower()==sBackspace.toLower()) return KeyCommand::Backspace;
    else if (s.toLower()==sDelete.toLower()) return KeyCommand::Delete;
    else if (s.toLower()==sRemoveLine.toLower()) return KeyCommand::RemoveLine;
    else if (s.toLower()==sRemoveTail.toLower()) return KeyCommand::RemoveTail;
    else if (s.toLower()==sCut.toLower()) return KeyCommand::Cut;
    else if (s.toLower()==sPaste.toLower()) return KeyCommand::Paste;

    else return KeyCommand::Type(0);
}


} // namespace Editor
