#ifndef WIDGETS_ICONPROVIDER_H
#define WIDGETS_ICONPROVIDER_H

#include <QtGlobal>
#include <QString>
#include <QImage>
#include <QIcon>
#include <QMap>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {

class WIDGETS_EXPORT IconProvider
{
public:
    static IconProvider* self();
    QIcon iconForName(const QString & name) const;
    static QIcon iconFromPath(const QString &path, qreal intensivity = 1.0, const QSize &targetSize = QSize());

private:

    void loadToCache(const QString & name) const;
    static QImage loadAndPreprocess(const QString & fileName, qreal intensivity = 1.0);
    static QImage centerizeToSize(const QImage & source, const QSize & size);

    IconProvider();    
    mutable QMap<QString,QIcon> cache_;
};

} // namespace Widgets

#endif // WIDGETS_ICONPROVIDER_H
