#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include "contentrenderer.h"

#include <QtGlobal>
#include <QTextBrowser>
#include <QSharedPointer>
#include <QResizeEvent>
#include <QWheelEvent>

namespace DocBookViewer {

typedef QSharedPointer<class DocBookModel> ModelPtr;

class ContentView
        : public QTextBrowser
        , public ContentRenderer
{
    Q_OBJECT
public:
    explicit ContentView(QWidget * parent);
    bool isEmpty() const;
    void reset();
    void renderData(ModelPtr data);
    QSize minimumSizeHint() const;

signals:
    void itemRequest(ModelPtr model);

private:
    void resizeEvent(QResizeEvent *e);
    void wheelEvent(QWheelEvent *e);

    QString wrapHTML(const QString & body) const;
    QString renderModel(ModelPtr data) const;
    QString renderElement(ModelPtr data) const;
    QString renderSet(ModelPtr data) const;
    QString renderChapter(ModelPtr data) const;
    QString renderArticle(ModelPtr data) const;
    QString renderAbstract(ModelPtr data) const;
    QString renderTOC(ModelPtr data) const;
    QString renderTOCElement(ModelPtr data, quint8 level, bool enumerate) const;
    QString renderPlainPage(ModelPtr data) const;
    QString renderOrderedList(ModelPtr data) const;
    QString renderItemizedList(ModelPtr data) const;
    QString renderListItem(ModelPtr data) const;
    QString renderParagraph(ModelPtr data) const;
    QString renderEmphasis(ModelPtr data) const;
    QString renderText(ModelPtr data) const;
    QString normalizeText(QString text) const;
    QString renderExample(ModelPtr data) const;
    QString renderChilds(ModelPtr data) const;
    QString renderSection(ModelPtr data) const;
    QString renderXref(ModelPtr data) const;
    QString renderProgramListing(ModelPtr data) const;
    QString renderCode(ModelPtr data) const;
    QString renderKeyCombo(ModelPtr data) const;
    QString renderKeySym(ModelPtr data) const;
    QString renderGuiMenu(ModelPtr data) const;
    QString renderGuiMenuItem(ModelPtr data) const;
    QString renderGuiButton(ModelPtr data) const;
    QString renderSubscript(ModelPtr data) const;
    QString renderSuperscript(ModelPtr data) const;
    static QString& wrapInlineElement(ModelPtr data, QString & result,
                                      bool left, bool right);

    QString renderInformalTable(ModelPtr data) const;
    QString renderTable(ModelPtr data) const;
    QString renderTableContent(ModelPtr data) const;
    QString renderTHead(ModelPtr data) const;
    QString renderTBody(ModelPtr data) const;
    QString renderRow(ModelPtr data) const;
    QString renderEntry(ModelPtr data) const;

    QString renderCaption(ModelPtr data, const QString &captionPrefix) const;
    QString renderMediaObject(ModelPtr data) const;
    QString renderInlineMediaObject(ModelPtr data) const;
    QString renderImageObject(ModelPtr data) const;

    QString renderFuncSynopsys(ModelPtr data) const;
    QString renderFuncSynopsysInfo(ModelPtr data) const;
    QString renderFuncPrototype(ModelPtr data) const;
    QString renderFuncDef(ModelPtr data) const;
    QString renderFunction(ModelPtr data) const;
    QString renderParamDef(ModelPtr data) const;
    QString renderParameter(ModelPtr data) const;
    QString renderType(ModelPtr data) const;

    QString renderListOfExamples(ModelPtr data) const;
    QString renderListOfFunctions(ModelPtr data) const;
    QString renderListOfTables(ModelPtr data) const;

    QString renderMathML(ModelPtr data) const;

    bool isPlainPage(ModelPtr data) const;

    ModelPtr topLevelModel(ModelPtr data) const;
    ModelPtr onePageParentModel(ModelPtr data) const;
    ModelPtr findModelById(ModelPtr top, const QString & modelId) const;
    ModelPtr findImageData(ModelPtr parent) const;
    QString modelToLink(ModelPtr data) const;
    ModelPtr findModelByRawPtr(quintptr raw) const;
    ModelPtr findModelByRawPtr(ModelPtr root, quintptr raw) const;
    QString renderItemContextLink(ModelPtr data) const;

    static quint16 chapterNumber(ModelPtr data);
    static quint16 elementNumber(ModelPtr data);
    static quint16 indexInParent(ModelPtr data);
    static quint16 countOfElements(ModelPtr root, ModelPtr until, bool &stop);
    static QString sectionNumber(ModelPtr data);




    bool hasModelOnThisPage(ModelPtr data) const;
    bool hasChild(ModelPtr who, ModelPtr child) const;

    static QString formatProgramSourceText(
            const QString &source,
            const QStringList &keywords,
            const QString & inlineCommentSymbol,
            const QString & multilineCommentStartSymbol,
            const QString & multilineCommentEndSymbol
            );

    static QString programTextForLanguage(const QString &source,
                                          const QString &language);

    QVariant loadResource(int type, const QUrl &name);

private /*fields*/:
    ModelPtr loadedModel_;
    QUrl lastAnchorUrl_;
    bool ignoreClearAnchorUrl_;

    static bool ExtraFontsLoaded_;

private slots:
    void clearLastAnchorUrl();
    void handleInternalLink(const QUrl & url);


};


}

#endif
