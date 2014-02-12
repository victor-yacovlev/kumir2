#ifndef DOCBOOKMODEL_H
#define DOCBOOKMODEL_H

// Qt includes
#include <QtGlobal>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QSharedPointer>
#include <QSvgRenderer>
#include <QImage>

#ifdef LIBRARY_BUILD
#define EXTERN Q_DECL_EXPORT
#else
#define EXTERN Q_DECL_IMPORT
#endif

namespace DocBookViewer {

static const quint8 MAX_SECTION_LEVEL_IN_TOC = 2u;
typedef QSharedPointer<class DocBookModel> ModelPtr;
typedef QList<ModelPtr>::iterator ModelIterator;
typedef QSharedPointer<QSvgRenderer> SvgRendererPtr;

class EXTERN DocBookModel
{
    friend class DocBookFactory;
    friend class MathMLRenderer;
public:
    enum ModelType {
        Unknown,

        Text,
        Book,
        Article,
        Set,
        Chapter,
        Section,
        Preface,
        Reference,
        Abstract,
        Para,
        Example,
        ProgramListing,
        Code,
        OrderedList,
        ItemizedList,
        ListItem,
        Xref,
        Emphasis,
        XInfo,
        KeywordSet,
        Keyword,
        Synopsis,
        KeyCombo,
        KeySym,
        Table,
        InformalTable,
        THead,
        TBody,
        Row,
        Entry,
        MediaObject,
        Caption,
        InlineMediaObject,
        ImageObject,
        ImageData,
        Subscript,
        Superscript,

        FuncSynopsys,
        FuncSynopsysInfo,
        FuncPrototype,
        FuncDef,
        ParamDef,
        Function,
        Parameter,
        Package,
        Type,

        MathML_Math,
        MathML_MRow,
        MathML_MSqrt,
        MathML_MFrac,
        MathML_MI,
        MathML_MN,
        MathML_MO,
        MathML_MText,
        MathML_MSup,

        GuiMenu,
        GuiMenuItem,
        GuiButton,

        // virtual entries
        ListOfExamples,
        ListOfTables,
        ListOfFunctions
    };

    quint8 sectionLevel() const;
    ModelType modelType() const;
    const QString & id() const;
    const QString & title() const;
    const QString & subtitle() const;
    const QString & text() const;
    const QString & role() const;
    const QString & xrefLinkEnd() const;
    const QString & xrefEndTerm() const;
    const QUrl & href() const;
    const QString format() const;
    ModelPtr parent() const;
    ModelPtr indexParent() const;
    const QList<ModelPtr>& children() const;
    bool isSectioningNode() const;

    const QImage& imageData() const;

    ModelPtr self() const;


protected /*methods*/:
    explicit DocBookModel(ModelPtr parent, const ModelType modelType);
    void updateSectionLevel();

protected /*fields*/:
    ModelPtr parent_;
    ModelPtr indexParent_;
    QList<ModelPtr> children_;
    ModelType modelType_;
    unsigned short sectionLevel_;
    QString title_;
    QString subtitle_;
    QString text_;
    QString id_;
    QString os_;
    QString xrefLinkEnd_;
    QString xrefEndTerm_;
    QString role_;
    QUrl href_;
    QString format_;
    mutable SvgRendererPtr svgRenderer_;
    mutable QImage cachedImage_;
};

}

inline bool operator==(const DocBookViewer::ModelPtr & model,
                DocBookViewer::DocBookModel::ModelType type)
{
    return model && model->modelType() == type;
}

inline bool operator!=(const DocBookViewer::ModelPtr & model,
                DocBookViewer::DocBookModel::ModelType type)
{
    return ! (model && model->modelType() == type);
}

#endif
