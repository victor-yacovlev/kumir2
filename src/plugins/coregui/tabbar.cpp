#include "tabbar.h"

namespace CoreGUI {

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
    setTabsClosable(true);
    static const char * css =
            "QTabBar::tab {"
            "  min-width: 18ex;"
            "  height: 28px;"
            "  text-align: right;"
            "}"
            "QTabBar::close-button {"
            "  image: url(:/coregui/close-tab.png);"
            "}"
            "QTabBar::close-button:hover {"
            "  image: url(:/coregui/close-tab-hovered.png);"
            "}"
            ;
    setStyleSheet(QString::fromAscii(css));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(handleChanged(int)));
    v_activeIcons = QVector<QIcon>(10);
    v_normalIcons = QVector<QIcon>(10);
    v_actions = QVector<QAction*>(10);
    for (int i=0; i<10; i++) {
        const QString text = i>0? QString::number(i) : "~";
        QFont f(font());
        f.setPixelSize(10);
        f.setBold(true);
        int w = QFontMetrics(f).width(text);

        QImage numberImageActive(16,16,QImage::Format_ARGB32);
        numberImageActive.fill(0);
        QPainter p(&numberImageActive);
        p.setPen(palette().brush(QPalette::HighlightedText).color());
        p.setBrush(palette().brush(QPalette::Highlight));
        p.drawRect(2,2,12,12);
        p.setFont(f);
        p.drawText(3+(12-w)/2, 12, text);
        p.end();
        QImage numberImage(16,16,QImage::Format_ARGB32);
        numberImage.fill(0);
        QPainter pp(&numberImage);
        pp.setPen(palette().brush(QPalette::WindowText).color());
        pp.setBrush(Qt::NoBrush);
        pp.drawRect(2,2,12,12);
        pp.setFont(f);
        pp.drawText(3+(12-w)/2, 12, text);
        pp.end();
        v_activeIcons[i] = QIcon(QPixmap::fromImage(numberImageActive));
        v_normalIcons[i] = QIcon(QPixmap::fromImage(numberImage));
        QAction * toggleView = new QAction(this);

        if (i==0) {
            toggleView->setShortcut(QKeySequence("Ctrl+`"));
        }
        else {
            toggleView->setShortcut(QKeySequence(QString("Ctrl+%1").arg(i)));
        }
        toggleView->setShortcutContext(Qt::ApplicationShortcut);
        toggleView->setProperty("tabIndex", i);
        connect(toggleView, SIGNAL(triggered()), this, SLOT(switchToTab()));
        addAction(toggleView);
        v_actions[i] = toggleView;
    }
    setIconSize(QSize(16,16));
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
    for (int i=0; i<qMin(count(),10); i++) {
        if (i!=index)
            setTabIcon(i, v_normalIcons[i]);
        if (i==0)
            setTabToolTip(i, tr("<b>Ctrl+%1</b> activates this tab").arg("~"));
        else
            setTabToolTip(i, tr("<b>Ctrl+%1</b> activates this tab").arg(i));
    }
    for (int i=10; i<count(); i++)
        setTabToolTip(i, "");
    if (index<10)
        setTabIcon(index, v_activeIcons[index]);
}

} // namespace CoreGUI
