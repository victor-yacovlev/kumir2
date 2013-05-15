#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include "contentrenderer.h"

#include <QtGlobal>
#include <QTextBrowser>

namespace DocBookViewer {

class ContentView
        : public QTextBrowser
        , public ContentRenderer
{
    Q_OBJECT
public:
    explicit ContentView(QWidget * parent);

    void reset();
    void addData(const class DocBookModel *data);

signals:
    void requestModelLoad(quintptr dataPtr);

private:
    QString wrapHTML(const QString & body) const;
    QString render(const class DocBookModel *data) const;
    QString renderTOC(const class DocBookModel * data) const;
    QString renderTOCElement(const class DocBookModel * data) const;
    QString renderPlainPage(const class DocBookModel * data) const;
    QString renderOrderedList(const class DocBookModel * data) const;
    QString renderItemizedList(const class DocBookModel * data) const;
    QString renderListItem(const class DocBookModel * data) const;
    QString renderParagraph(const class DocBookModel * data) const;
    QString renderEmphasis(const class DocBookModel * data) const;
    QString renderText(const class DocBookModel * data) const;
    QString normalizeText(QString text) const;
    QString renderElement(const class DocBookModel * data) const;
    QString renderExample(const class DocBookModel * data) const;
    QString renderChilds(const class DocBookModel * data) const;
    QString renderSection(const class DocBookModel * data) const;
    QString renderXref(const class DocBookModel * data) const;
    QString renderProgramListing(const class DocBookModel * data) const;
    QString renderKeyCombo(const class DocBookModel * data) const;
    QString renderKeySym(const class DocBookModel * data) const;
    bool isPlainPage(const class DocBookModel * data) const;

    const class DocBookModel * topLevelModel(
            const class DocBookModel * data) const;
    const class DocBookModel * findModelById(
            const class DocBookModel * top,
            const QString & modelId
            ) const;

    bool hasModelOnThisPage(const class DocBookModel* data) const;
    bool hasChild(const class DocBookModel* who,
                  const class DocBookModel* child) const;

private /*fields*/:
    quint8 currentSectionLevel_;
    QList<const class DocBookModel*> loadedModels_;
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
