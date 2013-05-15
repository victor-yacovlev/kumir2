#include "docbookmodel.h"
#include "contentrenderer.h"

namespace DocBookViewer {

DocBookModel::DocBookModel(DocBookModel * parent, const ModelType modelType)
    : parent_(parent)
    , modelType_(modelType)
    , sectionLevel_(0)
{
    updateSectionLevel();
}

DocBookModel::~DocBookModel()
{
    foreach (DocBookModel *child, children_) {
        delete child;
    }
}

void DocBookModel::render(ContentRenderer *renderer)
{
    renderer->addData(this);
}

quint8 DocBookModel::sectionLevel() const
{
    return sectionLevel_;
}

DocBookModel::ModelType DocBookModel::modelType() const
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

DocBookModel* DocBookModel::parent() const
{
    return parent_;
}

void DocBookModel::setParent(DocBookModel *parent)
{
    if (parent_) {
        parent_->children_.removeAll(this);
    }
    parent_ = parent;
    if (parent_) {
        if (!parent_->children_.contains(this)) {
            parent_->children_.append(this);
        }
    }
    updateSectionLevel();
}

const QList<DocBookModel*>& DocBookModel::children() const
{
    return children_;
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
        DocBookModel* p = parent_;
        sectionLevel_ = 1u;
        while (p && p->modelType_ == Section) {
            p = p->parent();
            sectionLevel_ ++;
        }
    }
    foreach (DocBookModel * child, children_) {
        child->updateSectionLevel();
    }
}

}
