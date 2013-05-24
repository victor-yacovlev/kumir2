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
    ModelPtr content;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        content = parseDocument(&file, url, error);
        file.close();
    }
    return Document(url, content);
}

ModelPtr DocBookFactory::parseDocument(QIODevice *stream,
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
            doc_.clear();
        const QString errorText = reader_->errorHandler()->errorString();
        if (error) {
            *error = errorText;
        }
        return ModelPtr();
    }
}

void DocBookFactory::filterByOs(ModelPtr root) const
{
    if (!root)
        return;
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
    QList<ModelPtr> newList;
    for (ModelIterator it = root->children_.begin();
         it!=root->children_.end();
         it++)
    {
        ModelPtr child = *it;
        bool toDelete = false;
        if (child->os_.length() > 0) {
            const QString os = child->os_.toLower().trimmed();
            toDelete = os.indexOf(pattern) == -1;
            if (os.startsWith("!") || os.startsWith("not")) {
                toDelete = !toDelete;
            }
        }
        if (!toDelete) {
            newList.push_back(child);
        }
        else {
            filterByOs(child);
        }
    }
    root->children_ = newList;
}

bool DocBookFactory::startDocument()
{
    doc_.clear();
    root_.clear();
    return true;
}

bool DocBookFactory::startElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName,
        const QXmlAttributes &atts)
{
    const QString element = localName.toLower();
    static const QRegExp XInclude("http://www.w3.org/\\d+/XInclude");
    static const QRegExp MathML("http://www.w3.org/\\d+/Math/MathML");
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
    else if (element == "code") {
        model = new DocBookModel(root_, DocBookModel::Code);
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
    else if (element == "table") {
        model = new DocBookModel(root_, DocBookModel::Table);
    }
    else if (element == "informaltable") {
        model = new DocBookModel(root_, DocBookModel::InformalTable);
    }
    else if (element == "thead") {
        model = new DocBookModel(root_, DocBookModel::THead);
    }
    else if (element == "tbody") {
        model = new DocBookModel(root_, DocBookModel::TBody);
    }
    else if (element == "row") {
        model = new DocBookModel(root_, DocBookModel::Row);
    }
    else if (element == "entry") {
        model = new DocBookModel(root_, DocBookModel::Entry);
    }
    else if (element == "mediaobject") {
        model = new DocBookModel(root_, DocBookModel::MediaObject);
    }
    else if (element == "caption") {
        model = new DocBookModel(root_, DocBookModel::Caption);
    }
    else if (element == "inlinemediaobject") {
        model = new DocBookModel(root_, DocBookModel::InlineMediaObject);
    }
    else if (element == "imageobject") {
        model = new DocBookModel(root_, DocBookModel::ImageObject);
    }
    else if (element == "imagedata") {
        model = new DocBookModel(root_, DocBookModel::ImageData);
    }
    else if (element == "subscript") {
        model = new DocBookModel(root_, DocBookModel::Subscript);
    }
    else if (element == "superscript") {
        model = new DocBookModel(root_, DocBookModel::Superscript);
    }
    else if (element == "funcsynopsis") {
        model = new DocBookModel(root_, DocBookModel::FuncSynopsys);
    }
    else if (element == "funcsynopsisinfo") {
        model = new DocBookModel(root_, DocBookModel::FuncSynopsysInfo);
    }
    else if (element == "funcprototype") {
        model = new DocBookModel(root_, DocBookModel::FuncPrototype);
    }
    else if (element == "funcdef") {
        model = new DocBookModel(root_, DocBookModel::FuncDef);
    }
    else if (element == "function") {
        model = new DocBookModel(root_, DocBookModel::Function);
    }
    else if (element == "paramdef") {
        model = new DocBookModel(root_, DocBookModel::ParamDef);
    }
    else if (element == "parameter") {
        model = new DocBookModel(root_, DocBookModel::Parameter);
    }
    else if (element == "type") {
        model = new DocBookModel(root_, DocBookModel::Type);
    }
    else if (element == "package") {
        model = new DocBookModel(root_, DocBookModel::Package);
    }
    else if (element == "xref") {
        model = new DocBookModel(root_, DocBookModel::Xref);
        model->xrefLinkEnd_ = atts.value("linkend");
        model->xrefEndTerm_ = atts.value("endterm");
    }
    else if (element == "title" || element == "subtitle") {
        buffer_.clear();
    }
    else if (element == "include" && XInclude.indexIn(namespaceURI)!=-1) {
        const QString href = atts.value("href");
        if (href.length() > 0) {
            const QUrl hrefUrl = url_.resolved(href);
            // TODO network url support
            const QString fileName = hrefUrl.toLocalFile();
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly)) {
                DocBookFactory* innerFactory = new DocBookFactory();
                QString localError;
                ModelPtr include =
                        innerFactory->parseDocument(&file, hrefUrl, &localError);
                if (include) {
                    if (root_) {
                        include->parent_ = root_;
                        root_->children_.append(include);
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
    else if (element == "math" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_Math);
    }
    else if (element == "mrow" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MRow);
    }
    else if (element == "msqrt" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MSqrt);
    }
    else if (element == "mfrac" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MFrac);
    }
    else if (element == "mi" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MI);
    }
    else if (element == "mn" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MN);
    }
    else if (element == "mo" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MO);
    }
    else if (element == "mtext" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MText);
    }
    else if (element == "msup" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, DocBookModel::MathML_MSup);
    }
    else {
        model = new DocBookModel(root_, DocBookModel::Unknown);
        buffer_.clear();
    }
    if (model) {
        if (root_ && buffer_.length() > 0) {
            DocBookModel* text = new DocBookModel(root_, DocBookModel::Text);
            text->text_ = buffer_;
            root_->children_.append(ModelPtr(text));
            buffer_.clear();
        }
        model->id_ = atts.value("id");
        model->os_ = atts.value("os");
        model->role_ = atts.value("role");
        if (atts.value("language").length() > 0) {
            if (model->modelType_==DocBookModel::ProgramListing ||
                    model->modelType_==DocBookModel::Code)
            {
                model->role_ = atts.value("language");
            }
        }
        if (model->modelType() == DocBookModel::ImageData) {
            model->format_ = atts.value("format");
            const QString href = atts.value("fileref");
            if (href.length() > 0) {
                model->href_ = url_.resolved(href);
                if (model->format()=="png") {
                    model->cachedImage_ = QImage(model->href().toLocalFile());
                }
                else if (model->format()=="svg") {
                    model->svgRenderer_ = SvgRendererPtr(
                                new QSvgRenderer(model->href().toLocalFile())
                                );
                }
            }
        }
        root_ = ModelPtr(model);
    }
    return true;
}

bool DocBookFactory::characters(const QString &ch)
{
    bool preformatMode = root_ &&
            ( root_->modelType_ == DocBookModel::ProgramListing ||
              root_->modelType_ == DocBookModel::Code );
    if (preformatMode) {
        buffer_ += ch;
    }
    else {
//        if (buffer_.length() > 0 && ch.trimmed().length() > 0) {
//            buffer_.push_back(' ');
//        }
        buffer_ += ch.simplified();
    }
    return true;
}

bool DocBookFactory::skippedEntity(const QString &name)
{
    if (name == "nbsp") {
        buffer_.push_back(QChar::Nbsp);
    }
    else if (name == "lt") {
        buffer_.push_back('<');
    }
    else if (name == "gt") {
        buffer_.push_back('>');
    }
    else if (name == "le") {
        buffer_.push_back(QChar(0x2264));  // less or equal
    }
    else if (name == "ge") {
        buffer_.push_back(QChar(0x2265));  // greater or equal
    }
    else if (name == "times") {
        // See: http://www.w3.org/TR/xhtml1/DTD/xhtml-lat1.ent
        buffer_.push_back(QChar(0x00D7)); // multiplication sign
    }
    else if (name == "hellip") {
        // See: http://www.w3.org/TR/xhtml1/DTD/xhtml-symbol.ent
        buffer_.push_back(QChar(0x2026)); // three dots at bottom
    }
    else if (name == "alpha") {
        buffer_.push_back(QChar(0x03B1));
    }
    else if (name == "beta") {
        buffer_.push_back(QChar(0x03B2));
    }
    else if (name == "gamma") {
        buffer_.push_back(QChar(0x03B3));
    }
    return true;
}

bool DocBookFactory::endElement(const QString &namespaceURI,
                                const QString &localName,
                                const QString &qName)
{
    const QString element = localName.toLower();
    static const QRegExp XInclude("http://www.w3.org/\\d+/XInclude");
    if (root_ && element == "title") {
        root_->title_ = buffer_;
        buffer_.clear();
    }
    else if (root_ && element == "subtitle") {
        root_->subtitle_ = buffer_;
        buffer_.clear();
    }
    else if (element == "include" && XInclude.indexIn(namespaceURI)!=-1) {
        // do nothing here
    }
    else if (root_) {
        if (root_->title().isEmpty()) {
            if (root_ == DocBookModel::Example || root_ == DocBookModel::FuncSynopsys) {
                ModelPtr parent = root_->parent();
                while (parent) {
                    if (parent == DocBookModel::Section
                            && !parent->title().isEmpty())
                    {
                        root_->title_ = parent->title();
                        break;
                    }
                    parent = parent->parent();
                }
            }
        }
        if (buffer_.length() > 0) {
            DocBookModel* text = new DocBookModel(root_, DocBookModel::Text);
            text->text_ = buffer_;
            if (root_ == DocBookModel::Code) {
                text->text_.replace(' ', QChar(QChar::Nbsp));
            }
            root_->children_.append(ModelPtr(text));
            buffer_.clear();
        }
        ModelPtr parent = root_->parent();
        if (parent) {
            parent->children_.append(root_);
            root_ = parent;
        }
        else {
            doc_ = root_;
            root_.clear();
        }
    }
    return true;
}

bool DocBookFactory::error(const QXmlParseException &exception)
{
    qDebug() << "Error parsing " << url_;
    qDebug() << "At " << exception.lineNumber() << ":" << exception.columnNumber();
    qDebug() << exception.message();
    return false;
}

bool DocBookFactory::fatalError(const QXmlParseException &exception)
{
    qDebug() << "Fatal error parsing " << url_;
    qDebug() << "At " << exception.lineNumber() << ":" << exception.columnNumber();
    qDebug() << exception.message();
    return false;
}

bool DocBookFactory::warning(const QXmlParseException &exception)
{
    qDebug() << "Warning parsing " << url_;
    qDebug() << "At " << exception.lineNumber() << ":" << exception.columnNumber();
    qDebug() << exception.message();
    return true;
}

QList<ModelPtr> DocBookFactory::findEntriesOfType(
        ModelPtr root,
        DocBookModel::ModelType findType
        )
{
    QList<ModelPtr> result;
    if (root->modelType() == findType) {
        result += root;
    }
    else {
        foreach (ModelPtr child, root->children()) {
            result += findEntriesOfType(child, findType);
        }
    }
    return result;
}

ModelPtr DocBookFactory::createListOfEntries(ModelPtr root,
                                             DocBookModel::ModelType resType,
                                             DocBookModel::ModelType findType)
{
    ModelPtr result;
    QList<ModelPtr> entries = findEntriesOfType(root, findType);
    if (entries.size() > 0) {
        result = ModelPtr(new DocBookModel(ModelPtr(), resType));
        foreach (ModelPtr entry, entries) {
            result->children_.append(entry);
            entry->indexParent_ = result;
        }
        result->title_ = root->title();
        result->subtitle_ = root->subtitle();
    }
    return result;
}

QMap<QString, ModelPtr> & DocBookFactory::updateListOfAlgorithms(
        ModelPtr root,
        QMap<QString, ModelPtr> &result)
{
    QList<ModelPtr> allItems = findEntriesOfType(root, DocBookModel::FuncSynopsys);

    foreach (ModelPtr item, allItems) {
        QString moduleName;
        QList<ModelPtr> infos = findEntriesOfType(item,
                                                  DocBookModel::FuncSynopsysInfo);
        if (infos.size() > 0) {
            ModelPtr info = infos.first();
            QList<ModelPtr> packages = findEntriesOfType(info,
                                                         DocBookModel::Package);
            if (packages.size() > 0) {
                ModelPtr package = packages.first();
                foreach (ModelPtr packageChild, package->children()) {
                    if (packageChild == DocBookModel::Text) {
                        if (moduleName.length() > 0)
                            moduleName += " ";
                        moduleName += packageChild->text().trimmed();
                    }
                }
            }
        }

        ModelPtr moduleRoot;
        if (result.contains(moduleName)) {
            moduleRoot = result[moduleName];
        }
        else {
            moduleRoot = ModelPtr(new DocBookModel(
                                      ModelPtr(),
                                      DocBookModel::ListOfFunctions
                                      )
                                  );
            result[moduleName] = moduleRoot;
            moduleRoot->title_ = moduleName;
        }
        item->indexParent_ = moduleRoot;
        moduleRoot->children_.append(item);
    }

    return result;
}

}
