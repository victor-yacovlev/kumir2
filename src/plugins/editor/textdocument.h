#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QtCore>
#include <QtGui>

#include "interfaces/lexemtype.h"
#include "interfaces/analizerinterface.h"
#include "dataformats/kumfile.h"

namespace Editor {



struct TextLine
{
    inline explicit TextLine() {
        indentStart = indentEnd = 0;
        lineEndSelected = false;
        changed = false;
        inserted = false;
        protecteed = false;
        hidden = false;
    }
    int indentStart;
    int indentEnd;
    QList<Shared::LexemType> highlight;
    QList<bool> selected;
    bool lineEndSelected;
    bool protecteed;
    bool hidden;
    QString text;
    QStringList errors;
    QString marginText;
    bool changed;
    bool inserted;
};

class TextDocument
        : public QObject
{
    Q_OBJECT
    friend class InsertCommand;
    friend class RemoveCommand;
    friend class InsertBlockCommand;
    friend class RemoveBlockCommand;
public:
    // Flag, set on restore session.
    // If present, do not actually undo/redo
    // while pushing actions into undo-stack.
    // Becomes false after session restore done.
    static bool noUndoRedo;

    explicit TextDocument(QObject * parent);
    int documentId;
    int indentAt(int lineNo) const;
    inline bool isProtected(int lineNo) const { return data[lineNo].protecteed; }
    inline void setProtected(int lineNo, bool v) { data[lineNo].protecteed = v; }
    inline bool isHidden(int lineNo) const { return data[lineNo].hidden; }
    inline void setHidden(int lineNo, bool v) { data[lineNo].hidden = v; }
    int hiddenLineStart() const;
    inline int linesCount() const { return data.size(); }
    KumFile::Data toKumFile() const;
    void setKumFile(const KumFile::Data & data);
    inline QString textAt(int index) const { return data[index].text; }
    inline QString marginTextAt(int index) const { return data[index].marginText; }
    inline QList<bool> selectionMaskAt(int index) const { return data[index].selected; }
    inline void setSelectionMaskAt(int index, const QList<bool> mask) { data[index].selected = mask; }
    inline bool lineEndSelectedAt(int index) const { return data[index].lineEndSelected; }
    inline QList<Shared::LexemType> highlightAt(int index) { return data[index].highlight; }
    inline void setMarginTextAt(int index, const QString & text) { data[index].marginText = text; }
    inline void setIndentRankAt(int index, const QPoint & rank) { data[index].indentStart = rank.x(); data[index].indentEnd = rank.y(); }
    inline void setHighlightAt(int index, const QList<Shared::LexemType> & highlight) { data[index].highlight = highlight; }
    inline void setErrorsAt(int index, const QStringList & errs) { data[index].errors = errs; }
    inline QStringList errorsAt(int index) const { return data[index].errors; }
    inline void clearErrorsAt(int index) { data[index].errors.clear(); }

    inline void setSelected(int line, int pos, bool v) { data[line].selected[pos] = v; }
    inline void setEndOfLineSelected(int line, bool v) { data[line].lineEndSelected = v; }
    void evaluateCommand(const QUndoCommand & cmd);
    inline const QUndoStack * undoStack() const { return m_undoStack; }
    inline QUndoStack * undoStack() { return m_undoStack; }
    void removeSelection();
    void forceCompleteRecompilation();
    void flushChanges();
    void flushTransaction();
signals:
    void compilationRequest(const QStack<Shared::ChangeTextTransaction> & changes);
    void completeCompilationRequest(const QStringList & visibleText,
                                    const QStringList & hiddenText,
                                    int hiddenBaseLine);
protected:
    void insertText(const QString & text, const Shared::AnalizerInterface * analizer, int line, int pos, int &blankLines, int &blankChars);
    void removeText(QString &removedText, const Shared::AnalizerInterface * analizer, int line, int pos, int  blankLines, int  blankChars, int count);
    void checkForCompilationRequest(const QPoint & cursorPosition);
private:
    QSet<int> m_removedLines;
    QSet<int> m_removedHiddenLines;
    QStack<Shared::ChangeTextTransaction> changes;
    QPoint lastCursorPos;
    QUndoStack * m_undoStack;
    QList<TextLine> data;
};

}

#endif // TEXTBLOCKDATA_H
