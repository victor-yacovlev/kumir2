#ifndef PYTHON3LANGUAGE_SANDBOXWIDGET_H
#define PYTHON3LANGUAGE_SANDBOXWIDGET_H

#include <QtGlobal>
#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QImage>

#include "extensionsystem/settings.h"

namespace Python3Language {

class SandboxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SandboxWidget(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
    void reset();
    void addInputItem(const QString & promptText);
    void addTextOutputItem(const QString & text);
    void addGraphicsOutputItem(const QImage & image);
    void addHtmlItem(const QString & html);

private:
    ExtensionSystem::SettingsPtr editorSettings();
    quint32 commandNumber_;
    QTextEdit *contents_;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_SANDBOXWIDGET_H
