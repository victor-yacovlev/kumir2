#include "secondarywindow.h"
#include "secondarywindow_impl.h"

#include "dockwindowplace.h"
#include "dockwindowplace_impl.h"

namespace Widgets {

class SecondaryWindow;


SecondaryWindow::SecondaryWindow(QWidget *centralComponent,
                                 DockWindowPlace * dockPlace,
                                 QMainWindow * mainWindow,
                                 ExtensionSystem::SettingsPtr settings,
                                 const QString &settingsKey)
    : QWidget(PARENT, REGULAR_FLAGS)
{    
    pImpl_.reset(new SecondaryWindowImpl);
    pImpl_->pClass_ = this;
    dockPlace->registerWindowHere(this);
    pImpl_->init(centralComponent,
                 dockPlace->pImpl_,
                 mainWindow,
                 settings, settingsKey,
                 true, true);
}

SecondaryWindow::SecondaryWindow(QWidget *centralComponent,
                                 QMainWindow * mainWindow,
                                 ExtensionSystem::SettingsPtr settings,
                                 const QString &settingsKey)
    : QWidget(PARENT, REGULAR_FLAGS)
{
    pImpl_.reset(new SecondaryWindowImpl);
    pImpl_->pClass_ = this;
    pImpl_->init(centralComponent,
                 QSharedPointer<class DockWindowPlaceImpl>(),
                 mainWindow,
                 settings, settingsKey,
                 false, false);
}

SecondaryWindow::~SecondaryWindow()
{

}

void SecondaryWindow::restoreState()
{
    const QVariant fl = pImpl_->settings_->value(pImpl_->settingsKey_+"/Floating");
    bool f = fl.isValid() ? fl.toBool() : true;
    if (f) {
        const QRect r = pImpl_->settings_->value(
                    pImpl_->settingsKey_+"/WindowRect", QRect()
                    ).toRect();
        QPoint position;
        if (r.isValid()) {
            resize(r.size());
            position = r.topLeft();
        }
        else {
            int screenNumber = QApplication::desktop()->screenNumber(pImpl_->mainWindow_);
            const QPoint screenCenter = QApplication::desktop()->screenGeometry(screenNumber).center();
            position = screenCenter - QPoint(width()/2, height()/2);
        }
#ifdef Q_OS_MAC
        static const int MenuBarOffset = 24;
#else
        static const int MenuBarOffset = 0;
#endif
        if (position.y() < MenuBarOffset) {
            position.setY(MenuBarOffset);
        }
        int screenNumber = QApplication::desktop()->screenNumber(pImpl_->mainWindow_);
        const QRect screenRect = QApplication::desktop()->screenGeometry(screenNumber);
        const QPoint screenCenter = QApplication::desktop()->screenGeometry(screenNumber).center();
        qDebug() << "Screen rect: " << screenRect;
        if (position.x() + width() > screenRect.right()) {
            position = screenCenter - QPoint(width()/2, height()/2);
        }
        if (position.y() + height() > screenRect.bottom()) {
            position = screenCenter - QPoint(width()/2, height()/2);
        }
        move(position);
        qDebug() << "Window " << pImpl_->settingsKey_ << " position: " << position;
    }
    else {
        bool visible = pImpl_->settings_->value(pImpl_->settingsKey_+"/Visible", false).toBool();
        setVisible(visible);
    }
    pImpl_->setFloating(f);
}

void SecondaryWindow::saveState()
{
    pImpl_->settings_->setValue(pImpl_->settingsKey_ + "/Floating",
                                isFloating());
    if (isFloating()) {
        const QRect r(x(), y(), width(), height());
        pImpl_->settings_->setValue(pImpl_->settingsKey_+"/WindowRect", r);
    }

        pImpl_->settings_->setValue(pImpl_->settingsKey_ + "/Visible",
                                isVisible());

}

QSize SecondaryWindow::minimumSizeHint() const
{
    const QSize minimumChildSize = pImpl_->centralWidget_->minimumSizeHint();

    const QSize myMinimumSize = minimumChildSize +
            QSize(pImpl_->leftBorder_->width(), 0) +
            QSize(pImpl_->rightBorder_->width(), 0) +
            QSize(0, pImpl_->topBorder_->height()) +
            QSize(0, pImpl_->bottomBorder_->height());

    // Reasonable minimum size in case if there is no size hints
    const QSize reasonableMinumumSize = QSize(
                qMax(300, myMinimumSize.width()),
                qMax(200, myMinimumSize.height())
                );

    return reasonableMinumumSize;
}

void SecondaryWindow::setVisible(bool visible)
{
    pImpl_->visibleFlag_ = visible;
    QWidget::setVisible(visible);
}

void SecondaryWindow::setWindowTitle(const QString &title)
{
    pImpl_->toggleVisibleAction_->setText(title);
    QWidget::setWindowTitle(title);
}

QString SecondaryWindow::windowTitle()
{
    return QWidget::windowTitle();
}

void SecondaryWindow::setStayOnTop(bool v)
{
    if (v) {
        setWindowFlags(STAY_ON_TOP_FLAGS);
        setVisible(true);
    }
    else {
        setWindowFlags(REGULAR_FLAGS);
        setVisible(true);
    }
    pImpl_->centralWidget_->setFocus();
}


bool SecondaryWindow::isStayOnTop()
{
    return windowFlags() & Qt::WindowStaysOnTopHint;
}

void SecondaryWindow::close()
{  
    setVisible(false);
    if (!isFloating()) {
        pImpl_->dockPlace_->undockWindow(this);
    }
    saveState();
}

void SecondaryWindow::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
}


QAction * SecondaryWindow::toggleViewAction() const
{
    return pImpl_->toggleVisibleAction_;
}


void SecondaryWindow::showMinimized()
{
    QWidget::showMinimized();
}

void SecondaryWindow::activate()
{
    if (!isVisible())
        setVisible(true);
    if (isFloating())
        activateWindow();
    else {
        pImpl_->dockPlace_->dockWindow(this, true);
    }
}


bool SecondaryWindow::isFloating() const
{
    return pImpl_->floating_;
}

void SecondaryWindow::toggleDocked()
{
    SecondaryWindowButton * btn = static_cast<SecondaryWindowButton*>(pImpl_->buttonToggleDocked_);
    btn->forceUnhighlight();
    if (!pImpl_->dockPlace_)
        return;
    if (isFloating()) {
        setVisible(true);
        pImpl_->setFloating(false);
        setVisible(true);
    }
    else {
        pImpl_->setFloating(true);
        activate();
    }
}

void SecondaryWindow::setSettingsObject(ExtensionSystem::SettingsPtr settings)
{
    pImpl_->settings_ = settings;
    restoreState();
}



}
