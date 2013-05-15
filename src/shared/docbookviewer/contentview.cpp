#include "contentview.h"
#include "docbookmodel.h"

#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

ContentView::ContentView(QWidget *parent)
    : QTextBrowser(parent)
    , currentSectionLevel_(0u)
{
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(handleInternalLink(QUrl)));
}

void ContentView::reset()
{
    loadedModels_.clear();
    clear();
    currentSectionLevel_ = 0u;
    counters_.example = counters_.figure = counters_.table = 0u;
}

void ContentView::addData(const DocBookModel *data)
{
    if (loadedModels_.size() == 0) {
        currentSectionLevel_ = data->sectionLevel();
    }
    loadedModels_.push_back(data);
    const QString html = wrapHTML(render(data));
    setHtml(html);
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

QString ContentView::render(const DocBookModel *data) const
{
    return isPlainPage(data) ? renderPlainPage(data) : renderTOC(data);
}

bool ContentView::isPlainPage(const DocBookModel *data) const
{
    quint32 sectionedChilds = 0;
    foreach (const DocBookModel * child, data->children()) {
        if (child->isSectioningNode())
            sectionedChilds += 1;
    }
    return sectionedChilds == 0;
}

QString ContentView::renderPlainPage(const DocBookModel *data) const
{
    return renderElement(data);
}

QString ContentView::renderChilds(const DocBookModel *data) const
{
    QString result;
    foreach (const DocBookModel * child, data->children()) {
        result += renderElement(child) + "\n";
    }
    return result;
}

QString ContentView::renderElement(const DocBookModel *data) const
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

QString ContentView::renderKeyCombo(const DocBookModel *data) const
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

QString ContentView::renderKeySym(const DocBookModel *data) const
{
    QString result;
    QString keysym;
    foreach (const DocBookModel * child, data->children()) {
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

QString ContentView::renderProgramListing(const DocBookModel *data) const
{
    QString result = "<table width='100%' border='1'><tr><td>";
    result += "<pre align='left'>" + renderChilds(data) + "</pre>\n";
    result += "</td></tr></table>\n";
    return result;
}

QString ContentView::renderExample(const DocBookModel *data) const
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

QString ContentView::renderEmphasis(const DocBookModel *data) const
{
    const QString tag = data->role()=="bold" ? "b" : "i";
    QString result = "<" + tag + ">";
    result += renderChilds(data);
    result += "</" + tag + ">";
    return result;
}

QString ContentView::renderItemizedList(const DocBookModel *data) const
{
    QString result = "<ul>\n";
    result += renderChilds(data);
    result += "</ul>";
    return result;
}


QString ContentView::renderOrderedList(const DocBookModel *data) const
{
    QString result = "<ol>\n";
    result += renderChilds(data);
    result += "</ol>";
    return result;
}

QString ContentView::renderListItem(const DocBookModel *data) const
{
    QString result = "<li>\n";
    result += renderChilds(data);
    result += "</li>";
    return result;
}

QString ContentView::renderParagraph(const DocBookModel *data) const
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

QString ContentView::renderText(const DocBookModel *data) const
{
    const DocBookModel* parent = data->parent();
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

QString ContentView::renderSection(const DocBookModel *data) const
{
    const qint8 thisSectionLevel = data->sectionLevel() - currentSectionLevel_;
    const QString tag = QString::fromAscii("h%1").arg(thisSectionLevel + 1);
    QString result = "<" + tag + +" class=\"title\">" +
            normalizeText(data->title()) +
            "</" + tag + ">\n";
    if (data->subtitle().length() > 0) {
        result += "<" + tag + " class=\"subtitle\">" +
                normalizeText(data->subtitle()) +
                "</"  + tag + ">\n";
    }
    result += renderChilds(data);
    return result;
}

QString ContentView::renderXref(const DocBookModel *data) const
{
    QString result;
    const QString & linkEnd = data->xrefLinkEnd();
    const QString & endTerm = data->xrefEndTerm();
    const DocBookModel* target = findModelById(topLevelModel(data), linkEnd);
    if (target) {
        QString href;
        if (hasModelOnThisPage(target)) {
            href = "#" + linkEnd;
        }
        else {
            const DocBookModel * container = findModelById(
                        topLevelModel(data), linkEnd
                        );
            if (container) {
                const quintptr ptr = quintptr(container);
                QByteArray buffer;
                QDataStream ds(&buffer, QIODevice::WriteOnly);
                ds << ptr;
                href = QString::fromAscii("model_ptr:") +
                        QString::fromAscii(buffer.toHex());
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

const DocBookModel* ContentView::findModelById(
        const DocBookModel * top,
        const QString & modelId
        ) const
{
    if (!top) {
        return nullptr;
    }
    else if (top->id() == modelId) {
        return top;
    }
    else {
        foreach (const DocBookModel * child, top->children()) {
            const DocBookModel * result = findModelById(child, modelId);
            if (result) {
                return result;
            }
        }
    }
    return nullptr;
}

const DocBookModel* ContentView::topLevelModel(const DocBookModel * data) const
{
    if (data->parent() == nullptr) {
        return data;
    }
    else {
        return topLevelModel(data->parent());
    }
}

bool ContentView::hasModelOnThisPage(const DocBookModel *data) const
{
    foreach (const DocBookModel* item, loadedModels_) {
        if (data == item || hasChild(item, data)) {
            return true;
        }
    }
    return false;
}

bool ContentView::hasChild(
        const DocBookModel *who,
        const DocBookModel *childToFind) const
{
    foreach (const DocBookModel* child, who->children()) {
        if (childToFind == child || hasChild(child, childToFind)) {
            return true;
        }
    }
    return false;
}



QString ContentView::renderTOC(const DocBookModel *data) const
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
    foreach (const DocBookModel * child, data->children()) {
        result += renderTOCElement(child);
    }
    result += "</ol>\n";
    result += "<hr/>\n";
    return result;
}

QString ContentView::renderTOCElement(const DocBookModel *data) const
{
    const quintptr dataPtr = quintptr(data);
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << dataPtr;
    const QString href = QString::fromAscii("model_ptr:") +
            QString::fromAscii(buffer.toHex());

    QString result = "\n<li>";
    result += "<p><a href=\"" + href + "\">" + data->title() + "</p>";
    if (!isPlainPage(data)) {
        result += "\n<ol>\n";
        foreach (const DocBookModel * child, data->children()) {
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
