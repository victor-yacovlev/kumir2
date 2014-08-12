#ifndef EDITOR_INSTANCEINTERFACE_H
#define EDITOR_INSTANCEINTERFACE_H

#include "dataformats/kumfile.h"
#include "interfaces/analizer_instanceinterface.h"
#include "interfaces/analizer_sourcefileinterface.h"
#include "interfaces/analizer_helperinterface.h"

#include <QWidget>
#include <QList>
#include <QMenu>
#include <QAction>

namespace Shared { namespace Editor {

class InstanceInterface {
public:
    virtual bool isModified() const = 0;
    virtual void setNotModified() = 0;

    virtual void loadDocument(QIODevice * device,
                              const QString & fileNameSuffix = "",
                              const QString & sourceEncoding = "",
                              const QUrl & sourceUrl = QUrl()
            ) /* throws QString */ = 0;
    virtual void loadDocument(const QString & fileName) /* throws QString */ = 0;
    virtual void loadDocument(const Analizer::SourceFileInterface::Data &data) /* throws QString */ = 0;

    virtual void saveDocument(const QString & fileName) /* throws QString */ = 0;
    virtual void saveDocument(QIODevice * device) /* throws QString */ = 0;

    virtual Analizer::SourceFileInterface::Data documentContents() const = 0;

    virtual Analizer::InstanceInterface * analizer() = 0;
    virtual quint32 errorLinesCount() const = 0;
    virtual void ensureAnalized() = 0;

    virtual void highlightLineGreen(int lineNo, quint32 colStart, quint32 colEnd) = 0;
    virtual void highlightLineRed(int lineNo, quint32 colStart, quint32 colEnd) = 0;
    virtual void unhighlightLine() = 0;

    virtual void appendMarginText(int lineNo, const QString & text) = 0;
    virtual void setMarginText(int lineNo, const QString & text, const QColor & fgColor) = 0;
    virtual void clearMarginText(int fromLine = 0, int toLine = -1) = 0;

    virtual bool supportsContextHelp() const = 0;
    inline virtual Analizer::ApiHelpItem contextHelpItem() const { return Analizer::ApiHelpItem(); }

    virtual QWidget * widget() = 0;
    virtual QList<QMenu*> menus() const = 0;
    virtual QList<QAction*> toolBarActions() const = 0;
};


}}

Q_DECLARE_INTERFACE(Shared::Editor::InstanceInterface,
                    "kumir2.Editor.InstanceInterface")

#endif // EDITOR_INSTANCEINTERFACE_H
