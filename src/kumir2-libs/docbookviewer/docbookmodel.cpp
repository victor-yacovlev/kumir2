#include "docbookmodel.h"
#include "contentrenderer.h"

#include <QImage>
#include <QPainter>

namespace DocBookViewer {

DocBookModel::DocBookModel(ModelPtr parent, const ModelType modelType)
    : parent_(parent)
    , modelType_(modelType)
    , sectionLevel_(0)
{
    updateSectionLevel();
}


quint8 DocBookModel::sectionLevel() const
{
    return sectionLevel_;
}

ModelType DocBookModel::modelType() const
{
    return modelType_;
}

const QString& DocBookModel::id() const
{
    return id_;
}

const QString& DocBookModel::title() const
{
    return title_;
}

QString DocBookModel::titleAbbrev() const
{
    if (titleAbbrev_.isEmpty())
        return title_;
    else
        return titleAbbrev_;
}

const QString& DocBookModel::subtitle() const
{
    return subtitle_;
}

const QString& DocBookModel::text() const
{
    return text_;
}

const QString& DocBookModel::role() const
{
    return role_;
}

const QString& DocBookModel::xrefLinkEnd() const
{
    return xrefLinkEnd_;
}

const QString& DocBookModel::xrefEndTerm() const
{
    return xrefEndTerm_;
}

const QString DocBookModel::format() const
{
    return format_.toLower().trimmed();
}

const QUrl& DocBookModel::href() const
{
    return href_;
}

ModelPtr DocBookModel::parent() const
{
    return parent_;
}

ModelPtr DocBookModel::indexParent() const
{
    return indexParent_;
}

ModelPtr DocBookModel::findChildrenOfType(const ModelType modelType) const
{
    ModelPtr result;
    foreach (ModelPtr child, children_) {
        if (modelType == child->modelType()) {
            result = child;
        }
        else {
            result = child->findChildrenOfType(modelType);
        }
        if (result) break;
    }
    return result;
}

const QList<ModelPtr>& DocBookModel::children() const
{
    return children_;
}

ModelPtr DocBookModel::self() const
{
    ModelPtr result;
    if (parent()) {
        foreach (ModelPtr child, parent()->children()) {
            if (child.data() == this) {
                result = child;
                break;
            }
        }
    }
    if (!result) foreach (ModelPtr child, children()) {
        if (child->parent().data() == this) {
            result = child->parent();
            break;
        }
    }
    return result;
}

bool DocBookModel::isSectioningNode() const
{
    if (title_.length() == 0) {
        return false;
    }
    else if (modelType_ == Section) {
        return sectionLevel_ <= MAX_SECTION_LEVEL_IN_TOC;
    }
    else {
        return modelType_ == Book ||
                modelType_ == Article ||
                modelType_ == Set ||
                modelType_ == Chapter ||
                modelType_ == Preface ||
                modelType_ == Reference ||
                modelType_ == Abstract;
    }
}

void DocBookModel::updateSectionLevel()
{
    if (modelType_ != Section) {
        sectionLevel_ = 0u;
    }
    else {
        ModelPtr p = parent_;
        sectionLevel_ = 1u;
        while (p && p->modelType_ == Section) {
            p = p->parent();
            sectionLevel_ ++;
        }
    }
    foreach (ModelPtr child, children_) {
        child->updateSectionLevel();
    }
}

const QImage& DocBookModel::imageData() const
{
    if (svgRenderer_ && cachedImage_.isNull()) {
        const QSize size = svgRenderer_->defaultSize();
        QImage img(size, QImage::Format_ARGB32);
        img.fill(0);
        QPainter painter(&img);
        svgRenderer_->render(&painter);
        cachedImage_ = img;
    }
    return cachedImage_;
}

}
