#include "row.h"
#include "widgets/dockwindowplace.h"

#include <QDebug>
#include <QResizeEvent>

namespace CoreGUI {

Row::Row(QWidget *parent, const QString &settingsKey)
    : QSplitter(Qt::Horizontal, parent)
    , settingsKey_(settingsKey)
{
    setHandleWidth(10);
    setAutoFillBackground(true);
}

void Row::updateSettings(ExtensionSystem::SettingsPtr settings)
{
//    if (settings_) save();
    settings_ = settings;
    for (int i=0; i<count(); i++) {
        QWidget * w = widget(i);
        Widgets::DockWindowPlace * place =
                qobject_cast<Widgets::DockWindowPlace*>(w);
        if (place) {
            place->updateSettings(settings);
        }
    }
    restore();
}

void Row::addComponent(QWidget *widget, bool autoResizable)
{
    addWidget(widget);
    autoResizable_.push_back(autoResizable);
    setCollapsible(count()-1, !autoResizable);
    updateGeometry();
}

void Row::save()
{
    for (int i=0; i<count(); i++) {
        QWidget * w = widget(i);
        Widgets::DockWindowPlace * place =
                qobject_cast<Widgets::DockWindowPlace*>(w);
        if (place) {
            place->saveState();
        }
    }
    QList<int> szs = sizes();
    for (int i=0; i<szs.size(); i++) {
        settings_->setValue(settingsKey_+"/Size" + QString::number(i), szs[i]);
    }
}

void Row::restore()
{
    for (int i=0; i<count(); i++) {
        QWidget * w = widget(i);
        Widgets::DockWindowPlace * place =
                qobject_cast<Widgets::DockWindowPlace*>(w);
        if (place) {
            place->restoreState();
        }
    }
    QList<int> szs;
    for (int i=0; i<10; i++) {
        const QString key = settingsKey_+"/Size" + QString::number(i);
        const QVariant value = settings_->value(key);
        if (value.isValid())
            szs.push_back(value.toInt());
        else
            break;
    }
    if (szs.size() > 0)
        setSizes(szs);
}

QSize Row::sizeHint() const
{
    int w = (count() - 1) * handleWidth();
    int h = 0;
    for (int i=0; i<sizes().size(); i++) {
        w += sizes().at(i);
        h = qMax(h, widget(i)->height());
    }
    return QSize(w, h);
}

QSize Row::minimumSizeHint() const
{
    int w = (count() - 1) * handleWidth();
    int h = 0;
    for (int i=0; i<count(); i++) {
        const QSize sz = widget(i)->minimumSizeHint();
        w += sz.width();
        h = qMax(h, sz.height());
    }
    return QSize(w, h);
}

void Row::handleVisiblityRequest(bool visible, const QSize & size)
{
    QWidget * component = qobject_cast<QWidget*>(sender());
    if (visible)
        ensureEnoughtSpaceForComponent(component, size);
    else
        releaseSpaceUsesByComponent(component);

    if (visible) {
        emit visiblityRequest();
    }
}

void Row::ensureEnoughtSpaceForComponent(QWidget *component, const QSize &size)
{
    int index = indexOf(component);
    QList<int> szs = sizes();
    int summ = 0;
    for (int i=0; i<szs.size(); i++) {
        summ += szs[i];
    }
    int w = size.isValid() ? size.width() : component->minimumSizeHint().width();
    int diff = w - szs[index];
    if (diff <= 0)
        return;

    // 1. Try to decrease width of auto-resizable component
    int resizableIndex = autoResizable_.indexOf(true);
    if (resizableIndex != -1 && resizableIndex != index) {
        int minW = widget(resizableIndex)->minimumSizeHint().width();
        int curW = szs[resizableIndex];
        int space = curW - minW;
        if (space > 0) {
            int spaceToEat = qMin(space, diff);
            szs[resizableIndex] = szs[resizableIndex] - spaceToEat;
            diff -= spaceToEat;
        }
    }

    // 2. Try to decrease all other component width
    if (diff > 0) {
        for (int i=0; i<szs.size(); i++) {
            if (i!=resizableIndex && i!=index) {
                int minW = widget(i)->minimumSizeHint().width();
                int curW = szs[i];
                int space = curW - minW;
                if (space > 0) {
                    int spaceToEat = qMin(space, diff);
                    szs[i] = szs[i] - spaceToEat;
                    diff -= spaceToEat;
                }
            }
        }
    }

    // 3. Try to decrease component
    if (diff > 0 && w > component->minimumSizeHint().width()) {
        int space = w - component->minimumSizeHint().width();
        int spaceToEat = qMin(diff, space);
        w -= spaceToEat;
        diff -= spaceToEat;
    }

    szs[index] = w;
    setSizes(szs);
}

void Row::releaseSpaceUsesByComponent(QWidget *component)
{
    int index = indexOf(component);
    QList<int> szs = sizes();
    int freed = szs[index];
    szs[index] = 0;
    int resizableIndex = autoResizable_.indexOf(true);
    if (resizableIndex != -1 && resizableIndex != index) {
        szs[resizableIndex] = szs[resizableIndex] + freed + handleWidth();
    }
    setSizes(szs);
}

void Row::resizeEvent(QResizeEvent *event)
{
    if (event->size().width() < minimumSizeHint().width() || event->size().height() < minimumSizeHint().height()) {
        event->ignore();
        return;
    }
    QList<int> szs = sizes();
    QSplitter::resizeEvent(event);
    if (event->size().width() > event->oldSize().width())
        increaseSize(event->size().width() - event->oldSize().width(), szs);
    else if (event->size().width() < event->oldSize().width())
        decreaseSize(event->oldSize().width() - event->size().width(), szs);
}

void Row::increaseSize(int diff, QList<int> & szs)
{
    int resizableIndex = autoResizable_.indexOf(true);
    if (resizableIndex) {
        QList<int> szs = sizes();
        szs[resizableIndex] = szs[resizableIndex] + diff;
        setSizes(szs);
    }
}

void Row::decreaseSize(int diff, QList<int> & szs)
{

}

} // namespace CoreGUI
