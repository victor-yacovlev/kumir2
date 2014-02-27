#include "utf8textbrowser.h"

#include <QFile>

namespace Widgets {

QVariant Utf8TextBrowser::loadResource(int type, const QUrl &name)
{
    const QString fileName = name.toLocalFile();
    QFile f(fileName);
    QIODevice::OpenMode flags = QIODevice::ReadOnly;
    if (QTextDocument::ImageResource != type)
        flags = flags | QIODevice::Text;
    if (f.open(flags)) {
        QByteArray data = f.readAll();
        f.close();
        if (QTextDocument::ImageResource != type) {
            return QString::fromUtf8(data);
        }
        else {
            return data;
        }
    }
    else {
        return QVariant();
    }
}

} // namespace Widgets
