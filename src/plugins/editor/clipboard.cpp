#include "clipboard.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace Editor {

QString Clipboard::BlockMimeType = "application/vnd.kumir.textblock";

Clipboard * Clipboard::instance()
{
    static Clipboard INSTANCE;
    return &INSTANCE;
}

Clipboard::Clipboard()
    : QObject()
    , selection_(-1)
{
    connect(qApp->clipboard(), SIGNAL(changed(QClipboard::Mode)),
            this, SLOT(checkForChanged()));
}

int Clipboard::entriesCount() const
{
    int result = data_.size();
    QClipboard * cl = QApplication::clipboard();
    bool duplicate = false;
    if (cl->mimeData()->hasText() )
    {
        foreach (const ClipboardData & entry, data_) {
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
#ifdef Q_OS_WIN32
    static const QString RTFMime = "Rich Text Format";
#else
    static const QString RTFMime = "text/rtf";
#endif
    md->setData(RTFMime, data.rtf);
    if (data.type==ClipboardData::Block)
        md->setData(BlockMimeType, data.block.join("\n").toUtf8());
    cl->setMimeData(md);
    data_.prepend(data);
}

void Clipboard::select(int index)
{
    selection_ = index;
}

bool Clipboard::hasContent() const
{
    if (selection_==-1) {
        QClipboard * cl = QApplication::clipboard();
        const QMimeData * data = cl->mimeData();
        bool text = data->hasText();
        bool block = data->hasFormat(BlockMimeType);
        return text || block;
    }
    else {
        return selection_ < data_.size();
    }
}

ClipboardData Clipboard::content() const
{
    if (selection_==-1 || selection_>=data_.size()) {
        QClipboard * cl = QApplication::clipboard();
        ClipboardData result;
        result.type = ClipboardData::Invalid;
        if (cl->mimeData()->hasText()) {
            result.type = ClipboardData::Text;
            result.text = cl->mimeData()->text();
        }
        if (cl->mimeData()->hasFormat(BlockMimeType)) {
            result.type = ClipboardData::Block;
            const QByteArray & raw = cl->mimeData()->data(BlockMimeType);
            result.block = QString::fromUtf8(raw).split("\n");
        }        
        return result;
    }
    else {
        return data_[selection_];
    }
}

void Clipboard::clear()
{
    data_.clear();
    checkForChanged();
}

} // namespace Editor
