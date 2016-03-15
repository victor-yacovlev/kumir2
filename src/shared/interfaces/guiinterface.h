#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H

#include <dataformats/kumfile.h>

#include <QtCore>
#include <QWidget>

namespace Shared
{

class GuiInterface
{
    Q_ENUMS(Shared::GuiInterface::ProgramSourceText::Language)
public:
    struct ProgramSourceText {
        enum Language { Kumir, Pascal, Python, etc = 255 };

        Language language;
        QDateTime saved;
        QDateTime changed;
        Shared::Analizer::SourceFileInterface::Data content;
        QString title;
        QUrl url;
    };
public slots:
    virtual QObject * pluginObject() = 0;
    virtual QObject * mainWindowObject() = 0;
    virtual void setProgramSource(const ProgramSourceText &source) = 0;
    virtual ProgramSourceText programSource() const = 0;

    virtual void startTesting() = 0;

};

}


Q_DECLARE_METATYPE(Shared::GuiInterface::ProgramSourceText)
Q_DECLARE_INTERFACE(Shared::GuiInterface, "kumir2.Gui")


#endif // GUIINTERFACE_H
