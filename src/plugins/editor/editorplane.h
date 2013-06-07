#ifndef EDITORPLANE_H
#define EDITORPLANE_H

#include <QtGui>

#include "interfaces/lexemtype.h"
#include "interfaces/analizerinterface.h"


namespace Editor {

class EditorPlane : public QWidget
{
    friend class Editor;
    Q_OBJECT
    Q_PROPERTY(qreal dontEditState READ dontEditState WRITE setDontEditState)
public:
    explicit EditorPlane(class TextDocument * doc
                         , Shared::AnalizerInterface * analizer
                         , class Editor * editor
                         , class TextCursor * cursor
                         , class Clipboard * clipboard
                         , QSettings * settings
                         , QScrollBar * horizontalSB
                         , QScrollBar * verticalSB
                         , bool hasAnalizer
                         , QWidget *parent = 0);
    uint widthInChars() const;
    uint charWidth() const;
    uint lineHeight() const;
    inline int marginCharactersCount() const {
        return settings_->value(MarginWidthKey, MarginWidthDefault).toInt();
    }
    inline qreal dontEditState() const { return dontEditImageOpacity_; }
    void setDontEditState(qreal v) { dontEditImageOpacity_ = v; update(); }
    QRect cursorRect() const;
    uint marginLeftBound() const;
    QRect marginBackgroundRect() const;
    QRect marginLineRect() const;
    uint normalizedNewMarginLinePosition(uint x) const;
    static QString MarginWidthKey;
    static uint MarginWidthDefault;
    void setTeacherMode(bool v);
    inline bool isTeacherMode() const { return teacherModeFlag_; }
    void addContextMenuAction(QAction * a);
public slots:
    void selectAll();
    void copy();
    void paste();
    void cut();
    void removeLine();
    void removeLineTail();
    void setLineHighlighted(int lineNo, const QColor & color, quint32 colStart, quint32 colEnd);
    void signalizeNotEditable();
protected:
    static QPolygon errorUnderline(int x, int y, int len);
    void dragEventHandler(QDragMoveEvent * e);
    void wheelEvent(QWheelEvent *);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *e);
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    bool canDrop(const QPoint &pos, const QMimeData * data) const;
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool eventFilter(QObject *, QEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void initMouseCursor();
    void updateScrollBars();
    void findCursor();
    void ensureCursorVisible();
    void ensureHighlightedLineVisible();
//    void updateOffset();
    void paintEvent(QPaintEvent *);
    bool event(QEvent *e);
    void paintLockSymbol(QPainter * p, bool colored, const QRect & r);
    QPoint offset() const;
    void paintBackground(QPainter *p, const QRect &rect);
    void paintMarginBackground(QPainter *p, const QRect & rect);

    void paintMarginText(QPainter *p, const QRect & rect);
    void paintCursor(QPainter *p, const QRect &rect);
    void paintText(QPainter *p, const QRect &rect);
    void paintHiddenTextDelimeterLine(QPainter *p);
    void paintSelection(QPainter *p, const QRect &rect);
    void paintRectSelection(QPainter *p, const QRect &rect);
    void paintLineNumbers(QPainter *p, const QRect &rect);
    void paintNewMarginLine(QPainter *p);
    void paintNewHiddenDelimeterLine(QPainter *p);
    void paintDropPosition(QPainter *p);
    void keyPressEvent(QKeyEvent *);
    void doAutocomplete();
    void keyReleaseEvent(QKeyEvent *);
    void setProperFormat(QPainter * p, Shared::LexemType type, const QChar &c);
protected slots:
    void updateCursor();
    void updateText(int fromLine, int toLine);
    void finishAutoCompletion(const QString & suggession);


private:
    int timerId_;
    class TextDocument * document_;
    class TextCursor * cursor_;
    class Clipboard * clipboard_;
    class Editor * editor_;

    QSettings * settings_;
    QScrollBar * verticalScrollBar_;
    QScrollBar * horizontalScrollBar_;
    QPoint marginMousePressedPoint_;
    QPoint delimeterRuleMousePressedPoint_;
    QPoint textMousePressedPoint_;
    QPoint textPressedPosition_;
    bool selectionInProgressFlag_;

    QPoint pnt_dropPosMarker;
    QPoint pnt_dropPosCorner;
    int marginBackgroundAlpha_;
    bool hasAnalizerFlag_;
    bool teacherModeFlag_;
    QImage dontEditImage_;
    qreal dontEditImageOpacity_;
    QPropertyAnimation * doNotEditAnimation_;

    int highlightedTextLineNumber_;
    QColor highlightedTextLineColor_;
    int highlightedLockSymbolLineNumber_;
    quint32 highlightedTextColumnStartNumber_;
    quint32 highlightedTextColumnEndNumber_ ;
    class SuggestionsWindow * autocompleteWidget_;
    Shared::AnalizerInterface * analizer_;
    QList<QAction*> contextMenuActions_;
signals:
    void urlsDragAndDropped(const QList<QUrl> &);
    void requestAutoScroll(char a);
    void requestAutoScrollX(char a);
    void disableInsertActions();
    void enableInsertActions();

};

} // namespace Editor

#endif // EDITORPLANE_H
