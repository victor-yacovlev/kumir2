#include "iconprovider.h"

#include <QApplication>
#include <QVariant>
#include <QFile>
#include <QPalette>
#include <QColor>
#include <QPainter>

namespace Widgets {


QIcon IconProvider::iconForName(const QString &name) const
{
    if (!cache_.contains(name)) {
        loadToCache(name);
    }
    return cache_[name];
}

void IconProvider::loadToCache(const QString &name) const
{
    QIcon icon;

    static const QList<QSize> Sizes = QList<QSize>()
            << QSize(16, 16) << QSize(22, 22) << QSize(32, 32);

    static const QString BaseDir =
            QApplication::instance()->property("sharePath").toString()
            + "/icons/iconset/";

    foreach (const QSize &size, Sizes) {
        const QString fileName = QString::fromLatin1("%1/%2x%3/%4.png")
                .arg(BaseDir)
                .arg(size.width())
                .arg(size.height())
                .arg(name);
        if (QFile::exists(fileName)) {
            QImage iconImage = centerizeToSize(loadAndPreprocess(fileName), size);
            icon.addPixmap(QPixmap::fromImage(iconImage));
        }
    }
    cache_[name] = icon;
}

QImage IconProvider::loadAndPreprocess(const QString &fileName)
{
    QImage source(fileName);
    QImage result;

    static const QPalette palette = QApplication::palette();
    static const QRgb foreground =
            palette.brush(QPalette::Text).color().rgba();
    static const QRgb background =
            palette.brush(QPalette::Base).color().rgba();
    static int normRed = qRed(foreground)-qRed(background);
    static int normBlue = qBlue(foreground)-qBlue(background);
    static int normGreen = qGreen(foreground)-qGreen(background);
    static int bgRed = qRed(background);
    static int bgBlue = qBlue(background);
    static int bgGreen = qGreen(background);
    result = QImage(source.size(), QImage::Format_ARGB32);
    result.fill(0);
    for (int y=0; y<source.height(); y++) {
        for (int x=0; x<source.width(); x++) {
            QRgb sourcePixel = source.pixel(QPoint(x, y));
            int sourceAlpha = qAlpha(sourcePixel);
            qreal sourceIntensivity = 1.0 -
                    (qRed(sourcePixel) + qGreen(sourcePixel) + qBlue(sourcePixel)) /
                    (3.0 * 255.0) ;
            int resultRed = bgRed + normRed * sourceIntensivity;
            int resultBlue = bgBlue + normBlue * sourceIntensivity;
            int resultGreen = bgGreen + normGreen * sourceIntensivity;
            QRgb resultPixel = qRgba(resultRed, resultGreen, resultBlue, sourceAlpha);
            result.setPixel(QPoint(x, y), resultPixel);
        }
    }

    return result;
}

QImage IconProvider::centerizeToSize(const QImage &source, const QSize & size)
{
    if (source.size()==size) {
        return source;
    }
    QImage result = QImage(size, QImage::Format_ARGB32);
    result.fill(0);
    QPainter painter(&result);
    int x = (size.width() - source.width()) / 2;
    if (source.size().width() % 2 !=0 && source.width() < size.width() && source.size().width() + x < size.width()-1) {
        x += 1;
    }
    const int y = (size.height() - source.height()) / 2;
    painter.drawImage(x, y, source);
    painter.end();
    return result;
}

IconProvider* IconProvider::self()
{
    static IconProvider * me = new IconProvider();
    return me;
}

IconProvider::IconProvider()
{
}

} // namespace Widgets
