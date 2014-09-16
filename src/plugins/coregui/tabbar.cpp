#include "tabbar.h"

namespace CoreGUI {

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setTabsClosable(true);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
#ifdef Q_OS_MAC
    static const char * css =
//            "QTabBar::tab {"
//            "  min-width: 18ex;"
//            "  text-align: right;"
//            "}"
//            "QTabBar::tab:selected {"
//            "  background-color: $windowColor;"
//            "}"
            "QTabBar::close-button {"
            "  image: url(:/coregui/close-tab.png);"
            "}"
            "QTabBar::close-button:hover {"
            "  image: url(:/coregui/close-tab-hovered.png);"
            "}"
            ;
#else
    static const char * css =
            "QTabBar::tab {"
            "  min-width: 18ex;"
            "  height: 28px;"
            "  text-align: right;"
            "}"
            "QTabBar::tab:selected {"
            "}"
            "QTabBar::close-button {"
            "  image: url(:/coregui/close-tab.png);"
            "}"
            "QTabBar::close-button:hover {"
            "  image: url(:/coregui/close-tab-hovered.png);"
            "}"
            ;
#endif
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setStyleSheet(QString::fromLatin1(css).replace("$windowColor", palette().brush(QPalette::Window).color().name()));
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(handleChanged(int)));
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    v_activeIcons = QVector<QIcon>(10);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    v_normalIcons = QVector<QIcon>(10);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    v_actions = QVector<QAction*>(10);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
#ifndef Q_OS_MAC
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    for (int i=0; i<10; i++) {
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        const QString text = i>0? QString::number(i) : "~";
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QFont f(font());
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        f.setPixelSize(10);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        f.setBold(true);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        int w = QFontMetrics(f).width(text);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QImage numberImageActive(16,16,QImage::Format_ARGB32);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        numberImageActive.fill(0);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QPainter p(&numberImageActive);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        p.setPen(palette().brush(QPalette::HighlightedText).color());
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        p.setBrush(palette().brush(QPalette::Highlight));
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        p.drawRect(2,2,12,12);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        p.setFont(f);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        p.drawText(3+(12-w)/2, 12, text);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        p.end();
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QImage numberImage(16,16,QImage::Format_ARGB32);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        numberImage.fill(0);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QPainter pp(&numberImage);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        pp.setPen(palette().brush(QPalette::WindowText).color());
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        pp.setBrush(Qt::NoBrush);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        pp.drawRect(2,2,12,12);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        pp.setFont(f);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        pp.drawText(3+(12-w)/2, 12, text);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        pp.end();
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        v_normalIcons[i] = QIcon(QPixmap::fromImage(numberImage));
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
//        v_activeIcons[i] = QIcon(QPixmap::fromImage(numberImageActive));
        v_activeIcons[i] = v_normalIcons[i];
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QAction * toggleView = new QAction(this);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

        if (i==0) {
            // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            toggleView->setShortcut(QKeySequence("Ctrl+`"));
            // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        }
        else {
            // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            toggleView->setShortcut(QKeySequence(QString("Ctrl+%1").arg(i)));
            // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        }
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        toggleView->setShortcutContext(Qt::ApplicationShortcut);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        toggleView->setProperty("tabIndex", i);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        connect(toggleView, SIGNAL(triggered()), this, SLOT(switchToTab()));
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        addAction(toggleView);
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        v_actions[i] = toggleView;
        // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }
#endif
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setIconSize(QSize(16,16));
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
}


void TabBar::tabInserted(int index)
{
    QTabBar::tabInserted(index);
    handleChanged(currentIndex());
}

void TabBar::tabRemoved(int index)
{
    QTabBar::tabRemoved(index);
    handleChanged(currentIndex());
}

void TabBar::switchToTab()
{
    int index = sender()->property("tabIndex").toInt();
    if (index>=0 && index<count())
        setCurrentIndex(index);
}

void TabBar::handleChanged(int index)
{
    if (tabButton(0, QTabBar::RightSide)) {
        tabButton(0, QTabBar::RightSide)->resize(QSize(0,0));
        tabButton(0, QTabBar::RightSide)->setVisible(false);
    }
    if (tabButton(0, QTabBar::LeftSide)) {
        tabButton(0, QTabBar::LeftSide)->resize(0, 0);
        tabButton(0, QTabBar::LeftSide)->setVisible(false);
    }
#ifndef Q_OS_MAC
    for (int i=0; i<qMin(count(),10); i++) {
        if (i!=index)
            setTabIcon(i, v_normalIcons[i]);
        if (i==0)
            setTabToolTip(i, tr("<b>Ctrl+%1</b> activates this tab").arg("~"));
        else
            setTabToolTip(i, tr("<b>Ctrl+%1</b> activates this tab").arg(i));
    }
#endif
    for (int i=10; i<count(); i++)
        setTabToolTip(i, "");
    if (index<10 && index>=0)
        setTabIcon(index, v_activeIcons[index]);
}

} // namespace CoreGUI
