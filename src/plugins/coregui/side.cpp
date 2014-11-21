#include "side.h"
#include "widgets/dockwindowplace.h"

#include <QDebug>
#include <QResizeEvent>

namespace CoreGUI {

Side::Side(QWidget *parent, const QString &settingsKey)
    : QSplitter(Qt::Horizontal, parent)
    , settingsKey_(settingsKey)
{   
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setVisible(false);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setHandleWidth(10);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setAutoFillBackground(true);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
}

void Side::updateSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
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
    int sum = 0;
    Q_FOREACH(int v, szs) sum += v;
    setVisible(sum > 0);
}

void Side::addComponent(QWidget *widget, bool autoResizable)
{
    addWidget(widget);
    autoResizable_.push_back(autoResizable);
    setCollapsible(count()-1, !autoResizable);
    updateGeometry();
}

void Side::save()
{
    if (!settings_) return;
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

void Side::restore()
{
    if (!settings_) return;
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

QSize Side::sizeHint() const
{
    int w = (count() - 1) * handleWidth();
    int h = 0;
    for (int i=0; i<sizes().size(); i++) {
        w += sizes().at(i);
        h = qMax(h, widget(i)->height());
    }
    return QSize(w, h);
}

QSize Side::minimumSizeHint() const
{
    int w = 0, h = 0;
    if (orientation() == Qt::Horizontal) {
        w = (count() - 1) * handleWidth();
        for (int i=0; i<count(); i++) {
            const QSize sz = widget(i)->minimumSizeHint();
            w += sz.width();
            h = qMax(h, sz.height());
        }
    }
    else {
        h = (count() - 1) * handleWidth();
        for (int i=0; i<count(); i++) {
            const QSize sz = widget(i)->minimumSizeHint();
            h += sz.height();
            w = qMax(w, sz.width());
        }
    }
    return QSize(w, h);
}

void Side::handleVisiblityRequest(bool visible, const QSize & size)
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

void Side::forceResizeItem(const QSize &sz)
{
    QWidget * who = qobject_cast<QWidget*>(sender());
    Q_ASSERT(who);
    const int index = indexOf(who);
    if (-1 == index) {
        return;
    }
    Q_ASSERT(-1 != index);
    QList<int> szs = sizes();
    const int diff = Qt::Horizontal == orientation()
            ? sz.width() - szs.at(index)
            : sz.height() - szs.at(index);
    for (int i=0; i<szs.size(); ++i) {
        if (i == index) {
            szs[i] += diff;
        }
        else {
            szs[i] -= diff;
        }
    }
    setSizes(szs);
}

void Side::ensureEnoughtSpaceForComponent(QWidget *component, const QSize &size)
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
    int widgetsCount = count();
    if (resizableIndex >= widgetsCount)
        return;
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
    int sum = 0;
    Q_FOREACH(int v, szs) sum += v;
    setVisible(sum > 0);
}

void Side::releaseSpaceUsesByComponent(QWidget *component)
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
    int sum = 0;
    Q_FOREACH(int v, szs) sum += v;
    setVisible(sum > 0);
}

void Side::resizeEvent(QResizeEvent *event)
{
    if (event->size().width() < minimumSizeHint().width() || event->size().height() < minimumSizeHint().height()) {
        event->ignore();
        return;
    }
    QList<int> szs = sizes();
    QSplitter::resizeEvent(event);
    if (orientation() == Qt::Horizontal) {
        if (event->size().width() > event->oldSize().width())
            increaseSize(event->size().width() - event->oldSize().width(), szs);
        else if (event->size().width() < event->oldSize().width())
            decreaseSize(event->oldSize().width() - event->size().width(), szs);
    }
    else {
        if (event->size().height() > event->oldSize().height())
            increaseSize(event->size().height() - event->oldSize().height(), szs);
        else if (event->size().height() < event->oldSize().height())
            decreaseSize(event->oldSize().height() - event->size().height(), szs);
    }
}

void Side::increaseSize(int diff, QList<int> & szs)
{
    int resizableIndex = autoResizable_.indexOf(true);
    if (resizableIndex != -1 && resizableIndex < szs.length()) {
        QList<int> szs = sizes();
        szs[resizableIndex] = szs[resizableIndex] + diff;
        setSizes(szs);
    }
}

void Side::decreaseSize(int diff, QList<int> & szs)
{

}

} // namespace CoreGUI
