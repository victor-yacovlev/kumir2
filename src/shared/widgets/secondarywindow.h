#ifndef WIDGETS_SECONDARYWINDOW_H
#define WIDGETS_SECONDARYWINDOW_H

#include "extensionsystem/settings.h"
#include <QObject>
#include <QWidget>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {

class WIDGETS_EXPORT SecondaryWindow
        : public QObject
{
    Q_OBJECT
public:
    class SecondaryWindowImplementationInterface * dockContainer();
    class SecondaryWindowImplementationInterface * windowContainer();

    const QString & settingsKey() const;

    static SecondaryWindow * createSecondaryWindow(
            QWidget * centralWidget,
            const QString & title,
            const QIcon & icon,
            QWidget * topLevelParent,
            class DockWindowPlace * dockPlace,
            const QString & settingsKey,
            bool resizable
            );

    void changeDockPlace(class DockWindowPlace * dockPlace);



signals:
    
public slots:
    void activate();
    void updateSettings(ExtensionSystem::SettingsPtr settings,
                        const QStringList & keys);
    void saveState();
    void restoreState();

private /*methods*/:
    explicit SecondaryWindow(QWidget * topLevelParent,
                             class SecondaryWindowImplementationInterface * windowContainer,
                             class SecondaryWindowImplementationInterface * dockContainer,
                             const QString & settingsKey,
                             QWidget * centralWidget
                             );

    static SecondaryWindowImplementationInterface * createWindowContainer(
            const QString & title,
            const QIcon & icon,
            QWidget * topLevelParent,
            bool resizable
            );

    static SecondaryWindowImplementationInterface * createDockContainer(
            const QString & title,
            class DockWindowPlace * dockPlace
            );

    class SecondaryWindowImplementationInterface * currentContainer();

private /*fields*/:
    class SecondaryWindowImplementationInterface * dockContainer_;
    class SecondaryWindowImplementationInterface * windowContainer_;
    QString settingsKey_;
    ExtensionSystem::SettingsPtr settings_;
    QWidget * topLevelParent_;
    QWidget * centralWidget_;

};

} // namespace Widgets

#endif // WIDGETS_SECONDARYWINDOW_H
