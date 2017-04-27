#include "iconprovider.h"

#include <QApplication>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QPalette>
#include <QColor>
#include <QPainter>
#include <QSet>

#ifdef Q_OS_LINUX
extern "C" {
#include <unistd.h>
}
#endif

namespace Widgets {


QIcon IconProvider::iconForName(const QString &name) const
{
    if (!cache_.contains(name)) {
        loadToCache(name);
    }
    return cache_[name];
}

QIcon IconProvider::iconFromPath(const QString &path, qreal intensivity, const QSize &targetSize)
{
    QIcon result;
    QImage image = loadAndPreprocess(path, intensivity);
    if (targetSize.isValid()) {
        result.addPixmap(QPixmap::fromImage(centerizeToSize(image, targetSize)));
    }
    else {
        result.addPixmap(QPixmap::fromImage(image));
    }
    return result;
}

static
QList<QDir> iconsDirs()
{
    QList<QDir> result;

    // Translations dir from base distribution
    const QString sharePath = QApplication::instance()->property("sharePath").toString();
    QDir baseTranslationsDir(sharePath+"/icons/iconset");
    if (baseTranslationsDir.exists()) {
        result.append(baseTranslationsDir);
    }

#ifdef Q_OS_LINUX
    // Search additional paths
    const QString homePath = QString::fromLocal8Bit(::getenv("HOME"));
    const QStringList extraPaths = QStringList()
            << "/usr/share/kumir2/icons/iconset"
            << "/usr/local/share/kumir2//icons/iconset"
            << "/opt/kumir2/share/icons/iconset"
            << "/opt/kumir/share/icons/iconset"
            << homePath + "/.local/share/kumir2/icons/iconset"
               ;

    Q_FOREACH(const QString & path, extraPaths) {
        QDir candidate(path);
        if (candidate.exists()) {
            result.append(candidate);
        }
    }
#endif

    return result;
}

void IconProvider::loadToCache(const QString &name) const
{
    QIcon icon;

    static const QList<QSize> Sizes = QList<QSize>()
            << QSize(16, 16) << QSize(22, 22) << QSize(24, 24) << QSize(32, 32);

    static const QList<QDir> SearchDirs = iconsDirs();
    QSet<int> loadedSizes;

    Q_FOREACH (const QSize &size, Sizes) {
        const QString fileName = QString::fromLatin1("%1x%2/%3.png")
                .arg(size.width())
                .arg(size.height())
                .arg(name);
        Q_FOREACH(const QDir & dir, SearchDirs) {
            const QString absPath = dir.absoluteFilePath(fileName);
            if (QFile::exists(absPath) && !loadedSizes.contains(size.width())) {
                QImage iconImage = centerizeToSize(loadAndPreprocess(absPath), size);
                icon.addPixmap(QPixmap::fromImage(iconImage));
                loadedSizes.insert(size.width());
            }
        }
    }
    cache_[name] = icon;
}

QImage IconProvider::loadAndPreprocess(const QString &fileName, qreal intensivity)
{
    QImage source(fileName);    
    QImage result;

    static const QPalette palette = QApplication::palette();
    static const QRgb foreground =
            palette.brush(QPalette::WindowText).color().rgba();
    static const QRgb background =
            palette.brush(QPalette::Window).color().rgba();
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
            int sourceRed = qRed(sourcePixel);
            int sourceGreen = qGreen(sourcePixel);
            int sourceBlue = qBlue(sourcePixel);
            int sourceAlpha = qAlpha(sourcePixel);
            static const int delta = 1;
            int sourceRedMin = sourceRed - delta;
            int sourceRedMax = sourceRed + delta;
            bool isGray =
                    sourceRedMin <= sourceGreen && sourceGreen <= sourceRedMax
                    &&
                    sourceRedMin <= sourceBlue && sourceGreen <= sourceRedMax
                    ;
            qreal sourceIntensivity = 1.0 -
                    (sourceRed + sourceGreen + sourceBlue) /
                    (3.0 * 255.0) ;
            int resultRed = bgRed + normRed * sourceIntensivity * intensivity;
            int resultBlue = bgBlue + normBlue * sourceIntensivity * intensivity;
            int resultGreen = bgGreen + normGreen * sourceIntensivity * intensivity;
            QRgb resultPixel;
            if (isGray) {
                // Adopt to system theme
                resultPixel = qRgba(resultRed, resultGreen, resultBlue, sourceAlpha);
            }
            else {
                // Keep color unchanged
                resultPixel = sourcePixel;
            }
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
