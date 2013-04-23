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

    struct Margin {
        QStringList errors;
        QString text;
        QColor color;
        QList<bool> selected;
        bool lineEndSelected;
    } margin;

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
    friend class ToggleCommentCommand;
    friend class TextCursor;
    friend class InsertImportCommand;
public:
    // Flag, set on restore session.
    // If present, do not actually undo/redo
    // while pushing actions into undo-stack.
    // Becomes false after session restore done.
    static bool noUndoRedo;
    inline TextLine & at(int index) { return data_[index]; }
    inline const TextLine & at(int index) const { return data_[index]; }
    explicit TextDocument(QObject * parent, QSettings * settings);
    int id_;
    uint indentAt(uint lineNo) const;
    inline void setAnalizer(Shared::AnalizerInterface * a) { analizer_ = a; }
    inline bool isProtected(uint lineNo) const {
        return lineNo < uint(data_.size())
                ? data_[lineNo].protecteed : false;
    }
    inline void setProtected(int lineNo, bool v) { data_[lineNo].protecteed = v; }
    inline bool isHidden(uint lineNo) const {
        return lineNo < uint(data_.size())
                ? data_[lineNo].hidden : false;
    }
    inline void setHidden(int lineNo, bool v) { data_[lineNo].hidden = v; }
    int hiddenLineStart() const;
    inline uint linesCount() const { return uint(data_.size()); }
    KumFile::Data toKumFile() const;
    QString toHtml(int fromLine = -1, int toLine = -1) const;
    QString lineToHtml(int lineNo) const;
    QByteArray toRtf(uint fromLine, uint toLine) const;
    void setKumFile(const KumFile::Data & data_, bool showHiddenLines);
    void setPlainText(const QString & data_);
    inline const QString& textAt(uint index) const {
        if (index < uint(data_.size())) {
            return data_.at(index).text;
        }
        else {
            static const QString dummyString;
            return dummyString;
        }
    }
    const TextLine::Margin & marginAt(uint index) const;
    TextLine::Margin & marginAt(uint index);

    inline const QList<bool>& selectionMaskAt(uint index) const {
        if (index < uint(data_.size())) {
            return data_.at(index).selected;
        }
        else {
            static const QList<bool> dummySelectionMask;
            return dummySelectionMask;
        }
    }
    inline void setSelectionMaskAt(int index, const QList<bool> mask) { if (index>=0 && index<data_.size()) data_[index].selected = mask; }
    inline bool lineEndSelectedAt(int index) const { return index>=0 && index<data_.size()? data_[index].lineEndSelected : false; }
    inline const QList<Shared::LexemType>& highlightAt(uint index) const {
        if (index < uint(data_.size())) {
            return data_.at(index).highlight;
        }
        else {
            static const QList<Shared::LexemType> dummyHighlight;
            return dummyHighlight;
        }
    }
    inline void setIndentRankAt(int index, const QPoint & rank) { if (index>=0 && index<data_.size()) data_[index].indentStart = rank.x(); data_[index].indentEnd = rank.y(); }
    inline void setHighlightAt(int index, const QList<Shared::LexemType> & highlight) { if (index>=0 && index<data_.size()) data_[index].highlight = highlight; }

    inline void setSelected(int line, int pos, bool v) { if (line<data_.size()) data_[line].selected[pos] = v; }
    inline void setEndOfLineSelected(int line, bool v) { if (line<data_.size()) data_[line].lineEndSelected = v; }
    void evaluateCommand(const QUndoCommand & cmd);
    inline const QUndoStack * undoStack() const { return undoStack_; }
    inline QUndoStack * undoStack() { return undoStack_; }
    void removeSelection();
    void forceCompleteRecompilation();
    void flushChanges();
    void flushTransaction();
signals:
    void compilationRequest(const QStack<Shared::ChangeTextTransaction> & changes_);
    void completeCompilationRequest(const QStringList & visibleText,
                                    const QStringList & hiddenText,
                                    int hiddenBaseLine);
protected:
    void insertText(const QString & text, const Shared::AnalizerInterface * analizer, int line, int pos, int &blankLines, int &blankChars);
    void removeText(QString &removedText, const Shared::AnalizerInterface * analizer, int line, int pos, int  blankLines, int  blankChars, int count);
    void insertLine(const QString & text, const uint beforeLineNo);
    void removeLine(const uint lineNo);
    void checkForCompilationRequest(const QPoint & cursorPosition);
private:
    QSet<int> removedLines_;
    QSet<int> removedHiddenLines_;
    QStack<Shared::ChangeTextTransaction> changes_;
    QPoint lastCursorPos_;
    QUndoStack * undoStack_;
    QList<TextLine> data_;
    QString hiddenText_;
    bool wasHiddenTextFlag_;
    QSettings * settings_;
    Shared::AnalizerInterface * analizer_;
};

}

#endif // TEXTBLOCKDATA_H
