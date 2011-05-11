#include "clipboard.h"

#include <QtGui>

namespace Editor {

Clipboard::Clipboard(QObject *parent)
    : QObject(parent)
    , i_selection(-1)
{
    connect(qApp->clipboard(), SIGNAL(changed(QClipboard::Mode)),
            this, SLOT(checkForChanged()));
}

int Clipboard::entriesCount() const
{
    int result = m_data.size();
    QClipboard * cl = QApplication::clipboard();
    if (cl->mimeData()->hasText()) {
        if (!m_data.contains(cl->text()))
            result ++;
    }
    return result;
}

void Clipboard::checkForChanged()
{
    emit bufferEntriesCountChanged(entriesCount());
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
    checkForChanged();
}

} // namespace Editor
