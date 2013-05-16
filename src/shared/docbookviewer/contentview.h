#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include "contentrenderer.h"

#include <QtGlobal>
#include <QTextBrowser>
#include <QSharedPointer>

namespace DocBookViewer {

typedef QSharedPointer<class DocBookModel> ModelPtr;

class ContentView
        : public QTextBrowser
        , public ContentRenderer
{
    Q_OBJECT
public:
    explicit ContentView(QWidget * parent);

    void reset();
    void addData(ModelPtr data);

signals:
    void requestModelLoad(quintptr dataPtr);

private:
    QString wrapHTML(const QString & body) const;
    QString render(ModelPtr data) const;
    QString renderTOC(ModelPtr data) const;
    QString renderTOCElement(ModelPtr data) const;
    QString renderPlainPage(ModelPtr data) const;
    QString renderOrderedList(ModelPtr data) const;
    QString renderItemizedList(ModelPtr data) const;
    QString renderListItem(ModelPtr data) const;
    QString renderParagraph(ModelPtr data) const;
    QString renderEmphasis(ModelPtr data) const;
    QString renderText(ModelPtr data) const;
    QString normalizeText(QString text) const;
    QString renderElement(ModelPtr data) const;
    QString renderExample(ModelPtr data) const;
    QString renderChilds(ModelPtr data) const;
    QString renderSection(ModelPtr data) const;
    QString renderXref(ModelPtr data) const;
    QString renderProgramListing(ModelPtr data) const;
    QString renderKeyCombo(ModelPtr data) const;
    QString renderKeySym(ModelPtr data) const;
    bool isPlainPage(ModelPtr data) const;

    ModelPtr topLevelModel(ModelPtr data) const;
    ModelPtr findModelById(ModelPtr top, const QString & modelId) const;

    bool hasModelOnThisPage(ModelPtr data) const;
    bool hasChild(ModelPtr who, ModelPtr child) const;

private /*fields*/:
    quint8 currentSectionLevel_;
    QList<ModelPtr> loadedModels_;
    mutable struct Counters {
        quint32 example;
        quint32 figure;
        quint32 table;
    } counters_;

private slots:
    void handleInternalLink(const QUrl & url);


};


}

#endif
