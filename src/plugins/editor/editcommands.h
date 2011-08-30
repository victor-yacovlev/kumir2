#ifndef EDITCOMMANDS_H
#define EDITCOMMANDS_H

#include <QtCore>
#include <QtGui>
#include "interfaces/analizerinterface.h"
#include "dataformats/kumfile.h"

namespace Editor {

class InsertCommand:
        public QUndoCommand
{
public:
    explicit InsertCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::AnalizerInterface * analizer,
                           int line,
                           int pos,
                           const QString & text
                           );
    explicit InsertCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::AnalizerInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 1; }
    bool mergeWith(const QUndoCommand *other);

//private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::AnalizerInterface * analizer;
    int line;
    int pos;
    QString text;
    int blankLines;
    int blankChars;
    int cursorRow;
    int cursorCol;

};

class RemoveCommand:
        public QUndoCommand
{
public:
    explicit RemoveCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::AnalizerInterface * analizer,
                           int line,
                           int pos,
                           int count,
                           bool keepCursor
                           );
    explicit RemoveCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::AnalizerInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 2; }
    bool mergeWith(const QUndoCommand *other);

//private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::AnalizerInterface * analizer;
    int line;
    int pos;
    int count;
    bool keepKursor;
    QString removedText;
    int insertedSpaces;
    int cursorRow;
    int cursorCol;

};

class InsertBlockCommand:
        public QUndoCommand
{
public:
    explicit InsertBlockCommand(class TextDocument * doc,
                                class TextCursor * cursor,
                                Shared::AnalizerInterface * analizer,
                                int row,
                                int column,
                                const QStringList & block);
    explicit InsertBlockCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::AnalizerInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 3; }

//private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::AnalizerInterface * analizer;
    int row;
    int column;
    QStringList block;
    int cursorRow;
    int cursorCol;
    QStringList previousLines;
    int addedLines;
};

class RemoveBlockCommand:
        public QUndoCommand
{
public:
    explicit RemoveBlockCommand(class TextDocument * doc,
                                class TextCursor * cursor,
                                Shared::AnalizerInterface * analizer,
                                const QRect & block);
    explicit RemoveBlockCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::AnalizerInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 4; }

//private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::AnalizerInterface * analizer;
    QRect block;
    int cursorRow;
    int cursorCol;
    QStringList previousLines;
};

class ToggleLineProtectedCommand:
        public QUndoCommand
{
public:
    explicit ToggleLineProtectedCommand(class TextDocument * Doc, int lineNo);
    void redo();
    void undo();
    inline int id() const { return 0xA0; }
// private:
    class TextDocument * doc;
    int lineNo;

};

class ChangeHiddenLineDelimeterCommand:
        public QUndoCommand
{
public:
    explicit ChangeHiddenLineDelimeterCommand(class TextDocument * doc,
                                              int firstHiddenLineNo);
    void redo();
    void undo();
// private:
    class TextDocument * doc;
    struct KumFile::Data prevData;
    int firstHiddenLineNo;
};



QDataStream & operator<< (QDataStream & stream, const InsertCommand & command);
QDataStream & operator<< (QDataStream & stream, const RemoveCommand & command);
QDataStream & operator<< (QDataStream & stream, const InsertBlockCommand & command);
QDataStream & operator<< (QDataStream & stream, const RemoveBlockCommand & command);
QDataStream & operator<< (QDataStream & stream, const ToggleLineProtectedCommand & command);
QDataStream & operator<< (QDataStream & stream, const ChangeHiddenLineDelimeterCommand & command);

QDataStream & operator>> (QDataStream & stream, InsertCommand & command);
QDataStream & operator>> (QDataStream & stream, RemoveCommand & command);
QDataStream & operator>> (QDataStream & stream, InsertBlockCommand & command);
QDataStream & operator>> (QDataStream & stream, RemoveBlockCommand & command);
QDataStream & operator>> (QDataStream & stream, ToggleLineProtectedCommand & command);
QDataStream & operator>> (QDataStream & stream, ChangeHiddenLineDelimeterCommand & command);

}

#endif // EDITCOMMANDS_H
