#include "contentview.h"
#include "docbookmodel.h"
#include "mathmlrenderer.h"

#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

static const QString MainFontFamily =
        "Droid Serif,PT Serif,Garamond,Times New Roman,serif";

#ifdef Q_OS_MAC
static const QString MainFontSize = "14pt";
static const QString CodeFontSize = "14pt";
#else
static const QString MainFontSize = "12pt";
static const QString CodeFontSize = "12pt";
#endif

static const QString CodeFontFamily =
        "DejaVu Sans Mono,Liberation Mono,PT Sans Mono,Courier New,monospace";

bool ContentView::ExtraFontsLoaded_ = false;

ContentView::ContentView(QWidget *parent)
    : QTextBrowser(parent)
{
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(handleInternalLink(QUrl)));
    connect(verticalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(clearLastAnchorUrl()));
    ignoreClearAnchorUrl_ = false;

#ifdef Q_OS_WIN32
    if (!ExtraFontsLoaded_) {
        const QString resourcesRoot = QCoreApplication::instance()->property("sharePath").toString();
        const QDir fontsDir = QDir(resourcesRoot + "/docbookviewer");
        const QStringList ttfFiles = fontsDir.entryList(QStringList() << "*.ttf" << "*.otf");
        foreach (const QString & fileName, ttfFiles) {
            const QString filePath = fontsDir.absoluteFilePath(fileName);
            int id = QFontDatabase::addApplicationFont(filePath);
            if (id == -1) {
                qWarning() << "Can't load font " << filePath;
            }
        }
        ExtraFontsLoaded_ = true;
    }
#endif
}

QSize ContentView::minimumSizeHint() const
{
    return QSize(500, 230);
}

void ContentView::reset()
{
    loadedModel_.clear();
    clear();    
}

bool ContentView::isEmpty() const
{
    return toPlainText().trimmed().isEmpty();
}

void ContentView::renderData(ModelPtr data)
{
    ModelPtr dataToRender = onePageParentModel(data);
    if (dataToRender != loadedModel_) {
        loadedModel_ = dataToRender;
        const QString html = wrapHTML(renderModel(dataToRender));
        setHtml(html);
    }
    if (dataToRender != data) {
        QString anchor = modelToLink(data);
        QUrl anchorUrl("#" + anchor);
        setSource(anchorUrl);
        lastAnchorUrl_ = anchorUrl;
    }
}

QString ContentView::renderChapter(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    foreach (ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString ContentView::renderAbstract(ModelPtr data) const
{
    QString result;
    result += "<div class='abstract'>";
    foreach (ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "</div>";
    return result;
}

QString ContentView::renderArticle(ModelPtr data) const
{
    QString result;
    result += "<h1 align='center'>" + normalizeText(data->title()) + "</h1>\n";
    ModelPtr abstract;
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::Abstract) {
            abstract = child;
            break;
        }
    }
    if (abstract) {
        result += renderAbstract(abstract);
    }
    result += "<hr/>";
    foreach (ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}


QString ContentView::wrapHTML(const QString &body) const
{
    return QString() +
            "<html><head>"
            "<style type=\"text/css\">"
            "a {"
            "   color: gray;"
            "   font-weight: bold;"
            "   text-decoration: none;"
            "}"
            ".abstract {"
            "   margin: 30;"
            "   font-style: italic;"
            "}"
            ".subtitle {"
            "   font-size: " + MainFontSize + ";"
            "}"
            "body {"
            "   font-family: " + MainFontFamily + ";"
            "   font-size: " + MainFontSize + ";"
            "   margin: 10;"
            "}"
            ".code {"
            "   font-family: " + CodeFontFamily + ";"
            "   font-size: " + CodeFontSize + ";"
            "}"
            "h2 {"
            "   align: center;"
            "   margin: 30;"
            "}"
            "kbd {"
            "   background-color: lightgray;"
            "}"
            "</style></head>"
            "<body>\n" + body +"\n</body></html>";
}

QString ContentView::renderModel(ModelPtr data) const
{
    if (data->modelType() == DocBookModel::Set) {
        return renderSet(data);
    }
    else if (data->modelType() == DocBookModel::Book)
    {
        return renderTOC(data);
    }
    else {
        return renderPlainPage(data);
    }
}

bool ContentView::isPlainPage(ModelPtr data) const
{
    quint32 sectionedChilds = 0;
    foreach (ModelPtr child, data->children()) {
        if (child->isSectioningNode())
            sectionedChilds += 1;
    }
    return sectionedChilds == 0;
}

QString ContentView::renderPlainPage(ModelPtr data) const
{
    return renderElement(data);
}

QString ContentView::renderChilds(ModelPtr data) const
{
    QString result;
    foreach (ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString ContentView::renderElement(ModelPtr data) const
{
    if (data == DocBookModel::Text) {
        return renderText(data);
    }
    else if (data == DocBookModel::Para) {
        return renderParagraph(data);
    }
    else if (data == DocBookModel::Emphasis) {
        return renderEmphasis(data);
    }
    else if (data == DocBookModel::ListItem) {
        return renderListItem(data);
    }
    else if (data == DocBookModel::OrderedList) {
        return renderOrderedList(data);
    }
    else if (data == DocBookModel::ItemizedList) {
        return renderItemizedList(data);
    }
    else if (data == DocBookModel::Chapter) {
        return renderChapter(data);
    }
    else if (data == DocBookModel::Article) {
        return renderArticle(data);
    }    
    else if (data == DocBookModel::Section) {
        return renderSection(data);
    }
    else if (data == DocBookModel::Example) {
        return renderExample(data);
    }
    else if (data == DocBookModel::ProgramListing) {
        return renderProgramListing(data);
    }
    else if (data == DocBookModel::Code) {
        return renderCode(data);
    }
    else if (data == DocBookModel::Xref) {
        return renderXref(data);
    }
    else if (data == DocBookModel::KeyCombo) {
        return renderKeyCombo(data);
    }
    else if (data == DocBookModel::KeySym) {
        return renderKeySym(data);
    }
    else if (data == DocBookModel::InformalTable) {
        return renderInformalTable(data);
    }
    else if (data == DocBookModel::Table) {
        return renderTable(data);
    }
    else if (data == DocBookModel::THead) {
        return renderTHead(data);
    }
    else if (data == DocBookModel::TBody) {
        return renderTBody(data);
    }
    else if (data == DocBookModel::Row) {
        return renderRow(data);
    }
    else if (data == DocBookModel::Entry) {
        return renderEntry(data);
    }
    else if (data == DocBookModel::Subscript) {
        return renderSubscript(data);
    }
    else if (data == DocBookModel::Superscript) {
        return renderSuperscript(data);
    }
    else if (data == DocBookModel::MediaObject) {
        return renderMediaObject(data);
    }
    else if (data == DocBookModel::Caption) {
        return renderCaption(data);
    }
    else if (data == DocBookModel::InlineMediaObject) {
        return renderInlineMediaObject(data);
    }
    else if (data == DocBookModel::ImageObject) {
        return renderImageObject(data);
    }
    else if (data == DocBookModel::FuncSynopsys) {
        return renderFuncSynopsys(data);
    }
    else if (data == DocBookModel::Function) {
        return renderFunction(data);
    }
    else if (data == DocBookModel::Parameter) {
        return renderParameter(data);
    }
    else if (data == DocBookModel::Type) {
        return renderType(data);
    }
    else if (data == DocBookModel::MathML_Math) {
        return renderMathML(data);
    }
    else if (data == DocBookModel::ListOfExamples) {
        return renderListOfExamples(data);
    }
    else if (data == DocBookModel::ListOfTables) {
        return renderListOfTables(data);
    }
    else if (data == DocBookModel::ListOfFunctions) {
        return renderListOfFunctions(data);
    }
    else {
        return "";
    }
}

QString ContentView::renderKeyCombo(ModelPtr data) const
{
    QString result;
    for (int i=0; i<data->children().size(); i++) {
        if (i>0) {
            result += "&nbsp;";
        }
        result += renderElement(data->children().at(i));
    }
    return result;
}

QString ContentView::renderKeySym(ModelPtr data) const
{
    QString result;
    QString keysym;
    foreach (ModelPtr  child, data->children()) {
        keysym += child->text();
    }
    const QStringList keys = keysym.split("+", QString::SkipEmptyParts);
    QStringList parts;
    foreach (const QString & key, keys) {
        QString part = "<kbd>";
        part += key.toUpper();
        part += "</kbd>";
        parts.push_back(part);
    }
    result = parts.join("+");
    return result;
}

QString ContentView::programTextForLanguage(const QString &source,
                                            const QString &language)
{
    QStringList keywordsList;
    QString inlineCommentSymbol;
    QString multilineCommentStartSymbol;
    QString multilineCommentEndSymbol;

    if (language.toLower() == "kumir") {
        keywordsList = QString::fromUtf8("алг,нач,кон,нц,кц,кц_при,если,"
                                         "то,иначе,все,выбор,при,утв,"
                                         "дано,надо,ввод,вывод,пауза,"
                                         "использовать,исп,кон_исп,"
                                         "цел,вещ,лит,сим,лог,таб,"
                                         "целтаб,вещтаб,"
                                         "литтаб,симтаб,логтаб,"
                                         "арг,рез,аргрез,пока,для,от,до,знач,"
                                         "да,нет,не,и,или,раз,нс,файл,"
                                         "выход,шаг"
                                         ).split(",");
        inlineCommentSymbol = "|";
    }
    else if (language.toLower() == "pascal") {
        keywordsList = QString::fromAscii("begin,end,program,unit,uses,for,from,"
                                         "to,if,then,else,"
                                         "integer,real,string,char,boolean,"
                                         "array,of"
                                         ).split(",");
        inlineCommentSymbol = "//";
        multilineCommentStartSymbol = "{";
        multilineCommentEndSymbol = "}";
    }
    return formatProgramSourceText(
                source.trimmed(),
                keywordsList,
                inlineCommentSymbol,
                multilineCommentStartSymbol,
                multilineCommentEndSymbol
                ).trimmed();
}

QString ContentView::renderProgramListing(ModelPtr data) const
{
    QString result = "<pre align='left' class='code'>";
    const QString programText = renderChilds(data);
    result += programTextForLanguage(programText, data->role());
    result += "</pre>\n";
    return result;
}

QString ContentView::renderCode(ModelPtr data) const
{
    QString result = "<span class='code'>";
    const QString programText = renderChilds(data);
    result += programTextForLanguage(programText, data->role());
    result += "</span>";
    return wrapInlineElement(data, result, true, true);
    return result;
}

QString ContentView::renderTableContent(ModelPtr data) const
{
    QString result;
    result += "<table border='1' bordercolor='black' cellspacing='0' cellpadding='0' width='100%'>\n";
    result += "<tr><td>\n";
    result += "<table border='0' cellspacing='0' cellpadding='10' width='100%'>\n";
    result += renderChilds(data);
    result += "</table>\n";
    result += "</td></tr>\n";
    result += "</table>\n";
    return result;
}

QString ContentView::renderTHead(ModelPtr data) const
{
    QString result;
    result += "<thead>\n";
    result += renderChilds(data);
    result += "</thead>\n";
    return result;
}

QString ContentView::renderTBody(ModelPtr data) const
{
    QString result;
    result += "<tbody>\n";
    result += renderChilds(data);
    result += "</tbody>\n";
    return result;
}

QString ContentView::renderRow(ModelPtr data) const
{
    ModelPtr parent = data->parent();
    bool inTableHead = false;
    bool inTableBody = false;
    while (parent) {
        if (parent->modelType()==DocBookModel::THead) {
            inTableHead = true;
            break;
        }
        if (parent->modelType()==DocBookModel::TBody) {
            inTableBody = true;
            break;
        }
        parent = parent->parent();
    }
    QString result;
    if (inTableHead) {
        result += "<tr valign='center' bgcolor='lightgray'>\n";
    }
    else {
        result += "<tr valign='center'>\n";
    }
    result += renderChilds(data);
    result += "</tr>\n";
    return result;
}

QString ContentView::renderEntry(ModelPtr data) const
{
    ModelPtr parent = data->parent();
    bool inTableHead = false;
    bool inTableBody = false;
    while (parent) {
        if (parent->modelType()==DocBookModel::THead) {
            inTableHead = true;
            break;
        }
        if (parent->modelType()==DocBookModel::TBody) {
            inTableBody = true;
            break;
        }
        parent = parent->parent();
    }
    QString result;
    result += "<td align='center' valign='center'>\n";
    if (inTableHead) {
        result += "<b>";
    }
    result += renderChilds(data);
    if (inTableHead) {
        result += "</b>";
    }
    result += "</td>\n";
    return result;
}


QString ContentView::renderTable(ModelPtr data) const
{
    QString result;
    const QString & title = data->title();

    const QString index = chapterNumber(data) > 0
            ? QString("%1.%2")
              .arg(chapterNumber(data))
              .arg(elementNumber(data))
            : QString::number(elementNumber(data));

    if (loadedModel_ == DocBookModel::ListOfTables) {
        result += "<a name='" + modelToLink(data) + "'></a>\n";
        result += "<h2 align='left' style='margin: 0;'>" +
                tr("Table&nbsp;%1. ").arg(index) +
                "<span style='font-weight:normal;'>" +
                normalizeText(title) + "</span>" +
                "</h2>\n";
        result += renderItemContextLink(data);
        result += renderTableContent(data);
        result += "<hr/>";
    }
    else {
        result += "<a name='" + modelToLink(data) + "'></a>\n";
        result += "<table width='100%'>\n";
        result += "<tr><td height='10'>&nbsp;</td></tr>\n";
        result += "<tr><td align='left'><b>";
        result += tr("Table&nbsp;%1. ").arg(index);
        result += "</b>" + title + "</td></tr>\n";
        result += "<tr><td>\n";
        result += renderTableContent(data);
        result += "</td></tr>\n";
        result += "<tr><td height='10'>&nbsp;</td></tr>\n";
        result += "</table>\n";
    }
    return result;
}

QString ContentView::renderInformalTable(ModelPtr data) const
{
    QString result;
    result += renderTableContent(data);
    return result;
}

quint16 ContentView::indexInParent(ModelPtr data)
{
    quint16 result = 0;
    if (data->parent()) {
        foreach (ModelPtr child, data->parent()->children()) {
            if (child->modelType() == data->modelType()) {
                result += 1;
            }
            if (child == data) {
                break;
            }
        }
    }
    return result;
}

QString ContentView::sectionNumber(ModelPtr data)
{
    QString result;
    ModelPtr parent = data->parent();
    ModelPtr current = data;
    while (parent) {
        result = QString("%1.").arg(indexInParent(current)) + result;
        current = parent;
        parent = parent->parent();
        if (current == DocBookModel::Book || current == DocBookModel::Article) {
            break;
        }
    }
    return result;
}

quint16 ContentView::elementNumber(ModelPtr data)
{
    ModelPtr root = data->parent();
    while (root) {
        if (root == DocBookModel::Chapter ||
                root == DocBookModel::Book ||
                root == DocBookModel::Article)
        {
            break;
        }
        root = root->parent();
    }
    bool stop = false;
    return countOfElements(root, data, stop) + 1;
}

quint16 ContentView::countOfElements(ModelPtr root, ModelPtr until, bool &stop)
{
    quint16 result = 0;
    DocBookModel::ModelType rt = root->modelType();
    DocBookModel::ModelType ut = until->modelType();
    quint8 rl = root->sectionLevel();
    quint8 ul = until->sectionLevel();
    bool match = rt == ut;
    if (match && rt == DocBookModel::Section) {
        match = rl == ul;
    }
    if (root == until) {
        stop = true;
    }
    else if (match) {
        result = 1;
    }
    else {
        for (int i=0; i<root->children().size(); i++) {
            const ModelPtr & child = root->children()[i];
            result += countOfElements(child, until, stop);
            if (stop) {
                break;
            }
        }
    }
    return result;
}

quint16 ContentView::chapterNumber(ModelPtr data)
{
    ModelPtr topLevelSection;
    quint16 chapterNumber = 0;
    ModelPtr topLevelBook;

    topLevelSection = data->parent();
    while (topLevelSection) {
        if (topLevelSection->modelType() == DocBookModel::Chapter) {
            topLevelBook = topLevelSection->parent();
            if (topLevelBook) {
                for (int i = 0; i < topLevelBook->children().size(); i++) {
                    const ModelPtr & child = topLevelBook->children()[i];
                    if (child->modelType() == DocBookModel::Chapter) {
                        chapterNumber ++;
                        if (child == topLevelSection) {
                            break;
                        }
                    }
                }
            }
            break;
        }
        topLevelSection = topLevelSection->parent();
    }
    return chapterNumber;
}

QString ContentView::renderItemContextLink(ModelPtr data) const
{
    QString result;
    ModelPtr context = data->parent();
    while (context) {
        if (context == DocBookModel::Section ||
                context == DocBookModel::Chapter ||
                context == DocBookModel::Article ||
                context == DocBookModel::Book)
        {
            break;
        }
        context = context->parent();
    }
    if (context) {
        const QString & contextTitle = sectionNumber(context) + "&nbsp;" +
                context->title();
        const QString contextLink = "model_ptr:" + modelToLink(context);
        const QString contextAHref = "<a href='" + contextLink + "'>" +
                contextTitle + "</a>";
        result += "<p align='left'>"+tr("See %1 for more details.")
                .arg(contextAHref)+"</p>";
    }
    return result;
}

QString ContentView::renderExample(ModelPtr data) const
{
    QString result;
    const QString & title = data->title();

    const QString index = chapterNumber(data) > 0
            ? QString("%1.%2")
              .arg(chapterNumber(data))
              .arg(elementNumber(data))
            : QString::number(elementNumber(data));

    if (loadedModel_ == DocBookModel::ListOfExamples) {
        result += "<a name='" + modelToLink(data) + "'></a>\n";
        result += "<h2 align='left' style='margin: 0;'>" +
                tr("Example&nbsp;%1. ").arg(index) +
                "<span style='font-weight:normal;'>" +
                normalizeText(title) + "</span>" +
                "</h2>\n";
        result += renderItemContextLink(data);
        result += renderChilds(data);
        result += "<hr/>";
    }
    else {
        result += "<a name='" + modelToLink(data) + "'></a>\n";
        result += "<table width='100%'>\n";
        result += "<tr><td height='10'>&nbsp;</td></tr>\n";
        result += "<tr><td align='center'>\n";
        result += "<table border='1' bordercolor='gray' cellspacing='0' cellpadding='10' width='100%'>";
        result += "<tr><td>\n";
        result += renderChilds(data);
        result += "</td></tr></table>\n";
        result += "</td></tr>\n";
        result += "<tr><td align='center'>\n";
        result += "<b>" + tr("Example&nbsp;%1. ").arg(index) + "</b>";
        result += normalizeText(title);
        result += "</p></td></tr>\n";
        result += "</table>\n";
    }
    return result;
}

QString ContentView::renderFuncSynopsys(ModelPtr data) const
{
    QString result;
    result += "<a name='" + modelToLink(data) + "'></a>";
    ModelPtr info, prototype;
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::FuncSynopsysInfo)
            info = child;
        else if (child == DocBookModel::FuncPrototype)
            prototype = child;
    }
    if (loadedModel_ == data->indexParent()) {
        result += "<h2 align='left' style='margin: 0;'>" +
                tr("Algorithm ") +
                " <span style='font-weight:normal;'>" +
                normalizeText(data->title()) + "</span>" +
                "</h2>\n";

        bool hasMoreThanText = false;

        if (data->parent()) {
            foreach (ModelPtr child, data->parent()->children()) {
                if (child != DocBookModel::Para &&
                        child != DocBookModel::FuncSynopsys)
                {
                    hasMoreThanText = true;
                    break;
                }
            }
        }

        if (hasMoreThanText) {
            result += renderItemContextLink(data);
        }

        if (info)
            result += renderFuncSynopsysInfo(info);
    }
    if (prototype) {
        result += "<table border='0' width='100%'><tr><td>";
        result += "<br/>";
//            result += "<tr><td height='1'>&nbsp;</td></tr>\n";


        result += "<b>" + tr("Synopsis:") + "</b>";
        result += "</td></tr><tr><td>";
        result += "<table border='1' bordercolor='gray' cellspacing='0' cellpadding='10' width='100%'>";
        result += "<tr><td>";
        result += renderFuncPrototype(prototype);
        result += "</td></tr><table></td></tr></table>\n";
    }
    if (loadedModel_ == data->indexParent() && data->parent()) {
        int from = data->parent()->children().indexOf(data);
        for (int i=from + 1; i<data->parent()->children().size(); i++) {
            ModelPtr child = data->parent()->children()[i];
            if (child == DocBookModel::Para) {
                result += renderParagraph(child);
            }
            else if (child == DocBookModel::FuncSynopsysInfo) {
                break;
            }
        }
    }
    if (loadedModel_ == data->indexParent()) {
        result += "<hr/>";
    }
    return result;
}

QString ContentView::renderFunction(ModelPtr data) const
{
    QString result;
    result += "<span class='code'>" + renderChilds(data) + "</span>";
    wrapInlineElement(data, result, true, data->parent() != DocBookModel::FuncDef);
    return result;
}

QString ContentView::renderParameter(ModelPtr data) const
{
    QString result;
    result += "<span class='code'><i>" + renderChilds(data) + "</i></span>";
    wrapInlineElement(data, result, true, data->parent() != DocBookModel::ParamDef);
    return result;
}

QString ContentView::renderType(ModelPtr data) const
{
    QString result;
    result += "<span class='code'><b>" + renderChilds(data) + "</b></span>";
    wrapInlineElement(data, result, true, true);
    if (!result.endsWith(" ") && data->parent() == DocBookModel::FuncDef)
        result += " ";
    return result;
}

QString ContentView::renderFuncSynopsysInfo(ModelPtr data) const
{
    QString result;
    return result;
}

QString ContentView::renderFuncPrototype(ModelPtr data) const
{
    QString result;
    ModelPtr funcdef;
    QList<ModelPtr> paramdefs;
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::FuncDef)
            funcdef = child;
        else if (child == DocBookModel::ParamDef)
            paramdefs.push_back(child);
    }
    result += "<pre class='code'>";
    if (funcdef)
        result += renderFuncDef(funcdef);

    const QString lang = data->role().toLower().trimmed();
    bool requireBraces = lang=="c" || lang=="c++" || lang=="python";

    if (paramdefs.size() > 0 || requireBraces)
        result += "(";

    foreach (ModelPtr paramdef, paramdefs) {
        if (paramdefs.indexOf(paramdef) > 0)
            result += ",&nbsp;";
        result += renderParamDef(paramdef);
    }

    if (paramdefs.size() > 0 || requireBraces)
        result += ")";

    result += "</pre>";
    return result;
}

QString ContentView::renderFuncDef(ModelPtr data) const
{
    QString result;
    QString lang = data->role();
    ModelPtr parent = data->parent();
    while (parent && lang.length() == 0) {
        lang = parent->role();
        parent = parent->parent();
    }
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::Text)
            result += programTextForLanguage(child->text(), lang);
        else
            result += renderElement(child);
    }
    return result;
}

QString ContentView::renderParamDef(ModelPtr data) const
{
    QString result;
    QString lang = data->role();
    ModelPtr parent = data->parent();
    while (parent && lang.length() == 0) {
        lang = parent->role();
        parent = parent->parent();
    }
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::Text)
            result += programTextForLanguage(child->text(), lang);
        else
            result += renderElement(child);
    }
    return result;
}

QString ContentView::renderEmphasis(ModelPtr data) const
{
    const QString tag = data->role()=="bold" ? "b" : "i";
    QString result = "<" + tag + ">";
    result += renderChilds(data);
    result += "</" + tag + ">";
    return wrapInlineElement(data, result, true, true);
}

QString ContentView::renderSubscript(ModelPtr data) const
{
    QString result = "<sub>";
    result += renderChilds(data);
    result += "</sub>";
    return wrapInlineElement(data, result, false, true);
}

QString ContentView::renderSuperscript(ModelPtr data) const
{
    QString result = "<sup>";
    result += renderChilds(data);
    result += "</sup>";
    return wrapInlineElement(data, result, false, true);
}



QString& ContentView::wrapInlineElement(ModelPtr data, QString & result,
                                        bool processLeft, bool processRight)
{
    ModelPtr parent = data->parent();
    if (parent) {
        int index = parent->children().indexOf(data);
        ModelPtr left, right;
        if (processLeft && index > 0) {
            left = parent->children()[index-1];
        }
        if (processRight && index < parent->children().size() - 1) {
            right = parent->children()[index+1];
        }
        if (left == DocBookModel::Text && left->text().length() > 0) {
            const QChar achar = left->text()[left->text().length()-1];
            if (achar != '(' && achar != '[' && achar != '"' && achar != '\'')
                result = " " + result;
        }
        if (right == DocBookModel::Text && right->text().length() > 0) {
            const QChar achar = right->text()[0];
            if (!achar.isPunct() || achar == '(' || achar =='[' || achar == '-')
                result = result + " ";
        }
    }
    return result;
}

QString ContentView::renderItemizedList(ModelPtr data) const
{
    QString result = "<ul>\n";
    result += renderChilds(data);
    result += "</ul>";
    return result;
}


QString ContentView::renderOrderedList(ModelPtr data) const
{
    QString result = "<ol>\n";
    result += renderChilds(data);
    result += "</ol>";
    return result;
}

QString ContentView::renderListItem(ModelPtr data) const
{
    QString result = "<li>\n";
    result += renderChilds(data);
    result += "</li>";
    return result;
}

QString ContentView::renderParagraph(ModelPtr data) const
{
    QString result = "<p align='justify'>";
    result += renderChilds(data);
    result += "</p>";
    return result;
}

QString ContentView::renderMediaObject(ModelPtr data) const
{
    QString result;
    ModelPtr mediaObject = findImageData(data);
    ModelPtr caption;
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::Caption) {
            caption = child;
            break;
        }
    }
    if (mediaObject) {
        result += "<div align='center' width='100%' padding='10'>" +
                renderElement(mediaObject);
        if (caption) {
            result += renderCaption(caption);
        }
        result += "</div>\n";
    }
    return result;
}

QString ContentView::renderCaption(ModelPtr data) const
{
    QString result;
    result += "<div align='center' width='100%'>";
    result += renderChilds(data);
    result += "</div>";
    return result;
}

QString ContentView::renderInlineMediaObject(ModelPtr data) const
{
    QString result;
    ModelPtr mediaObject = findImageData(data);
    if (mediaObject) {
        result += renderElement(mediaObject);
    }
    return result;
}

QString ContentView::renderImageObject(ModelPtr data) const
{
    QString result;
    ModelPtr imageData;
    foreach (ModelPtr child, data->children()) {
        if (child->modelType() == DocBookModel::ImageData) {
            imageData = child;
            break;
        }
    }
    result += "<img src='model_ptr:"+modelToLink(imageData)+"'>";
    return result;
}

QString ContentView::renderMathML(ModelPtr data) const
{
    QString result;
    QList<ModelPtr> rows;
    foreach (ModelPtr child, data->children()) {
        if (child == DocBookModel::MathML_MRow) {
            rows << child;
        }
    }
    if (rows.size() > 1) {
        foreach (ModelPtr row, rows) {
            result += "<p align='center'>";
            result += "<img src='model_ptr:"+modelToLink(row)+"'>";
            result += "</p>";
        }
    }
    else if (rows.size() == 1) {
        result += "<img src='model_ptr:"+modelToLink(rows[0])+"'>";
        wrapInlineElement(data, result, true, true);
    }
    return result;
}

QString ContentView::renderListOfExamples(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    result += renderChilds(data);
    return result;
}

QString ContentView::renderListOfTables(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    result += renderChilds(data);
    return result;
}

QString ContentView::renderListOfFunctions(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    result += renderChilds(data);
    return result;
}

QVariant ContentView::loadResource(int type, const QUrl &name)
{
    QVariant result;
    bool ignore = true;
    if (type == QTextDocument::ImageResource) {
        const QString link = name.toString();
        if (link.startsWith("model_ptr:")) {
            ignore = false;
            QByteArray linkPtr = QByteArray::fromHex(link.toAscii().mid(10));
            QDataStream ds(linkPtr);
            quintptr rawPointer = 0;
            ds >> rawPointer;
            if (rawPointer) {
                DocBookModel * model =
                        reinterpret_cast<DocBookModel*>(rawPointer);
                if (model->modelType() == DocBookModel::ImageData) {
                    const QImage & image = model->imageData();
                    result = image;
                }
                else if (model->modelType() == DocBookModel::MathML_MRow) {
                    MathMLRenderer::self()->render(model->self());
                    const QImage & image = model->imageData();
                    result = image;
                }
            }
        }
    }
    if (ignore) {
        return QTextBrowser::loadResource(type, name);
    }
    else {
        return result;
    }
}

QString ContentView::normalizeText(QString textData) const
{
    static QMap<QString,QString> replacements;
    if (replacements.empty()) {
        replacements["--"] = "&ndash;";
        replacements["---"] = "&mdash;";
    }
    foreach (const QString & key, replacements.keys())
    {
        textData.replace(key, replacements[key]);
    }
    bool inQuote = false;
    for (int i=0; i<textData.length(); i++) {
        if (textData[i] == '"') {
            if (!inQuote) {
                textData.replace(i, 1, "&lsaquo;&lsaquo;");
                inQuote = true;
            }
            else {
                textData.replace(i, 1, "&rsaquo;&rsaquo;");
                inQuote = false;
            }
        }
    }
    return textData;
}

QString ContentView::renderText(ModelPtr data) const
{
    ModelPtr parent = data->parent();
    bool isPreformat = false;
    while (parent) {
        if (parent->modelType() == DocBookModel::ProgramListing
                ||
                parent->modelType() == DocBookModel::Code
                ) {
            isPreformat = true;
            break;
        }
        parent = parent->parent();
    }
    return isPreformat? data->text() : normalizeText(data->text());
}

QString ContentView::renderSection(ModelPtr data) const
{
    QString result;
    const qint8 thisSectionLevel =
            data->sectionLevel() -
            onePageParentModel(data)->sectionLevel();
    const QString tag = QString::fromAscii("h%1").arg(thisSectionLevel + 1);
    const QString number = sectionNumber(data);
    const QString title = number + "&nbsp;" + data->title();
    QString style;
    if (tag == "h2") {
        style = "align='center'";
    }
    else if (tag == "h3" && !number.endsWith("1.")) {
        result += "<p>&nbsp;</p>";
    }
    const QString anchor = modelToLink(data);
    result += "<a name='" + anchor + "'></a><" + tag + " " + style +
            " class=\"title\">" +
            normalizeText(title) +
            "</" + tag + ">\n";
    if (data->subtitle().length() > 0) {
        result += "<" + tag + " class=\"subtitle\">" +
                normalizeText(data->subtitle()) +
                "</"  + tag + ">\n";
    }
    result += renderChilds(data);
    return result;
}

QString ContentView::modelToLink(ModelPtr data) const
{
    const quintptr ptr = quintptr(data.toWeakRef().data());
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << ptr;
    return QString::fromAscii(buffer.toHex());
}

QString ContentView::renderXref(ModelPtr data) const
{
    QString result;
    const QString & linkEnd = data->xrefLinkEnd();
    const QString & endTerm = data->xrefEndTerm();
    ModelPtr target = findModelById(topLevelModel(data), linkEnd);
    ModelPtr term;
    if (endTerm.length() > 0) {
        term = findModelById(topLevelModel(data), linkEnd);
    }
    if (target) {
        QString href;
        if (hasModelOnThisPage(target)) {
            href = "#" + linkEnd;
        }
        else {
            ModelPtr container = findModelById(
                        topLevelModel(data), linkEnd
                        );
            if (container) {
                href = QString::fromAscii("model_ptr:") +
                        modelToLink(container);
            }
        }
        const QString targetTitle = normalizeText(
                    term ? term->title() : target->title()
                    );
        if (href.length() > 0) {
            result += tr("(see&nbsp;<a href=\"%1\">%2</a>)")
                    .arg(href)
                    .arg(targetTitle);
        }
    }
    return wrapInlineElement(data, result, true, true);
}

ModelPtr ContentView::findModelById(
        ModelPtr top,
        const QString & modelId
        ) const
{
    if (!top) {
        return ModelPtr();
    }
    else if (top->id() == modelId) {
        return top;
    }
    else {
        foreach (ModelPtr child, top->children()) {
            ModelPtr result = findModelById(child, modelId);
            if (result) {
                return result;
            }
        }
    }
    return ModelPtr();
}

ModelPtr ContentView::topLevelModel(ModelPtr data) const
{
    if (data->parent().isNull()) {
        return data;
    }
    else {
        return topLevelModel(data->parent());
    }
}

ModelPtr ContentView::onePageParentModel(ModelPtr data) const
{
    if (data->parent().isNull() || data->modelType() == DocBookModel::Chapter ||
            data->modelType() == DocBookModel::Article ||
            data->modelType() == DocBookModel::ListOfExamples ||
            data->modelType() == DocBookModel::ListOfTables ||
            data->modelType() == DocBookModel::ListOfFunctions ||
            data->modelType() == DocBookModel::Book)
    {
        return data;
    }
    else if (data->indexParent()) {
        return data->indexParent();
    }
    else if (data->parent()->modelType() == DocBookModel::Chapter ||
             data->parent()->modelType() == DocBookModel::Article ||
             data->parent()->modelType() == DocBookModel::Book)
    {
        return data->parent();
    }
    else {
        return onePageParentModel(data->parent());
    }
}

bool ContentView::hasModelOnThisPage(ModelPtr data) const
{
    return loadedModel_ && hasChild(loadedModel_, data);
}

bool ContentView::hasChild(ModelPtr who, ModelPtr childToFind) const
{
    foreach (ModelPtr child, who->children()) {
        if (childToFind == child || hasChild(child, childToFind)) {
            return true;
        }
    }
    return false;
}

ModelPtr ContentView::findImageData(ModelPtr parent) const
{
    ModelPtr svgChild;
    ModelPtr pngChild;
    foreach (ModelPtr child, parent->children()) {
        if (child->modelType()==DocBookModel::ImageObject) {
            foreach (ModelPtr childChild, child->children()) {
                if (childChild->modelType()==DocBookModel::ImageData) {
                    if (childChild->format() == "svg") {
                        svgChild = child;
                    }
                    else if (childChild->format() == "png") {
                        pngChild = child;
                    }
                }
            }
        }
    }
    return pngChild ? pngChild : svgChild;
}


QString ContentView::renderTOC(ModelPtr data) const
{
    QString result;
    QString title;
    if (data == DocBookModel::ListOfExamples) {
        title = tr("List of examples in \"%1\"").arg(data->title());
    }
    else if (data == DocBookModel::ListOfTables) {
        title = tr("List of tables in \"%1\"").arg(data->title());
    }
    else if (data == DocBookModel::ListOfFunctions) {
        if (data->title().isEmpty())
            title = tr("List of Standard Library algorithms");
        else
            title = tr("List of algorithms of module \"%1\"").arg(data->title());
    }
    else if (data == DocBookModel::Book || data == DocBookModel::Article) {
        title = data->title();
    }
    else {
        title = sectionNumber(data) + "&nbsp;" + data->title();
    }
    result += "<h1 class='title' align='center'>" + normalizeText(title) + "</h1>\n";
    if (data->subtitle().length() > 0) {
        result += "<p class='subtitle' align='center'>" +
                normalizeText(data->subtitle()) +
                "</p>\n";
    }
    result += "<hr/>\n";
    foreach (ModelPtr child, data->children()) {
        result += renderTOCElement(child, 0, true);
    }
    result += "<hr/>\n";
    return result;
}

QString ContentView::renderSet(ModelPtr data) const
{
    QString result;
    const QString & title = data->title();
    result += "<h1 class='title' align='center'>" + title + "</h1>\n";
    foreach (ModelPtr child, data->children()) {
        const quintptr dataPtr = quintptr(child.toWeakRef().data());
        QByteArray buffer;
        QDataStream ds(&buffer, QIODevice::WriteOnly);
        ds << dataPtr;
        const QString href = QString::fromAscii("model_ptr:") +
                QString::fromAscii(buffer.toHex());
        result += "<p align=\"left\"><a href=\"" + href +"\">" +
                child->title() + "</a></p>\n";
        result += "<p margin='10' align='left'>" + child->subtitle() + "</p>";
    }
    return result;

}

QString ContentView::renderTOCElement(ModelPtr data, quint8 level, bool enumerate) const
{
    const quintptr dataPtr = quintptr(data.toWeakRef().data());
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << dataPtr;
    const QString href = QString::fromAscii("model_ptr:") +
            QString::fromAscii(buffer.toHex());
    QString result = "\n<li>";
    QString index;
    if (data == DocBookModel::Example || data == DocBookModel::Table) {
        index = chapterNumber(data) > 0
                ? QString("%1.%2")
                  .arg(chapterNumber(data))
                  .arg(elementNumber(data))
                : QString::number(elementNumber(data));
        index = data == DocBookModel::Example
                ? tr("Example&nbsp;%1. ").arg(index)
                : tr("Table&nbsp;%1. ").arg(index);
    }
    else if (data == DocBookModel::FuncSynopsys) {
        QString::number(elementNumber(data));
    }
    else {
        index = sectionNumber(data) + " ";
    }
    QString title = enumerate
            ? index + data->title()
            : data->title();

    QString indent;
    for (quint8 i=0; i<level * 4; i++) {
        indent += "&nbsp;";
    }
    result += "<p align='left' margin='5'><a href=\"" + href + "\">" + indent + title + "</p>";
    if (!isPlainPage(data)) {
        foreach (ModelPtr child, data->children()) {
            result += renderTOCElement(child, level + 1, enumerate);
        }
    }
    return result + "</li>\n";
}

void ContentView::handleInternalLink(const QUrl &url)
{
    if (url.encodedPath().startsWith("model_ptr:")) {
        const QByteArray path = url.encodedPath().mid(10);
        QByteArray data = QByteArray::fromHex(path);
        QDataStream ds(&data, QIODevice::ReadOnly);
        quintptr ptr = 0u;
        ds >> ptr;
        emit itemRequest(findModelByRawPtr(ptr));
    }
}

ModelPtr ContentView::findModelByRawPtr(quintptr raw) const
{
    ModelPtr result;
    ModelPtr topLevel = topLevelModel(loadedModel_);
    if (topLevel) {
        result = findModelByRawPtr(topLevel, raw);
    }
    if (!result) {
        foreach (ModelPtr child, topLevel->children()) {
            ModelPtr newTopLevel = topLevelModel(child);
            result = findModelByRawPtr(newTopLevel, raw);
            if (result) {
                break;
            }
        }
    }
    return result;
}

ModelPtr ContentView::findModelByRawPtr(ModelPtr root, quintptr raw) const
{
    ModelPtr result;
    if (root.data() == reinterpret_cast<DocBookModel*>(raw)) {
        result = root;
    }
    else foreach (ModelPtr child, root->children()) {
        result = findModelByRawPtr(child, raw);
        if (result)
            break;
    }
    return result;
}

static QString screenRegexSymbols(QString s)
{
    s.replace("|", "\\|");
    s.replace("*", "\\*");
    s.replace("+", "\\+");
    s.replace("{", "\\{");
    s.replace("}", "\\}");
    s.replace("[", "\\[");
    s.replace("]", "\\]");
    return s;
}

QString ContentView::formatProgramSourceText(
        const QString &source,
        const QStringList &keywords,
        const QString &inlineCommentSymbol,
        const QString &multilineCommentStartSymbol,
        const QString &multilineCommentEndSymbol)
{
    QStringList kwds;
    QString result;
    if (keywords.isEmpty()) {
        return source;
    }
    static const QString kwdOpenTag = "<b>";
    static const QString kwdCloseTag = "</b>";
    static const QString beforeCommentTag = "<font color='gray'>";
    static const QString afterCommentTag = "</font>";
    static const QString commentOpenTag = "<i>";
    static const QString commentCloseTag = "</i>";

    foreach (const QString & keyword, keywords) {
        kwds << "\\b" + keyword + "\\b";
    }
    if (inlineCommentSymbol.length() > 0) {
        kwds << screenRegexSymbols(inlineCommentSymbol);
        kwds << "\n";
    }
    if (multilineCommentStartSymbol.length() > 0
            && multilineCommentEndSymbol.length() > 0)
    {
        kwds << screenRegexSymbols(multilineCommentStartSymbol);
        kwds << screenRegexSymbols(multilineCommentEndSymbol);
    }
    QRegExp rxLexer(kwds.join("|"));
    rxLexer.setMinimal(true);
    bool inlineComment = false;
    bool multilineComment = false;
    for (int p = 0, c = 0; ;  ) {
        c = rxLexer.indexIn(source, p);
        if (c == -1) {
            result += source.mid(p);
            break;
        }
        else {
            if (c > p) {
                result += source.mid(p, c - p);
            }
            const QString cap = rxLexer.cap();
            if (cap == inlineCommentSymbol) {
                inlineComment = true;
                result += beforeCommentTag;
                result += cap;
                result += commentOpenTag;
            }
            else if (inlineComment && cap=="\n") {
                inlineComment = false;
                result += commentCloseTag;
                result += afterCommentTag;
                result += "\n";
            }
            else if (cap == multilineCommentStartSymbol) {
                multilineComment = true;
                result += beforeCommentTag;
                result += cap;
                result += commentOpenTag;
            }
            else if (multilineComment && cap==multilineCommentEndSymbol) {
                multilineComment = false;
                result += commentCloseTag;
                result += cap;
                result += afterCommentTag;
            }
            else if (keywords.contains(cap)) {
                result += kwdOpenTag + cap + kwdCloseTag;
            }
            else {
                result += cap;
            }
            p = c + rxLexer.matchedLength();
        }
    }
    if (inlineComment) {
        result += commentCloseTag;
        result += afterCommentTag;
    }

    return result;
}

void ContentView::resizeEvent(QResizeEvent *e)
{
    ignoreClearAnchorUrl_ = true;
    QTextBrowser::resizeEvent(e);;
    if (lastAnchorUrl_.isValid()) {
        setSource(lastAnchorUrl_);
    }
}

void ContentView::wheelEvent(QWheelEvent *e)
{
    QTextBrowser::wheelEvent(e);
    if (e->buttons() == Qt::NoButton) {
        clearLastAnchorUrl();
    }
}

void ContentView::clearLastAnchorUrl()
{
    if (!ignoreClearAnchorUrl_) {
        lastAnchorUrl_.clear();
//        qDebug() << "Claar last anchor url";
    }
    else {
//        qDebug() << "Ignored clear last anchor url";
    }
    ignoreClearAnchorUrl_ = false;
}

}
