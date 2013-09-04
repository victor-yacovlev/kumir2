#include "tabwidgetelement.h"

namespace CoreGUI {

TabWidgetElement::TabWidgetElement(QWidget * w
                                   , bool enableToolBar
                                   , QList<QAction*> toolbarActions
                                   , QList<QMenu*> ms
//                                     , QList<QWidget*> sws
                                   , MainWindow::DocumentType t
                                   , QActionGroup * gr_fileActions
                                   , QActionGroup * gr_otherActions
                                   , class KumirProgram * kumir
                                   )
    : QWidget()
    , component(w)
    , menus(ms)
//        , statusbarWidgets(sws)
    , type(t)
    , editorInstance(nullptr)
    , browserInstance(nullptr)
    , kumirProgram_(kumir)
    , courseManagerTab_(false)
{
    kumirProgram_ = nullptr;
    Q_CHECK_PTR(w);
    Q_ASSERT(!QString::fromAscii(w->metaObject()->className()).isEmpty());
    setProperty("uncloseable", w->property("uncloseable"));
    if (type==MainWindow::WWW) {
        connect(w, SIGNAL(titleChanged(QString)), this, SIGNAL(changeTitle(QString)));
    }
    else {
        connect(w, SIGNAL(documentCleanChanged(bool)), this, SIGNAL(documentCleanChanged(bool)));
        connect(w, SIGNAL(documentCleanChanged(bool)), this, SLOT(setDocumentChangesClean(bool)));
    }
    QVBoxLayout * l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    setLayout(l);
    if (enableToolBar) {
        if (!toolbarActions.isEmpty()) {
            QToolBar * tb = new QToolBar(this);
            tb->setIconSize(QSize(22, 22));
#ifdef Q_OS_MAC
            static const char * css = ""
                    "QToolBar {"
                    "   border: 0px;"
                    "   background-color: $windowColor;"
                    "   padding: 8px;"
                    "}"
                    "QToolButton {"
                    "   border: 0px;"
                    "}"
                    "";
#else
            static const char * css = ""
                    "QToolBar { border: 0px }";
#endif
            tb->setStyleSheet(QString::fromAscii(css).replace("$windowColor",palette().brush(QPalette::Window).color().name()));
            l->addWidget(tb);
            if (type!=MainWindow::WWW) {
                tb->addActions(gr_fileActions->actions());
            }
            tb->addSeparator();
            foreach (QAction * a, toolbarActions)
                tb->addAction(a);
            if (type==MainWindow::Program) {
                tb->addSeparator();
                QList<QAction*> acts = kumir->actions()->actions();
                for (int i=0; i<acts.size(); i++) {
                    if (acts[i]->isSeparator() || !acts[i]->icon().isNull())
                        tb->addAction(acts[i]);
                }
            }
            if (!gr_otherActions->actions().isEmpty()) {
                tb->addSeparator();
                tb->addActions(gr_otherActions->actions());
            }
        }

    }
    l->addWidget(w);
}

QString TabWidgetElement::title() const
{
    if (editorInstance) {
        const KumFile::Data data = editorInstance->documentContents();
        const QUrl url = data.sourceUrl;
        if (url.isValid()) {
            const QString fullPath = editorInstance->documentContents().sourceUrl.toLocalFile();
            const QString shortPath = QFileInfo(fullPath).fileName();
            return shortPath;
        }
        else if (isCourseManagerTab()) {
            return tr("%1 (Course)").arg(courseTitle_).trimmed();
        }
        else {
            return "";
        }
    }
    else if (browserInstance) {
        return browserInstance->title();
    }
    else {
        return "";
    }
}


}

