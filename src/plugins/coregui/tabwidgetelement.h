#ifndef TABWIDGETELEMENT_H
#define TABWIDGETELEMENT_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "extensionsystem/settings.h"
#include "mainwindow.h"
#include "interfaces/startpage_widget_interface.h"

namespace CoreGUI {
class TabWidgetElement
        : public QWidget
{
    Q_OBJECT
public:
    explicit TabWidgetElement(QWidget * w
                              , ExtensionSystem::SettingsPtr settings
                              , bool enableToolBar
                              , QList<QAction*> toolbarActions
                              , QList<QMenu*> ms
                              , MainWindow::DocumentType t
                              , QActionGroup * gr_fileActions
                              , QActionGroup * gr_otherActions
                              , class KumirProgram * kumir
                              );

    QWidget * component;
    QList<QMenu*> menus;
    MainWindow::DocumentType type;

    inline Shared::Editor::InstanceInterface* editor() const { return editorInstance_; }
    void setEditor(Shared::Editor::InstanceInterface* editor);

    inline Shared::StartpageWidgetInterface* startPage() const { return startPageInstance_; }
    void setStartPage(Shared::StartpageWidgetInterface* sp);

    inline class KumirProgram * kumirProgram() { return kumirProgram_; }
    inline bool isCourseManagerTab() const { return courseManagerTab_; }
    inline void setCourseManagerTab(bool v) {
        courseManagerTab_ = v;
    }
    QString title() const;
    inline void setCourseTitle(const QString & title) { courseTitle_ = title; }
    void updateSettingsObject(ExtensionSystem::SettingsPtr settings);
    inline QSet<QString> availableMenuNames() const { return availableMenuNames_; }

signals:
    void titleChanged(const QString &);
    void documentCleanChanged(bool v);
    void explicitImportNamesRequest();
protected:
    inline void focusInEvent(QFocusEvent *e) {
        QWidget::focusInEvent(e);
        component->setFocus();
    }
    bool eventFilter(QObject *obj, QEvent *evt); // for toolbar context menu event
protected slots:
    void setDocumentChangesClean(bool clean);
    void updateCompilerImportsList(const QStringList & localizedNames);

private:
    Shared::Editor::InstanceInterface * editorInstance_;
    Shared::StartpageWidgetInterface * startPageInstance_;
    class KumirProgram * kumirProgram_;
    bool courseManagerTab_;
    QString courseTitle_;
    bool documentHasChanges_;
    QAction * actionSave_;
    class ToolbarContextMenu * toolbarContextMenu_;
    QSet<QString> availableMenuNames_;
};
}

#endif // TABWIDGETELEMENT_H
