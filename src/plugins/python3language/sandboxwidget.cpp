#include "sandboxwidget.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QTextBrowser>
#include <QTextCharFormat>

#include "extensionsystem/pluginmanager.h"
#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"

namespace Python3Language {

SandboxWidget::SandboxWidget(QWidget *parent)
    : QWidget(parent)
    , commandNumber_(0)
    , contents_(new QTextEdit(this))
{
    QVBoxLayout * mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(contents_);

    if (editorSettings()) {
        const QString fontName = editorSettings()->value("Font/Family", "monospace").toString();
        const int fontSize = editorSettings()->value("Font/Size", 12).toInt();
        QFont font = contents_->font();
        font.setFamily(fontName);
        font.setPointSize(fontSize);
        contents_->setFont(font);
    }

    reset();
}

void SandboxWidget::reset()
{
    commandNumber_ = 0;
    addInputItem("Python [%1]> ");
}

void SandboxWidget::addInputItem(const QString &promptText)
{
    contents_->textCursor().movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    QTextFrameFormat inputFormat;
    inputFormat.setBorder(1);
    inputFormat.setBorderBrush(QColor(Qt::blue));
    contents_->textCursor().insertFrame(inputFormat);
    const QString text = promptText.arg(++commandNumber_);
    contents_->textCursor().insertText(text);
}

void SandboxWidget::addTextOutputItem(const QString &text)
{

}

void SandboxWidget::addGraphicsOutputItem(const QImage &image)
{

}

void SandboxWidget::addHtmlItem(const QString &html)
{

}

ExtensionSystem::SettingsPtr SandboxWidget::editorSettings()
{
    using namespace ExtensionSystem;
    using namespace Shared;

    KPlugin* editorPlugin = PluginManager::instance()->findKPlugin<EditorInterface>();
    if (editorPlugin) {
        return editorPlugin->mySettings();
    }
    else {
        return SettingsPtr();
    }
}

} // namespace Python3Language
