#ifndef EDITORSTANDALONEWINDOW_H
#define EDITORSTANDALONEWINDOW_H

#include <QtGui>
#include "settingspage.h"
#include "interfaces/editorinterface.h"


namespace Editor {

class EditorStandaloneWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit EditorStandaloneWindow(class EditorPlugin *plugin,
                                    QSettings * settings,
                                    const QString &fileName
                                    );

signals:

public slots:
    void newProgram();
    void saveDocument();
    void saveDocumentAs();
    void openDocument();
    void showSettings();

    QString saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);

private slots:
    void loadSettings();
    void saveSettings();
    void loadFileFromDrop(const QList<QUrl> &urls);

protected:
    void closeEvent(QCloseEvent * e);

private:

    void setupEditor();
    struct Shared::EditorComponent m_editor;
    class EditorPlugin * m_plugin;
    QSettings * m_settings;
    QString s_fileName;
    static QString RecentPathSettingsKey;
    static QString WindowRectSettingsKey;

};

} // namespace Editor

#endif // EDITORSTANDALONEWINDOW_H
