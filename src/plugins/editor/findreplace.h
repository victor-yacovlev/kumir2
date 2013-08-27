#ifndef EDITOR_FINDREPLACE_H
#define EDITOR_FINDREPLACE_H

#include <QWidget>

namespace Editor {

class TextDocument;
class TextCursor;

namespace Ui {
class FindReplace;
}

class FindReplace
        : public QWidget
{
    Q_OBJECT
    
public:
    explicit FindReplace(class Editor * editor);
    ~FindReplace();

public slots:
    void showFind();
    void showReplace();
    
private slots:
    void handleMoreButtonChecked(bool v);
    void updateLayout(bool replaceMode);

    void doFindFirst(const QString & text);
    void doFindNext();
    void doFindPrevious();
    void doReplace();
    void doReplaceAll();

    void handleSearchParameterChanged();
    void handleReturnPressed();

private:

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void closeEvent(QCloseEvent *e);

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);


    void findText(const QString & text,
                  const QPoint & from,
                  const QPoint & to,
                  bool patternFlag,
                  bool matchCaseFlag,
                  char direction
                  );

    void markFoundText(const uint lineNo,
                       const uint fromPos,
                       const uint toPos,
                       const char direction
                       );

    static QRegExp makeAPatternRegExp(QString s, const bool matchCaseFlag);

    void show();


    Ui::FindReplace *ui;
    class Editor * editor_;
};


} // namespace Editor
#endif // EDITOR_FINDREPLACE_H
