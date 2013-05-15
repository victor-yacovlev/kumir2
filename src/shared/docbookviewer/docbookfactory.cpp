// Self includes
#include "docbookfactory.h"
#include "docbookmodel.h"
#include "document.h"

// Qt includes
#include <QtCore>
#include <QtXml>

namespace DocBookViewer {


DocBookFactory* DocBookFactory::self()
{
    static DocBookFactory * instance = new DocBookFactory();
    return instance;
}

DocBookFactory::DocBookFactory()
    : reader_(new QXmlSimpleReader)
    , doc_(0)
    , root_(0)
{
    reader_->setContentHandler(this);
    reader_->setErrorHandler(this);
}

Document DocBookFactory::parseDocument(const QUrl &url, QString *error) const
{
    // TODO network url loading
    const QString fileName = url.toLocalFile();
    DocBookModel * content = 0;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        content = parseDocument(&file, url, error);
        file.close();
    }
    return Document(url, content);
}

DocBookModel* DocBookFactory::parseDocument(QIODevice *stream,
                                            const QUrl & url,
                                            QString *error) const
{
    url_ = url;
    QXmlInputSource source(stream);
    if (reader_->parse(source)) {
        if (error)
            error->clear();
        filterByOs(doc_);
        return doc_;
    }
    else {
        if (doc_)
            delete doc_;
        const QString errorText = reader_->errorHandler()->errorString();
        if (error) {
            *error = errorText;
        }
        return 0;
    }
}

void DocBookFactory::filterByOs(DocBookModel *root) const
{
    QString pattern;
#ifdef Q_WS_MAC
    pattern = "mac";
#endif
#ifdef Q_WS_X11
    pattern = "x11";
#endif
#ifdef Q_WS_WIN
    pattern = "win";
#endif
    typedef QList<DocBookModel*>::iterator iterator;
    for (iterator it = root->children_.begin(); it!=root->children_.end(); )
    {
        DocBookModel* child = *it;
        bool toDelete = false;
        if (child->os_.length() > 0) {
            const QString os = child->os_.toLower().trimmed();
            toDelete = os.indexOf(pattern) == -1;
            if (os.startsWith("!") || os.startsWith("not")) {
                toDelete = !toDelete;
            }
        }
        if (toDelete) {
            it = root_->children_.erase(it);
            delete child;
        }
        else {
            filterByOs(child);
            it ++;
        }
    }
}

bool DocBookFactory::startDocument()
{
    if (doc_)
        delete doc_;
    doc_ = root_ = 0;
    return true;
}

bool DocBookFactory::startElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName,
        const QXmlAttributes &atts)
{
    const QString element = localName.toLower();
    DocBookModel * model = 0;
    if (element == "article") {
        model = new DocBookModel(root_, DocBookModel::Article);
    }
    else if (element == "book") {
        model = new DocBookModel(root_, DocBookModel::Book);
    }
    else if (element == "set") {
        model = new DocBookModel(root_, DocBookModel::Set);
    }
    else if (element == "abstract") {
        model = new DocBookModel(root_, DocBookModel::Abstract);
    }
    else if (element == "preface") {
        model = new DocBookModel(root_, DocBookModel::Preface);
    }
    else if (element == "chapter") {
        model = new DocBookModel(root_, DocBookModel::Chapter);
    }
    else if (element == "section") {
        model = new DocBookModel(root_, DocBookModel::Section);
    }
    else if (element == "para") {
        model = new DocBookModel(root_, DocBookModel::Para);
    }
    else if (element == "programlisting") {
        model = new DocBookModel(root_, DocBookModel::ProgramListing);
    }
    else if (element == "example") {
        model = new DocBookModel(root_, DocBookModel::Example);
    }
    else if (element == "orderedlist") {
        model = new DocBookModel(root_, DocBookModel::OrderedList);
    }
    else if (element == "itemizedlist") {
        model = new DocBookModel(root_, DocBookModel::ItemizedList);
    }
    else if (element == "listitem") {
        model = new DocBookModel(root_, DocBookModel::ListItem);
    }
    else if (element == "emphasis") {
        model = new DocBookModel(root_, DocBookModel::Emphasis);
        model->role_ = atts.value("role");
    }
    else if (element == "keycombo") {
        model = new DocBookModel(root_, DocBookModel::KeyCombo);
    }
    else if (element == "keysym") {
        model = new DocBookModel(root_, DocBookModel::KeySym);
    }
    else if (element == "xref") {
        model = new DocBookModel(root_, DocBookModel::Xref);
        model->xrefLinkEnd_ = atts.value("linkend");
        model->xrefEndTerm_ = atts.value("endterm");
    }
    if (model) {
        if (root_ && buffer_.length() > 0) {
            DocBookModel* text = new DocBookModel(root_, DocBookModel::Text);
            text->text_ = buffer_;
            root_->children_.append(text);
            buffer_.clear();
        }
        root_ = model;
        model->id_ = atts.value("id");
        model->os_ = atts.value("os");
    }
    else if (element == "include") {
        const QString href = atts.value("href");
        if (href.length() > 0) {
            const QUrl hrefUrl = url_.resolved(href);
            // TODO network url support
            const QString fileName = hrefUrl.toLocalFile();
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly)) {
                DocBookFactory* innerFactory = new DocBookFactory();
                QString localError;
                DocBookModel * include =
                        innerFactory->parseDocument(&file, hrefUrl, &localError);
                if (include) {
                    if (root_) {
                        include->setParent(root_);
                    }
                    else {
                        root_ = include;
                    }
                }
                file.close();
                delete innerFactory;
            }
        }
    }
    else {
        buffer_.clear();
    }
    return true;
}

bool DocBookFactory::characters(const QString &ch)
{
    bool preformatMode = root_ &&
            root_->modelType_ == DocBookModel::ProgramListing;
    if (preformatMode) {
        buffer_ += ch;
    }
    else {
        if (buffer_.length() > 0 && ch.trimmed().length() > 0) {
            buffer_.push_back(' ');
        }
        buffer_ += ch.simplified();
    }
    return true;
}

bool DocBookFactory::endElement(const QString &namespaceURI,
                                const QString &localName,
                                const QString &qName)
{
    static const QStringList tagsToClose = QStringList()
            << "book" << "article" << "set"
            << "chapter" << "section" << "para"
            << "listitem" << "itemizedlist" << "orderedlist"
            << "example" << "programlisting"
            << "preface" << "abstract" << "reference"
            << "emphasis" << "xref"  << "keycombo" << "keysym";
    const QString element = localName.toLower();
    if (root_ && element == "title") {
        root_->title_ = buffer_;
        buffer_.clear();
    }
    else if (root_ && element == "subtitle") {
        root_->subtitle_ = buffer_;
        buffer_.clear();
    }
    else if (root_ && tagsToClose.contains(element)) {
        if (buffer_.length() > 0) {
            DocBookModel* text = new DocBookModel(root_, DocBookModel::Text);
            text->text_ = buffer_;
            root_->children_.append(text);
            buffer_.clear();
        }
        DocBookModel* parent = root_->parent();
        if (parent) {
            parent->children_.append(root_);
            root_ = parent;
        }
        else {
            doc_ = root_;
            root_ = 0;
        }
    }
    return true;
}

}
