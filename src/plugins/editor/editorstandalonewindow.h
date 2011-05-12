#ifndef EDITORSTANDALONEWINDOW_H
#define EDITORSTANDALONEWINDOW_H

#include <QtGui>
#include "interfaces/visualcomponent.h"


namespace Editor {

class EditorStandaloneWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit EditorStandaloneWindow(class EditorPlugin *plugin,
                                    const QString &fileName
                                    );

signals:

public slots:
    void newProgram();
    void saveDocument();
    void saveDocumentAs();
    void openDocument();

    QString saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);

private:
    void setupEditor();
    int i_analizerId;
    class Shared::VisualComponent * m_editor;
    class EditorPlugin * m_plugin;
    QString s_fileName;

};

} // namespace Editor

#endif // EDITORSTANDALONEWINDOW_H
