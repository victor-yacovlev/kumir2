#ifndef DOCBOOKMODEL_H
#define DOCBOOKMODEL_H

// self includes
#include "modeltype.h"

// Qt includes
#include <QtGlobal>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QSharedPointer>
#include <QSvgRenderer>
#include <QImage>

#ifdef DOCBOOKVIEWER_LIBRARY
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

    quint8 sectionLevel() const;
    ModelType modelType() const;
    const QString & id() const;
    const QString & title() const;
    QString titleAbbrev() const;
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

    ModelPtr findChildrenOfType(const ModelType modelType) const;


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
    QString titleAbbrev_;
    QString subtitle_;
    QString text_;
    QString id_;
    QString os_;
    QString configuration_;
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
                DocBookViewer::ModelType type)
{
    return model && model->modelType() == type;
}

inline bool operator!=(const DocBookViewer::ModelPtr & model,
                DocBookViewer::ModelType type)
{
    return ! (model && model->modelType() == type);
}

#endif
