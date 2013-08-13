#ifndef COREGUI_ROW_H
#define COREGUI_ROW_H

#include "extensionsystem/settings.h"

#include <QtGlobal>
#include <QSplitter>


namespace CoreGUI {

class Row : public QSplitter
{
    Q_OBJECT
public:
    explicit Row(QWidget *parent,
                 const QString & settingsKey
                 );
    void addComponent(QWidget * widget, bool autoResizable);
    void updateSettings(ExtensionSystem::SettingsPtr settings);
    void save();
    void restore();
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
signals:
    void visiblityRequest();

private slots:
    void handleVisiblityRequest(bool visible, const QSize & size);

private /*methods*/:
    void resizeEvent(QResizeEvent * event);

    void ensureEnoughtSpaceForComponent(QWidget * component, const QSize & size);
    void releaseSpaceUsesByComponent(QWidget * component);
    void increaseSize(int diff, QList<int> & szs);
    void decreaseSize(int diff, QList<int> & szs);

private /*fields*/:
    ExtensionSystem::SettingsPtr settings_;
    QString settingsKey_;
    QList<bool> autoResizable_;
    
};

} // namespace CoreGUI

#endif // COREGUI_ROW_H
