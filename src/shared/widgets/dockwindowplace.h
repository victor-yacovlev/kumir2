#ifndef WIDGETS_DOCKWINDOWPLACE_H
#define WIDGETS_DOCKWINDOWPLACE_H

#include "extensionsystem/settings.h"

#include <QSharedPointer>
#include <QtGlobal>
#include <QTabWidget>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif


namespace Widgets {

class WIDGETS_EXPORT DockWindowPlace
        : public QTabWidget
{
    friend class DockWindowPlaceImpl;
    friend class SecondaryWindow;
    Q_OBJECT
public:
    explicit DockWindowPlace(QWidget *parent,
                             const QString settingsKey);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updateSettings(ExtensionSystem::SettingsPtr settings);
    void addPersistentWidget(QWidget * widget);
    void saveState();
    void restoreState();
signals:
    void visiblityRequest(bool visible, const QSize &size);
protected:
    void registerWindowHere(class SecondaryWindow * window);
private:
    QSharedPointer<class DockWindowPlaceImpl> pImpl_;
};

} // namespace Widgets

#endif // WIDGETS_DOCKWINDOWPLACE_H
