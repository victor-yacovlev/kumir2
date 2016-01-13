// Self includes
#include "docbookfactory.h"
#include "docbookmodel.h"
#include "document.h"

// Qt includes
#include <QtCore>
#include <QtXml>
#include <QApplication>
#include <QPalette>

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

Document DocBookFactory::parseDocument(const QMap<ModelType,QString> roleValues, const QUrl &url, QString *error) const
{
    // TODO network url loading
    const QString fileName = url.toLocalFile();
    ModelPtr content;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        content = parseDocument(roleValues, &file, url, error);
        file.close();
    }
    return Document(url, content);
}

Document DocBookFactory::createNamedSet(const QString &name, const QList<Document> documents) const
{
    ModelPtr namedSetRoot(new DocBookModel(ModelPtr(), Set));
    namedSetRoot->title_ = name;
    Q_FOREACH(const Document & doc, documents) {
        namedSetRoot->children_.append(doc.root_);
        doc.root_->parent_ = namedSetRoot;
    }

    return Document(QUrl(), namedSetRoot);
}

void DocBookFactory::setConfigurationName(const QString &configurationName)
{
    configurationName_ = configurationName;
}

ModelPtr DocBookFactory::parseDocument(
        const QMap<ModelType,QString> & roles,
        QIODevice *stream,
        const QUrl & url,
        QString *error
        ) const
{
    roles_ = roles;
    url_ = url;
    QXmlInputSource source(stream);
    if (reader_->parse(source)) {
        if (error)
            error->clear();
        filterByOs(doc_);
        filterByConfiguration(doc_);
        filterByRoles(roles, doc_);
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
            filterByOs(child);
        }
    }
    root->children_ = newList;
}

void DocBookFactory::filterByConfiguration(ModelPtr root) const
{
    if (!root)
        return;

    QString confName;
    if (configurationName_.isEmpty()) {
        static const QString applicationLanucher = QDir::fromNativeSeparators(qApp->arguments().at(0));
        confName =
                applicationLanucher.startsWith(qApp->applicationDirPath())
                ? applicationLanucher.mid(qApp->applicationDirPath().length() + 1)
                : applicationLanucher;
    #ifdef Q_OS_WIN32
        if (confName.endsWith(".exe")) {
            confName.remove(confName.length()-4, 4);
        }
    #endif
        confName.remove("kumir2-");
    }
    else {
        confName = configurationName_;
    }
    QList<ModelPtr> newList;
    for (ModelIterator it = root->children_.begin();
         it!=root->children_.end();
         it++)
    {
        ModelPtr child = *it;
        bool toDelete = false;
        if (child->configuration_.length()) {
            const QString & childConf = child->configuration_;
            toDelete = childConf.toLower() != confName;
        }
        if (!toDelete) {
            newList.push_back(child);
            filterByConfiguration(child);
        }
    }
    root->children_ = newList;
}

void DocBookFactory::filterByRoles(const QMap<ModelType, QString> &roles, ModelPtr root) const
{
    if (!root)
        return;
    QList<ModelPtr> newList;
    for (ModelIterator it = root->children_.begin();
         it!=root->children_.end();
         it++)
    {
        ModelPtr child = *it;
        bool toDelete = false;
        if (child->role_.length() > 0 && roles.contains(child->modelType_)) {
            const QString & matchRole = roles[child->modelType_];
            const QString & childRole = child->role_;
            toDelete = childRole.toLower() != matchRole;
        }
        if (!toDelete) {
            newList.push_back(child);
            filterByRoles(roles, child);
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
        model = new DocBookModel(root_, Article);
    }
    else if (element == "book") {
        model = new DocBookModel(root_, Book);
    }
    else if (element == "set") {
        model = new DocBookModel(root_, Set);
    }
    else if (element == "abstract") {
        model = new DocBookModel(root_, Abstract);
    }
    else if (element == "preface") {
        model = new DocBookModel(root_, Preface);
    }
    else if (element == "chapter") {
        model = new DocBookModel(root_, Chapter);
    }
    else if (element == "section") {
        model = new DocBookModel(root_, Section);
    }
    else if (element == "para") {
        model = new DocBookModel(root_, Para);
    }
    else if (element == "programlisting") {
        model = new DocBookModel(root_, ProgramListing);
    }
    else if (element == "code") {
        model = new DocBookModel(root_, Code);
    }
    else if (element == "example") {
        model = new DocBookModel(root_, Example);
    }
    else if (element == "orderedlist") {
        model = new DocBookModel(root_, OrderedList);
    }
    else if (element == "itemizedlist") {
        model = new DocBookModel(root_, ItemizedList);
    }
    else if (element == "listitem") {
        model = new DocBookModel(root_, ListItem);
    }
    else if (element == "emphasis") {
        model = new DocBookModel(root_, Emphasis);
        model->role_ = atts.value("role");
    }
    else if (element == "keycombo") {
        model = new DocBookModel(root_, KeyCombo);
    }
    else if (element == "keysym") {
        model = new DocBookModel(root_, KeySym);
    }
    else if (element == "table") {
        model = new DocBookModel(root_, Table);
    }
    else if (element == "informaltable") {
        model = new DocBookModel(root_, InformalTable);
    }
    else if (element == "thead") {
        model = new DocBookModel(root_, THead);
    }
    else if (element == "tbody") {
        model = new DocBookModel(root_, TBody);
    }
    else if (element == "row") {
        model = new DocBookModel(root_, Row);
    }
    else if (element == "entry") {
        model = new DocBookModel(root_, Entry);
    }
    else if (element == "mediaobject") {
        model = new DocBookModel(root_, MediaObject);
    }
    else if (element == "caption") {
        model = new DocBookModel(root_, Caption);
    }
    else if (element == "inlinemediaobject") {
        model = new DocBookModel(root_, InlineMediaObject);
    }
    else if (element == "imageobject") {
        model = new DocBookModel(root_, ImageObject);
    }
    else if (element == "imagedata") {
        model = new DocBookModel(root_, ImageData);
    }
    else if (element == "subscript") {
        model = new DocBookModel(root_, Subscript);
    }
    else if (element == "superscript") {
        model = new DocBookModel(root_, Superscript);
    }
    else if (element == "funcsynopsis") {
        model = new DocBookModel(root_, FuncSynopsys);
    }
    else if (element == "funcsynopsisinfo") {
        model = new DocBookModel(root_, FuncSynopsysInfo);
    }
    else if (element == "funcprototype") {
        model = new DocBookModel(root_, FuncPrototype);
    }
    else if (element == "funcdef") {
        model = new DocBookModel(root_, FuncDef);
    }
    else if (element == "function") {
        model = new DocBookModel(root_, Function);
    }
    else if (element == "paramdef") {
        model = new DocBookModel(root_, ParamDef);
    }
    else if (element == "parameter") {
        model = new DocBookModel(root_, Parameter);
    }
    else if (element == "type") {
        model = new DocBookModel(root_, Type);
    }
    else if (element == "package") {
        model = new DocBookModel(root_, Package);
    }
    else if (element == "guimenu") {
        model = new DocBookModel(root_, GuiMenu);
    }
    else if (element == "guimenuitem") {
        model = new DocBookModel(root_, GuiMenuItem);
    }
    else if (element == "guibutton") {
        model = new DocBookModel(root_, GuiButton);
    }
    else if (element == "xref") {
        model = new DocBookModel(root_, Xref);
        model->xrefLinkEnd_ = atts.value("linkend");
        model->xrefEndTerm_ = atts.value("endterm");
    }
    else if (element == "title" || element == "subtitle" || element == "titleabbrev") {
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
                innerFactory->configurationName_ = configurationName_;
                QString localError;
                ModelPtr include =
                        innerFactory->parseDocument(roles_, &file, hrefUrl, &localError);
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
        model = new DocBookModel(root_, MathML_Math);
    }
    else if (element == "mrow" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MRow);
    }
    else if (element == "msqrt" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MSqrt);
    }
    else if (element == "mfrac" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MFrac);
    }
    else if (element == "mi" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MI);
    }
    else if (element == "mn" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MN);
    }
    else if (element == "mo" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MO);
    }
    else if (element == "mtext" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MText);
    }
    else if (element == "msup" && MathML.indexIn(namespaceURI) != -1) {
        model = new DocBookModel(root_, MathML_MSup);
    }    
    else {
        model = new DocBookModel(root_, Unknown);
        buffer_.clear();
    }
    if (model) {
        if (root_ && buffer_.length() > 0) {
            DocBookModel* text = new DocBookModel(root_, Text);
            text->text_ = buffer_;
            root_->children_.append(ModelPtr(text));
            buffer_.clear();
        }
        model->id_ = atts.value("id");
        model->os_ = atts.value("os");
        model->role_ = atts.value("role");
        model->configuration_ = atts.value("configuration");
        if (atts.value("language").length() > 0) {
            if (model->modelType_==ProgramListing ||
                    model->modelType_==Code)
            {
                model->role_ = atts.value("language");
            }
        }
        if (model->modelType() == ImageData) {
            model->format_ = atts.value("format");
            const QString href = atts.value("fileref");
            if (href.length() > 0) {
                model->href_ = url_.resolved(href);
                if (model->format()=="png") {
                    model->cachedImage_ = loadAndPreprocessPng(model->href().toLocalFile());
                }
                else if (model->format()=="svg") {
                    model->svgRenderer_ = SvgRendererPtr(
                                new QSvgRenderer(loadAndPreprocessSvg(model->href().toLocalFile()))
                                );
                }
            }
        }
        root_ = ModelPtr(model);
    }
    return true;
}

QByteArray DocBookFactory::loadAndPreprocessSvg(const QString &fileName)
{
    QByteArray result;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return result;
    }
    result = file.readAll();
    file.close();
    QCoreApplication * app = qApp->instance();
    QApplication * guiApp = qobject_cast<QApplication*>(app);
    if (guiApp) {
        static const QPalette palette = QApplication::palette();
        static const QByteArray foreground =
                palette.brush(QPalette::Text).color().name().toLatin1();
        static const QByteArray background =
                palette.brush(QPalette::Base).color().name().toLatin1();
        result.replace("fill:foreground", "fill:" + foreground);
        result.replace("fill:background", "fill:"+background);
    }
    return result;
}

QImage DocBookFactory::loadAndPreprocessPng(const QString &fileName)
{
    QImage source(fileName);
    QImage result;
    if (source.allGray()) {
        QCoreApplication * app = qApp->instance();
        QApplication * guiApp = qobject_cast<QApplication*>(app);
        if (guiApp) {
            static const QPalette palette = QApplication::palette();
            static const QRgb foreground =
                    palette.brush(QPalette::Text).color().rgba();
            static const QRgb background =
                    palette.brush(QPalette::Base).color().rgba();
            static int normRed = qRed(foreground)-qRed(background);
            static int normBlue = qBlue(foreground)-qBlue(background);
            static int normGreen = qGreen(foreground)-qGreen(background);
            static int bgRed = qRed(background);
            static int bgBlue = qBlue(background);
            static int bgGreen = qGreen(background);
            result = QImage(source.size(), QImage::Format_ARGB32);
            result.fill(0);
            for (int y=0; y<source.height(); y++) {
                for (int x=0; x<source.width(); x++) {
                    QRgb sourcePixel = source.pixel(QPoint(x, y));
                    int sourceAlpha = qAlpha(sourcePixel);
                    qreal sourceIntensivity = 1.0 -
                            (qRed(sourcePixel) + qGreen(sourcePixel) + qBlue(sourcePixel)) /
                            (3.0 * 255.0) ;
                    int resultRed = bgRed + normRed * sourceIntensivity;
                    int resultBlue = bgBlue + normBlue * sourceIntensivity;
                    int resultGreen = bgGreen + normGreen * sourceIntensivity;
                    QRgb resultPixel = qRgba(resultRed, resultGreen, resultBlue, sourceAlpha);
                    result.setPixel(QPoint(x, y), resultPixel);
                }
            }
        }
    }

    if (result.isNull()) {
        result = source;
    }

    return result;
}

bool DocBookFactory::characters(const QString &ch)
{
    bool preformatMode = root_ &&
            ( root_->modelType_ == ProgramListing ||
              root_->modelType_ == Code );
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
    // Arrows from http://www.w3.org/TR/xhtml1/DTD/xhtml-symbol.ent
    else if (name == "rarr") {
        buffer_.push_back(QChar(0x2192));
    }
    else if (name == "larr") {
        buffer_.push_back(QChar(0x2190));
    }
    else if (name.startsWith("#")) {
        const QString sCode = name.mid(1);
        bool ok = false;
        unsigned int code = sCode.toUInt(&ok);
        if (ok) {
            const QChar symbol(code);
            buffer_.push_back(symbol);
        }
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
    else if (root_ && element == "titleabbrev") {
        root_->titleAbbrev_ = buffer_;
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
            if (root_ == Example || root_ == FuncSynopsys) {
                ModelPtr parent = root_->parent();
                while (parent) {
                    if (parent == Section
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
            DocBookModel* text = new DocBookModel(root_, Text);
            text->text_ = buffer_;
            if (root_ == Code) {
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
        ModelType findType
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
                                             ModelType resType,
                                             ModelType findType)
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
    QList<ModelPtr> allItems = findEntriesOfType(root, FuncSynopsys);

    foreach (ModelPtr item, allItems) {
        QString moduleName;
        QList<ModelPtr> infos = findEntriesOfType(item,
                                                  FuncSynopsysInfo);
        if (infos.size() > 0) {
            ModelPtr info = infos.first();
            QList<ModelPtr> packages = findEntriesOfType(info,
                                                         Package);
            if (packages.size() > 0) {
                ModelPtr package = packages.first();
                foreach (ModelPtr packageChild, package->children()) {
                    if (packageChild == Text) {
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
                                      ListOfFunctions
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
