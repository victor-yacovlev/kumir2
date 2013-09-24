#include "generator.h"
#include "preambule.tex.h"

#include "docbookviewer/docbookmodel.h"

#include <QTextCodec>
#include <QByteArray>
#include <QDir>
#include <QMap>

namespace docbook2latex {

using namespace std;
using namespace DocBookViewer;

generator::generator()
    : addSpace_(false)
{
}


generator * generator::self()
{
    static generator * instance = new generator();
    return instance;
}

void generator::saveToFile(const std::string &filename) const
{
    const QString qFileName = QString::fromStdString(filename);
    QFile f(qFileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextCodec * koi8 = QTextCodec::codecForName("KOI8-R");
        f.write(koi8->fromUnicode(data_));
        f.close();
    }
    else {
        throw string("Can't open file for write: ") + filename;
    }
}

bool generator::isEmpty() const
{
    return ! bool(root_);
}

void generator::reset()
{
    root_.clear();
    data_.clear();

}


void generator::renderData(ModelPtr data)
{
    root_ = data;
    QString preambule = QString::fromUtf8(PREAMBULE);
    QString paper = QString::fromStdString(options::page_size).toLower() + "paper";
    preambule.replace("%%paper%%", paper);
    preambule.replace("%%class%%", root_ == DocBookModel::Book ? "book" : "article");
    data_ = preambule + "\n";
    data_ += "\\begin{document}\n\n";
    data_ += DOCUMENT_BEGIN;
    if (root_ == DocBookModel::Book) {
        data_ += renderBookTitle();
    }
    Q_FOREACH(ModelPtr child, data->children()) {
        data_ += renderElement(child);
    }
    data_.replace("--\\", "-- \\");
    data_.replace(":\\", ": \\");
    data_.replace("}(", "} (");
    data_ += "\n\\end{document}\n";
}

QString generator::renderElement(ModelPtr data)
{
    QString result;
    if (data == DocBookModel::Chapter) {
        result = renderChapter(data);
    }
    else if (data == DocBookModel::Section) {
        result = renderSection(data);
    }
    else if (data == DocBookModel::Para) {
        result = renderPara(data);
    }
    else if (data == DocBookModel::Emphasis) {
        result = renderEmphasis(data);
    }
    else if (data == DocBookModel::Text) {
        if (data->parent() == DocBookModel::ProgramListing ||
                data->parent() == DocBookModel::Code)
        {
            result = data->text().trimmed();
            static QMap<QString,QString> replacements;
            if (replacements.empty()) {
                replacements["~"] = "\\~";
                replacements["&lt;"] = "<";
                replacements["&gt;"] = ">";
//                replacements["_"] = "\\_";
                replacements[QString(QChar(0x2264))] = "$<=$";
                replacements[QString(QChar(0x2265))] = "$>=$";
            }
            foreach (const QString & key, replacements.keys())
            {
                result.replace(key, replacements[key]);
            }
            addSpace_ = data->parent() == DocBookModel::Code;
        }
        else {
            QString normText = normalizeText(data->text());
            if (addSpace_ && normText.length() > 0 &&
                    ( normText[0].isLetterOrNumber() || normText[0] == '-' )
                    )
                result += " ";
            result += normText;
            addSpace_ = result.length() > 0 &&
                    result.at(result.length()-1).isLetterOrNumber();
        }
    }
    else if (data == DocBookModel::OrderedList) {
        result = renderOrderedList(data);
    }
    else if (data == DocBookModel::ItemizedList) {
        result = renderItemizedList(data);
    }
    else if (data == DocBookModel::ListItem) {
        result = renderListItem(data);
    }
    else if (data == DocBookModel::ProgramListing) {
        result = renderProgramListing(data);
    }
    else if (data == DocBookModel::Code) {
        result = renderCode(data);
    }
    else if (data == DocBookModel::Example) {
        result = renderExample(data);
    }
    return result;
}

QString generator::renderChapter(ModelPtr data)
{
    QString result;
    result = QString::fromAscii("\\chapter{%1}\n").arg(normalizeText(data->title()));
    result += "\\thispagestyle{fancy}\n";
    result += "\\setcounter{example}{1}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\vfill\n";
    return result;
}


QString generator::renderSection(ModelPtr data)
{
    QString result;
    QString sec =
            (root_ == DocBookModel::Article && data->sectionLevel() == 0) ||
            (root_ == DocBookModel::Book && data->sectionLevel() == 1)
            ? "section" : "subsection";
    result = QString::fromAscii("\\%1{%2}\n\n").arg(sec).arg(normalizeText(data->title()));
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}


QString generator::renderPara(ModelPtr data)
{
    QString result = "\n\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\n\n";
    return result;
}

QString generator::normalizeText(QString textData) const
{
    static QMap<QString,QString> replacements;
    if (replacements.empty()) {
        replacements["~"] = "\\~";
        replacements["_"] = "\\_";
        replacements[QString(QChar(0x2264))] = "$<=$";
        replacements[QString(QChar(0x2265))] = "$>=$";
    }
    foreach (const QString & key, replacements.keys())
    {
        textData.replace(key, replacements[key]);
    }
    bool inQuote = false;
    for (int i=0; i<textData.length(); i++) {
        if (textData[i] == '"') {
            if (!inQuote) {
                textData.replace(i, 1, "<<");
                inQuote = true;
            }
            else {
                textData.replace(i, 1, ">>");
                inQuote = false;
            }
        }
    }
    return textData.simplified();
}

QString generator::renderProgramListing(ModelPtr data)
{
    QString result;
    result +=
            "\\lstset{"
            "   aboveskip=0pt,"
            "   belowskip=0pt";
    if (data->role().length() > 0) {
        result += ",language=" + data->role();
    }
    result += "}\n";
    result += "\\begin{lstlisting}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{lstlisting}\n";
    return result;
}

QString generator::renderCode(ModelPtr data)
{
    QString result;

    result += " ";

    result +=
            "\\lstset{"
            "   aboveskip=0pt,"
            "   belowskip=0pt";
    if (data->role().length() > 0) {
        result += ",language=" + data->role();
    }
    result += "}";
    result += "\\lstinline{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderExample(ModelPtr data)
{
    QString result;
    result += QString::fromAscii("\\begin{example}{%1}\n").arg(normalizeText(data->title()));
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{example}\n";
//    result += QString::fromAscii("\\begin{table}\n");
//    result += "\\centering\n";
//    Q_FOREACH(ModelPtr child, data->children()) {
//        result += renderElement(child);
//    }
//    result += QString::fromAscii("\\caption{\\textbf{Пример.} %1}\n").arg(normalizeText(data->title()));
//    result += "\\end{table}\n";
    return result;
}

QString generator::renderEmphasis(ModelPtr data)
{
    QString result;
    if (data->role() == "bold")
        result += "\\textbf{";
    else
        result += "\\textit{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderBookTitle()
{
    QString result;
    result += QString::fromAscii("\\title{%1}\n").arg(root_->title());
    result += "\\maketitle\n";
    return result;
}

QString generator::renderOrderedList(ModelPtr data)
{
    QString result;
    result += "\\begin{enumerate}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{enumerate}\n";
    return result;
}

QString generator::renderItemizedList(ModelPtr data)
{
    QString result;
    result += "\\begin{itemize}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{itemize}\n";
    return result;
}

QString generator::renderListItem(ModelPtr data)
{
    QString result;
    result += "\\item ";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\n";
    return result;
}


} // namespace docbook2pdf
