#ifndef TABWIDGETELEMENT_H
#define TABWIDGETELEMENT_H

#include <QtCore>
#include <QtGui>
#include "mainwindow.h"

namespace CoreGUI {
class TabWidgetElement
        : public QWidget
{
    Q_OBJECT
public:
    explicit TabWidgetElement(QWidget * w
                                     , bool enableToolBar
                                     , QList<QAction*> toolbarActions
                                     , QList<QMenu*> ms
//                                     , QList<QWidget*> sws
                                     , MainWindow::DocumentType t
                                     , QActionGroup * gr_fileActions
                                     , QActionGroup * gr_otherActions
                                     , class KumirProgram * kumir
                                     );

    QWidget * component;
    QList<QMenu*> menus;
    MainWindow::DocumentType type;
    Shared::Editor::InstanceInterface * editorInstance;
    Shared::Browser::InstanceInterface * browserInstance;

    inline class KumirProgram * kumirProgram() { return kumirProgram_; }
    inline bool isCourseManagerTab() const { return courseManagerTab_; }
    inline void setCourseManagerTab(bool v) {
        courseManagerTab_ = v;
    }
    QString title() const;
    inline void setCourseTitle(const QString & title) { courseTitle_ = title; }

signals:
    void titleChanged(const QString &);
    void documentCleanChanged(bool v);
protected:
    inline void focusInEvent(QFocusEvent *e) {
        QWidget::focusInEvent(e);
        component->setFocus();
    }
protected slots:
    void setDocumentChangesClean(bool clean);

private:
    class KumirProgram * kumirProgram_;
    bool courseManagerTab_;
    QString courseTitle_;
    bool documentHasChanges_;
};
}

#endif // TABWIDGETELEMENT_H
