#ifndef TERMINAL_TERMINAL_H
#define TERMINAL_TERMINAL_H

#include <QtCore>
#include <QtGui>

#include "extensionsystem/kplugin.h"

namespace Terminal {

class Term : public QWidget
{
    friend class Plane;
    Q_OBJECT
public:
    explicit Term(QWidget *parent = 0);
signals:
    void openTextEditor(const QString & suggestedFileName, const QString & text);
    void message(const QString &);
    void inputFinished(const QVariantList &);
    void showWindowRequest();
public slots:
    void clear();
    void start(const QString &fileName);
    void finish();
    void terminate();
    void output(const QString & text);
    void error(const QString & message);
    void input(const QString & format);
    void saveLast();
    void saveAll();
    void editLast();
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);

protected:
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void resizeEvent(QResizeEvent *);
    void saveText(const QString & suggestedFileName, const QString & text);

protected slots:
    void handleInputTextChanged(const QString & text);
    void handleInputCursorPositionChanged(quint16 pos);
    void handleInputFinishRequested();
    void handleInputDone();

private:
    QList<class OneSession*> l_sessions;
    QScrollBar * sb_vertical;
    QScrollBar * sb_horizontal;
    class Plane * m_plane;
    QAction * a_saveAll;
    QAction * a_saveLast;
    QAction * a_editLast;
    QAction * a_clear;
    QToolBar * m_toolBar;
    QGridLayout * m_layout;


};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_H
