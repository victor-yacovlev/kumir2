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
    friend QDataStream & operator<< (QDataStream & stream, const InsertCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, InsertCommand & command);
public:
    explicit InsertCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::Analizer::InstanceInterface * analizer,
                           int line,
                           int pos,
                           const QString & text
                           );
    explicit InsertCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::Analizer::InstanceInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 1; }
    bool mergeWith(const QUndoCommand *other);

private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::Analizer::InstanceInterface * analizer;
    int line;
    int pos;
    QString text;
    int blankLines;
    int blankChars;
    int cursorRow;
    int cursorCol;
    int indentSize;
    bool hardIndents;

};

class RemoveCommand:
        public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream & stream, const RemoveCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, RemoveCommand & command);
public:
    explicit RemoveCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::Analizer::InstanceInterface * analizer,
                           int line,
                           int pos,
                           int count,
                           bool keepCursor,
                           int newCursorRow,
                           int newCursorColumn
                           );
    explicit RemoveCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::Analizer::InstanceInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 2; }
    bool mergeWith(const QUndoCommand *other);
private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::Analizer::InstanceInterface * analizer;
    int line;
    int pos;
    int count;
    bool keepKursor;
    QString removedText;
    int insertedSpaces;
    int cursorRow;
    int cursorCol;
    int cursorRowAfter;
    int cursorColAfter;
    int indentSize;
    bool hardIndents;
};

class InsertBlockCommand:
        public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream & stream, const InsertBlockCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, InsertBlockCommand & command);
public:
    explicit InsertBlockCommand(class TextDocument * doc,
                                class TextCursor * cursor,
                                Shared::Analizer::InstanceInterface * analizer,
                                int row,
                                int column,
                                const QStringList & block);
    explicit InsertBlockCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::Analizer::InstanceInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 3; }

private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::Analizer::InstanceInterface * analizer;
    int row;
    int column;
    QStringList block;
    int cursorRow;
    int cursorCol;
    QStringList previousLines;
    int addedLines;
    int indentSize;
    bool hardIndents;
};

class RemoveBlockCommand:
        public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream & stream, const RemoveBlockCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, RemoveBlockCommand & command);
public:
    explicit RemoveBlockCommand(class TextDocument * doc,
                                class TextCursor * cursor,
                                Shared::Analizer::InstanceInterface * analizer,
                                const QRect & block);
    explicit RemoveBlockCommand(class TextDocument * doc,
                           class TextCursor * cursor,
                           Shared::Analizer::InstanceInterface * analizer);
    void redo();
    void undo();
    inline int id() const { return 4; }

private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::Analizer::InstanceInterface * analizer;
    QRect block;
    int cursorRow;
    int cursorCol;
    QStringList previousLines;
    int indentSize;
    bool hardIndents;
};

class InsertImportCommand
    : public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream &, const InsertImportCommand&);
    friend QDataStream & operator>> (QDataStream &, InsertImportCommand&);
public:
    explicit InsertImportCommand(
            class TextDocument * document,
            class TextCursor * cursor,
            Shared::Analizer::InstanceInterface * analizer,
            const QString & importName
            );

    void redo();
    void undo();
    inline int id() const { return 5; }

private:
    class TextDocument * document_;
    class TextCursor * cursor_;
    Shared::Analizer::InstanceInterface * analizer_;
    QString importName_;
    uint lineNo_;
    int indentSize;
    bool hardIndents;
};

class ToggleLineProtectedCommand:
        public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream & stream, const ToggleLineProtectedCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, ToggleLineProtectedCommand & command);
public:
    explicit ToggleLineProtectedCommand(class TextDocument * Doc, int lineNo);
    void redo();
    void undo();
    inline int id() const { return 0xA0; }
private:
    class TextDocument * doc;
    int lineNo;

};

class ChangeHiddenLineDelimeterCommand:
        public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream & stream, const ChangeHiddenLineDelimeterCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, ChangeHiddenLineDelimeterCommand & command);
public:
    explicit ChangeHiddenLineDelimeterCommand(class TextDocument * doc,
                                              int firstHiddenLineNo);
    void redo();
    void undo();
private:
    class TextDocument * doc;
    struct KumFile::Data prevData;
    int firstHiddenLineNo;
};

class ToggleCommentCommand:
        public QUndoCommand
{
    friend QDataStream & operator<< (QDataStream & stream, const ToggleCommentCommand & command);
    friend QDataStream & operator>> (QDataStream & stream, ToggleCommentCommand & command);
public:
    explicit ToggleCommentCommand(
        class TextDocument * Doc,
        uint FromLineInclusive,
        uint ToLineInclusive,
        class TextCursor * cursor,
        Shared::Analizer::InstanceInterface * analizer
        );
    void redo();
    void undo();
    inline int id() const { return 0xB0; }
private:
    class TextDocument * doc;
    class TextCursor * cursor;
    Shared::Analizer::InstanceInterface * analizer;
    uint fromLineInclusive;
    uint toLineInclusive;
    QSet<int> commentedLines;
    QSet< QPair<int,int> > uncommentedLines;
    int indentSize;
};

}

#endif // EDITCOMMANDS_H
