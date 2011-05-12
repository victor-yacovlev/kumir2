#ifndef EDITORSTANDALONEWINDOW_H
#define EDITORSTANDALONEWINDOW_H

#include <QtGui>

namespace Editor {

class EditorStandaloneWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit EditorStandaloneWindow(QWidget *parent,
                                    const QString &fileName
                                    );

signals:

public slots:
    void newDocument();
    void saveDocument();
    void saveDocumentAs();
    void openDocument();

    void saveToFile(const QString &fileName);
    void loadFromFile(const QString &fileName);

private:
    int i_analizerId;
    class Editor * m_editor;
    QString s_fileName;

};

} // namespace Editor

#endif // EDITORSTANDALONEWINDOW_H
