#include "contentview.h"
#include "docbookmodel.h"

#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

ContentView::ContentView(QWidget *parent)
    : QTextBrowser(parent)
{
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(handleInternalLink(QUrl)));
}

void ContentView::reset()
{
    loadedModels_.clear();
    clear();
    counters_.example = counters_.figure = counters_.table = 0u;
}

void ContentView::addData(ModelPtr data)
{
    ModelPtr dataToRender = onePageParentModel(data);
    loadedModels_.push_back(data);
    const QString html = wrapHTML(render(dataToRender));
    setHtml(html);
    if (dataToRender != data) {
        QString anchor = modelToLink(data);
        QUrl anchorUrl("#" + anchor);
        setSource(anchorUrl);
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

QString ContentView::wrapHTML(const QString &body) const
{
    return QString() +
            "<html><head>"
            "<style type=\"text/css\">"
            "kbd {"
            "   background-color: lightgray;"
            "}"
            "</style></head>"
            "<body>\n" + body +"\n</body></html>";
}

QString ContentView::render(ModelPtr data) const
{
    if (data->modelType() == DocBookModel::Set ||
            data->modelType() == DocBookModel::Book)
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
        result += renderElement(child) + "\n";
    }
    return result;
}

QString ContentView::renderElement(ModelPtr data) const
{
    if (data->modelType() == DocBookModel::Text) {
        return renderText(data);
    }
    else if (data->modelType() == DocBookModel::Para) {
        return renderParagraph(data);
    }
    else if (data->modelType() == DocBookModel::Emphasis) {
        return renderEmphasis(data);
    }
    else if (data->modelType() == DocBookModel::ListItem) {
        return renderListItem(data);
    }
    else if (data->modelType() == DocBookModel::OrderedList) {
        return renderOrderedList(data);
    }
    else if (data->modelType() == DocBookModel::ItemizedList) {
        return renderItemizedList(data);
    }
    else if (data->modelType() == DocBookModel::Chapter) {
        return renderChapter(data);
    }
    else if (data->modelType() == DocBookModel::Section) {
        return renderSection(data);
    }
    else if (data->modelType() == DocBookModel::Example) {
        return renderExample(data);
    }
    else if (data->modelType() == DocBookModel::ProgramListing) {
        return renderProgramListing(data);
    }
    else if (data->modelType() == DocBookModel::Xref) {
        return renderXref(data);
    }
    else if (data->modelType() == DocBookModel::KeyCombo) {
        return renderKeyCombo(data);
    }
    else if (data->modelType() == DocBookModel::KeySym) {
        return renderKeySym(data);
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

QString ContentView::renderProgramListing(ModelPtr data) const
{
    QString result = "<table width='100%' border='1'><tr><td>";
    result += "<pre align='left'>" + renderChilds(data);
    result = result.trimmed();
    result += "</pre>\n";
    result += "</td></tr></table>\n";
    return result;
}

QString ContentView::renderExample(ModelPtr data) const
{
    QString result;
    const QString & title = data->title();
    counters_.example ++;
    result += "<table width='100%'>\n";
    result += "<tr><td height='10'>&nbsp;</td></tr>\n";
    result += "<tr><td align='center'>\n";
    result += renderChilds(data);
    result += "</td></tr>\n";
    result += "<tr><td align='center'>\n";
    result += "<b>" + tr("Example&nbsp;%1. ").arg(counters_.example) + "</b>";
    result += normalizeText(title);
    result += "</p></td></tr>\n";
    result += "</table>\n";
    return result;
}

QString ContentView::renderEmphasis(ModelPtr data) const
{
    const QString tag = data->role()=="bold" ? "b" : "i";
    QString result = "<" + tag + ">";
    result += renderChilds(data);
    result += "</" + tag + ">";
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
    QString result = "<p>";
    result += renderChilds(data);
    result += "</p>";
    return result;
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
        if (parent->modelType() == DocBookModel::ProgramListing) {
            isPreformat = true;
            break;
        }
        parent = parent->parent();
    }
    return isPreformat? data->text() : normalizeText(data->text());
}

QString ContentView::renderSection(ModelPtr data) const
{
    const qint8 thisSectionLevel =
            data->sectionLevel() -
            onePageParentModel(data)->sectionLevel();
    const QString tag = QString::fromAscii("h%1").arg(thisSectionLevel + 1);
    const QString anchor = data->id().length() > 0
            ? data->id()
            : modelToLink(data);
    QString result = "<a name='" + anchor + "'><" + tag + +" class=\"title\">" +
            normalizeText(data->title()) +
            "</" + tag + "></a>\n";
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
        const QString targetTitle = normalizeText(target->title());
        if (href.length() > 0) {
            result += tr("(see&nbsp;<a href=\"%1\">%2</a>)")
                    .arg(href)
                    .arg(targetTitle);
        }
    }
    return result;
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
            data->modelType() == DocBookModel::Book)
    {
        return data;
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
    foreach (ModelPtr item, loadedModels_) {
        if (data == item || hasChild(item, data)) {
            return true;
        }
    }
    return false;
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



QString ContentView::renderTOC(ModelPtr data) const
{
    QString result;
    result += "<h1 class=\"title\">" + normalizeText(data->title()) + "</h1>\n";
    if (data->subtitle().length() > 0) {
        result += "<h1 class=\"subtitle\">" +
                normalizeText(data->subtitle()) +
                "</h1>\n";
    }
    result += "<hr/>\n";
    result += "<ol>\n";
    foreach (ModelPtr child, data->children()) {
        result += renderTOCElement(child);
    }
    result += "</ol>\n";
    result += "<hr/>\n";
    return result;
}

QString ContentView::renderTOCElement(ModelPtr data) const
{
    const quintptr dataPtr = quintptr(data.toWeakRef().data());
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << dataPtr;
    const QString href = QString::fromAscii("model_ptr:") +
            QString::fromAscii(buffer.toHex());

    QString result = "\n<li>";
    result += "<p><a href=\"" + href + "\">" + data->title() + "</p>";
    if (!isPlainPage(data)) {
        result += "\n<ol>\n";
        foreach (ModelPtr child, data->children()) {
            result += renderTOCElement(child);
        }
        result += "\n</ol>\n";
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
        emit requestModelLoad(ptr);
    }
}

}
