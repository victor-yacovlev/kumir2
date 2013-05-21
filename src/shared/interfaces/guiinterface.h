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
        enum Language { Kumir, Pascal, etc = 255 };

        Language language;
        QDateTime saved;
        QDateTime changed;
        KumFile::Data content;
        QString title;
    };
public slots:
    virtual void setProgramSource(const ProgramSourceText &source) = 0;
    virtual ProgramSourceText programSource() const = 0;

    virtual void startTesting() = 0;

};

}


Q_DECLARE_METATYPE(Shared::GuiInterface::ProgramSourceText)
Q_DECLARE_INTERFACE(Shared::GuiInterface, "kumir2.Gui")


#endif // GUIINTERFACE_H
