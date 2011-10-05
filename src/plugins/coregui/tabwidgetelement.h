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
    inline explicit TabWidgetElement(QWidget * w
                                     , bool enableToolBar
                                     , QList<QAction*> toolbarActions
                                     , QList<QMenu*> ms
                                     , QList<QWidget*> sws
                                     , MainWindow::DocumentType t
                                     , QActionGroup * gr_fileActions
                                     , QActionGroup * gr_otherActions
                                     , class KumirProgram * kumir
                                     , class PascalProgram * pascal
                                     )
        : QWidget()
        , component(w)
        , menus(ms)
        , statusbarWidgets(sws)
        , type(t)
        , documentId(-1)
    {
        Q_CHECK_PTR(w);
        Q_ASSERT(!QString::fromAscii(w->metaObject()->className()).isEmpty());
        setProperty("uncloseable", w->property("uncloseable"));
        setProperty("documentId", w->property("documentId"));
        setProperty("fileName", w->property("fileName"));
        setProperty("realFileName", w->property("realFileName"));
        setProperty("title", w->property("title"));
        documentId = w->property("documentId").toInt();
        if (type==MainWindow::WWW) {
            connect(w, SIGNAL(titleChanged(QString)), this, SIGNAL(changeTitle(QString)));
        }
        else {
            connect(w, SIGNAL(documentCleanChanged(bool)), this, SIGNAL(documentCleanChanged(bool)));
        }
        QVBoxLayout * l = new QVBoxLayout;
        l->setContentsMargins(0,0,0,0);
        l->setSpacing(0);
        setLayout(l);
        if (enableToolBar) {
            if (!toolbarActions.isEmpty()) {
                QToolBar * tb = new QToolBar(this);
#ifndef Q_OS_MAC
                tb->setStyleSheet("QToolBar { border: 0px }");
#endif
                l->addWidget(tb);
                if (type!=MainWindow::WWW) {
                    tb->addActions(gr_fileActions->actions());
                }
                tb->addSeparator();
                foreach (QAction * a, toolbarActions)
                    tb->addAction(a);
                if (type==MainWindow::Kumir) {
                    tb->addSeparator();
                    QList<QAction*> acts = kumir->actions()->actions();
                    for (int i=0; i<acts.size(); i++) {
                        if (acts[i]->isSeparator() || !acts[i]->icon().isNull())
                            tb->addAction(acts[i]);
                    }
                }
                if (type==MainWindow::Pascal) {
                    tb->addSeparator();
                    Q_UNUSED(pascal);
                    //                tb->addActions(pascal->actions()->actions());
                }
                if (!gr_otherActions->actions().isEmpty()) {
                    tb->addSeparator();
                    tb->addActions(gr_otherActions->actions());
                }
            }

        }
        l->addWidget(w);
    }
    QWidget * component;
    QList<QMenu*> menus;
    QList<QWidget*> statusbarWidgets;
    MainWindow::DocumentType type;
    int documentId;
signals:
    void changeTitle(const QString & txt);
    void documentCleanChanged(bool v);
protected:
    inline void focusInEvent(QFocusEvent *e) {
        QWidget::focusInEvent(e);
        component->setFocus();
    }

};
}

#endif // TABWIDGETELEMENT_H
