#include "clipboard.h"

#include <QtGui>

namespace Editor {

Clipboard::Clipboard(QObject *parent)
    : QObject(parent)
    , i_selection(-1)
{
}

void Clipboard::push(const QString &text)
{
    QClipboard * cl = QApplication::clipboard();
    cl->setText(text);
    m_data.prepend(text);
}

void Clipboard::select(int index)
{
    i_selection = index;
}

bool Clipboard::hasContent() const
{
    if (i_selection==-1) {
        QClipboard * cl = QApplication::clipboard();
        return cl->mimeData()->hasText();
    }
    else {
        return i_selection < m_data.size() && !m_data[i_selection].isEmpty();
    }
}

QString Clipboard::content() const
{
    if (i_selection==-1 || i_selection>=m_data.size()) {
        QClipboard * cl = QApplication::clipboard();
        if (cl->mimeData()->hasText()) {
            return cl->text();
        }
        else {
            return "";
        }
    }
    else {
        return m_data[i_selection];
    }
}

void Clipboard::clear()
{
    m_data.clear();
}

} // namespace Editor
