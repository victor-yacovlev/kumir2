#include "fpcanalizerplugin.h"
#include "fpcanalizer.h"

namespace FpcAnalizer {

QString FpcAnalizerPlugin::initialize(const QStringList &, const CommandLine &)
{
    me_ = this;
    lastInstanceIndex_ = 0;
    return "";
}

void FpcAnalizerPlugin::updateSettings(const QStringList &)
{

}

FpcAnalizerPlugin* FpcAnalizerPlugin::me_ = 0;

FpcAnalizerPlugin* FpcAnalizerPlugin::self()
{
    return me_;
}


QMap<QString,QString> FpcAnalizerPlugin::readCapitalizationHints() const
{
    static QMap<QString,QString> result;
    if (result.isEmpty()) {
        const QString fn = myResourcesDir().absoluteFilePath("capitalization-hints.txt");
        QFile f(fn);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QStringList lines = QString::fromAscii(f.readAll()).split('\n', QString::SkipEmptyParts);
            Q_FOREACH(const QString line, lines) {
                if (line.trimmed().length() > 0 && !line.trimmed().startsWith("#")) {
                    const QString key = line.toLower().trimmed();
                    const QString val = line.trimmed();
                    result[key] = val;
                }
            }

            f.close();
        }
    }
    return result;
}

Analizer::InstanceInterface* FpcAnalizerPlugin::createInstance()
{
    lastInstanceIndex_ ++;
    return new FpcAnalizer(this, lastInstanceIndex_);
}

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(FPCAnalizerPlugin, FpcAnalizer::FpcAnalizerPlugin)
#endif
