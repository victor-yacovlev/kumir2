#include "clipboard.h"

#include <QtGui>

namespace Editor {

QString Clipboard::BlockMimeType = "application/vnd.kumir.textblock";

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
    bool duplicate = false;
    if (cl->mimeData()->hasText() )
    {
        foreach (const ClipboardData & entry, m_data) {
            if (entry.text==cl->text()) {
                duplicate = true;
                break;
            }
        }
    }
    if (!duplicate)
        result ++;
    return result;
}

void Clipboard::checkForChanged()
{
    emit bufferEntriesCountChanged(entriesCount());
}

void Clipboard::push(const ClipboardData & data)
{
    QClipboard * cl = QApplication::clipboard();
    QMimeData * md = new QMimeData;
    md->setText(data.text);
    if (data.type==ClipboardData::Block)
        md->setData(BlockMimeType, data.block.join("\n").toUtf8());
    cl->setMimeData(md);
    m_data.prepend(data);
}

void Clipboard::select(int index)
{
    i_selection = index;
}

bool Clipboard::hasContent() const
{
    if (i_selection==-1) {
        QClipboard * cl = QApplication::clipboard();
        return cl->mimeData()->hasText() || cl->mimeData()->hasFormat(BlockMimeType);
    }
    else {
        return i_selection < m_data.size();
    }
}

ClipboardData Clipboard::content() const
{
    if (i_selection==-1 || i_selection>=m_data.size()) {
        QClipboard * cl = QApplication::clipboard();
        ClipboardData result;
        if (cl->mimeData()->hasText()) {
            result.type = ClipboardData::Text;
            result.text = cl->mimeData()->text();
        }
        if (cl->mimeData()->hasFormat(BlockMimeType)) {
            result.type = ClipboardData::Block;
            const QByteArray & raw = cl->mimeData()->data(BlockMimeType);
            result.block = QString::fromUtf8(raw).split("\n");
        }
        else {
            result.type = ClipboardData::Invalid;
        }
        return result;
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
