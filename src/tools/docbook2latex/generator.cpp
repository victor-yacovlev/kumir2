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
    , tableColumns_(0u)
    , twoColumnLayout_(false)
    , allowRasterImages_(false)
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
    QString paper, clazz, clazzparam, lmargin, rmargin;
    QString geometry;
    QString colorlinks = "false";
    const QString profile = QString::fromStdString(options::profile).toLower();
    static const QStringList ValidProfiles = QStringList() << "a4" << "a5" <<
                                                              "a4ts" << "a5ts" <<
                                                              "tablet" << "ebook";
    if (!ValidProfiles.contains(profile)) {
        throw string("Not valid profile: ") + options::profile;
    }
    if (profile.startsWith("a4")) {
        paper = "a4paper";
        geometry = "tmargin=2cm,bmargin=2cm,%%LEFTMARGIN%%=2cm,%%RIGHTMARGIN%%=3cm";
        lmargin = "2cm";
        rmargin = "3cm";
    }
    else if (profile.startsWith("a5")) {
        paper = "a5paper";
        geometry = "tmargin=2cm,bmargin=2cm,%%LEFTMARGIN%%=2cm,%%RIGHTMARGIN%%=3cm";
        lmargin = "2cm";
        rmargin = "3cm";
    }
    else if (profile.startsWith("e") && profile.endsWith("book")) {
        paper = "a6paper";
        geometry = "papersize={110mm,142mm},total={110mm,142mm}";
    }
    else {
        paper = "a5paper";
        geometry = "tmargin=0.1cm,bmargin=0.1cm,%%LEFTMARGIN%%=0.5cm,%%RIGHTMARGIN%%=0.5cm";
        allowRasterImages_ = true;
        colorlinks = "true";
    }
    clazzparam = paper;
    if (profile.endsWith("ts")) {
        clazzparam += ",twoside";
        geometry = geometry.replace("%%LEFTMARGIN%%", "imargin");
        geometry = geometry.replace("%%RIGHTMARGIN%%", "omargin");
    }
    else {
        clazzparam += ",oneside";
        geometry = geometry.replace("%%LEFTMARGIN%%", "lmargin");
        geometry = geometry.replace("%%RIGHTMARGIN%%", "rmargin");
    }
    clazz = root_ == Book ? "book" : "article";

    preambule.replace("%%paper%%", paper);
    preambule.replace("%%class%%", clazz);
    preambule.replace("%%classparam%%", clazzparam);
    preambule.replace("%%geometry%%", geometry);
    preambule.replace("%%colorlinks%%", colorlinks);
    data_ = preambule + "\n";
    data_ += "\\begin{document}\n\n";
    if (clazz == "article" && paper == "a4paper") {
//        twoColumnLayout_ = true;
    }
    data_ += DOCUMENT_BEGIN;
    if (root_ == Book) {
        data_ += renderBookTitle();
    }
    else {
        data_ += renderArticleTitle();
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
    if (data == Chapter) {
        result = renderChapter(data);
    }
    else if (data == Section) {
        result = renderSection(data);
    }
    else if (data == Para) {
        result = renderPara(data);
    }
    else if (data == Emphasis) {
        result = renderEmphasis(data);
    }
    else if (data == Text) {
        if (data->parent() == ProgramListing ||
                data->parent() == Code)
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
            addSpace_ = data->parent() == Code;
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
    else if (data == OrderedList) {
        result = renderOrderedList(data);
    }
    else if (data == ItemizedList) {
        result = renderItemizedList(data);
    }
    else if (data == ListItem) {
        result = renderListItem(data);
    }
    else if (data == ProgramListing) {
        result = renderProgramListing(data);
    }
    else if (data == Code) {
        result = renderCode(data);
    }
    else if (data == Example) {
        result = renderExample(data);
    }
    else if (data == Table) {
        result = renderTable(data);
    }
    else if (data == Subscript) {
        result = renderSubscript(data);
    }
    else if (data == Superscript) {
        result = renderSuperscript(data);
    }
    else if (data == GuiMenu) {
        result = renderGuiMenu(data);
    }
    else if (data == GuiMenuItem) {
        result = renderGuiMenuItem(data);
    }
    else if (data == GuiButton) {
        result = renderGuiButton(data);
    }
    else if (data == KeyCombo) {
        result = renderKeyCombo(data);
    }
    else if (data == KeySym) {
        result = renderKeySym(data);
    }
    else if (data == MathML_Math) {
        result = renderMath(data);
    }
    else if (data == MathML_MRow) {
        result = renderMRow(data);
    }
    else if (data == MathML_MI) {
        result = renderMI(data);
    }
    else if (data == MathML_MN) {
        result = renderMN(data);
    }
    else if (data == MathML_MO) {
        result = renderMO(data);
    }
    else if (data == MathML_MSup) {
        result = renderMSup(data);
    }
    else if (data == MathML_MSqrt) {
        result = renderMSqrt(data);
    }
    else if (data == MathML_MFrac) {
        result = renderMFrac(data);
    }
    else if (data == FuncSynopsys) {
        result = renderFuncSynopsis(data);
    }
    else if (data == FuncPrototype) {
        result = renderFuncPrototype(data);
    }
    else if (data == FuncDef) {
        result = renderFuncDef(data);
    }
    else if (data == Function) {
        result = renderFunction(data);
    }
    else if (data == ParamDef) {
        result = renderParamDef(data);
    }
    else if (data == Parameter) {
        result = renderParameter(data);
    }
    else if (data == Type) {
        result = renderType(data);
    }
    else if (data == MediaObject) {
        result = renderMediaObject(data);
    }
    return result;
}

QString generator::renderChapter(ModelPtr data)
{
    QString result;
    result = QString::fromLatin1("\\chapter{%1}\n").arg(normalizeText(data->title()));
    result += "\\thispagestyle{fancy}\n";
    result += "\\setcounter{Example}{0}\n";
    result += "\\setcounter{Figure}{0}\n";
    result += "\\setcounter{Table}{0}\n";
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
            (root_ == Article && data->sectionLevel() == 1) ||
            (root_ == Book && data->sectionLevel() == 1)
            ? "section" : "subsection";


    result = QString::fromLatin1("\\%1{%2}\n\n").arg(sec).arg(normalizeText(data->title()));

    if (sec == "section" && twoColumnLayout_) {
        result = QString::fromLatin1("\\begin{multicols}{2}[%1]\n").arg(result.trimmed());
    }

    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }

    if (sec == "section" && twoColumnLayout_) {
        result += QString::fromLatin1("\\end{multicols}\n");
        result += sectionFloats_.join("\n");
        sectionFloats_.clear();
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

QString generator::renderFuncSynopsis(ModelPtr data)
{
    QString result = "\\begin{FuncSynopsis}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{FuncSynopsis}\n";
    return result;
}

QString generator::renderFuncPrototype(ModelPtr data)
{
    QString result;
    ModelPtr funcDef;
    QList<ModelPtr> paramDefs;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == FuncDef)
            funcDef = child;
        else if (child == ParamDef)
            paramDefs.push_back(child);
    }
    if (funcDef) {
        result += renderElement(funcDef);
    }
    if (paramDefs.size() > 0) {
        result += "(";
        addSpace_ = false;
        for (int i=0; i<paramDefs.size(); i++) {
            if (i>0)
                result += ", ";
            result += renderElement(paramDefs[i]);
        }
        result += ")";
    }
    return result;
}

QString generator::renderFuncDef(ModelPtr data)
{
    QString result = "\\textbf{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderParamDef(ModelPtr data)
{
    QString result = "\\textbf{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderFunction(ModelPtr data)
{
    QString result = "\\normalfont~";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString generator::renderParameter(ModelPtr data)
{
    QString result = "\\normalfont\\textit{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderType(ModelPtr data)
{
    QString result = "\\textbf{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderSubscript(ModelPtr data)
{
    QString result = "\\textsubscript{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderSuperscript(ModelPtr data)
{
    QString result = "\\textsuperscript{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderGuiButton(ModelPtr data)
{
    QString result = "\\textsc{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderGuiMenu(ModelPtr data)
{
    return renderGuiMenuItem(data);
}

QString generator::renderGuiMenuItem(ModelPtr data)
{
    QString result = "\\textsc{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderKeyCombo(ModelPtr data)
{
    addSpace_ = true;
    QString result;
    for (int i=0; i<data->children().size(); i++) {
        if (i>0) {
            result += " ";
        }
        result += renderElement(data->children().at(i));
    }
    return " " + result + " ";
}

QString generator::renderKeySym(ModelPtr data)
{
    addSpace_ = false;
    QString result;
    QString keysym;
    foreach (ModelPtr  child, data->children()) {
        keysym += child->text();
    }
    const QStringList keys = keysym.split("+", QString::SkipEmptyParts);
    QStringList parts;
    foreach (const QString & key, keys) {
        QString part = "\\textsc{";
        part += key.toUpper();
        part += "}";
        parts.push_back(part);
    }
    result = parts.join("+");
    return result;

}

QString generator::renderMath(ModelPtr data)
{
    addSpace_ = false;
    QString result;
    if (data->children().size() > 1) {
        result += "\\begin{equation}\n";
    }
    else {
        result += " $";
    }
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    if (data->children().size() > 1) {
        result += "\\end{equation}\n";
    }
    else {
        result += "$ ";
    }
    if (result == " $$ ") {
        addSpace_ = true;
        return "";
    }
    else {
        addSpace_ = false;
        return result;
    }
}

QString generator::renderMRow(ModelPtr data)
{
    QString result;
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString generator::renderMSup(ModelPtr data)
{
    QString result = "^{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMSqrt(ModelPtr data)
{
    QString result = "\\sqrt{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMFrac(ModelPtr data)
{
    QString above, below;
    QList<ModelPtr> mrows;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == MathML_MRow) {
            mrows << child;
        }
    }
    if (mrows.size() > 1) {
        above = renderElement(mrows[0]);
        below = renderElement(mrows[1]);
    }
    return QString::fromLatin1("\\frac{%1}{%2}").arg(above).arg(below);
}

QString generator::renderMN(ModelPtr data)
{
    QString result = "\\mathrm{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMO(ModelPtr data)
{
    QString result;
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}


QString generator::renderMI(ModelPtr data)
{
    QString name = data->children().size() > 0 ?
                data->children().at(0)->text().trimmed() : "";
    static const QStringList PredefinedNames = QStringList()
            << "sin" << "cos" << "tan" << "cot" << "arcsin"
            << "arccos" << "arctan" << "arccot" << "sinh"
            << "cosh" << "tanh" << "coth" << "sec" << "csc";

    static const QStringList RMNames = QStringList()
            << "ln" << "log" << "lg" << "tg" << "ctg" <<
               "arctg" << "arcctg";

    static QMap<QChar,QString> GreekSymbols;
    if (GreekSymbols.isEmpty()) {
        GreekSymbols[QChar(0x03B1)] = "alpha";
        GreekSymbols[QChar(0x03B2)] = "beta";
        GreekSymbols[QChar(0x03B3)] = "gamma";
    }

    QString result;
    if (PredefinedNames.contains(name))
        result = "\\" + name;
    else if (RMNames.contains(name))
        result = "\\mathrm{" + name + "}";
    else if (name.length() == 1 && GreekSymbols.contains(name.at(0)))
        result = "\\" + GreekSymbols[name.at(0)];
    else
        result = name;
    addSpace_ = false;
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
        replacements[QString(QChar(0x00D7))] = "$\\times$";
        replacements[QString(QChar(0x2026))] = "$\\ldots$";
        replacements[QString(QChar(0x2192))] = "$\\rightarrow$";
        replacements[QString(QChar(0x2190))] = "$\\leftarrow$";
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
    result += QString::fromLatin1("\\begin{Example}\n");
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += QString::fromLatin1("\\caption{%1}\n").arg(normalizeText(data->title()));
    result += "\\end{Example}\n";
    if (twoColumnLayout_) {
        sectionFloats_.push_back(result);
        return "";
    }
    else {
        return result;
    }
}

QString generator::renderMediaObject(ModelPtr data)
{
    QString result;    
    ModelPtr caption;
    QString imageUrl;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == Caption)
            caption = child;
        else if (child == ImageObject && imageUrl.length()==0) {
            if (child->children().size() > 0 &&
                    child->children().at(0) == ImageData)
            {
                ModelPtr imageData = child->children().at(0);
                imageUrl = imageData->href().toLocalFile();
                if (!imageUrl.toLower().endsWith(".pdf"))
                    imageUrl.clear();
            }
        }
    }
    if (imageUrl.length() == 0 && allowRasterImages_) {
        Q_FOREACH(ModelPtr child, data->children()) {
            if (child == ImageObject && imageUrl.length()==0) {
                if (child->children().size() > 0 &&
                        child->children().at(0) == ImageData)
                {
                    ModelPtr imageData = child->children().at(0);
                    imageUrl = imageData->href().toLocalFile();
                    if (!imageUrl.toLower().endsWith(".png"))
                        imageUrl.clear();
                }
            }
        }
    }
    if (imageUrl.length() == 0)
        return "";
    result += QString::fromLatin1("\\begin{Figure}\n");
    result += "\\centering\\includegraphics{" + imageUrl + "}\n";
    result += "\\caption{";
    if (caption) {
        Q_FOREACH(ModelPtr child, caption->children())
            result += renderElement(child);
    }
    result += "}\n";
    result += "\\end{Figure}\n";
    if (twoColumnLayout_) {
        sectionFloats_.push_back(result);
        return "";
    }
    else {
        return result;
    }
    return result;
}

QString generator::renderTable(ModelPtr data)
{
    QString result;
    result += QString::fromLatin1("\\begin{Table}\n");
    result += QString::fromLatin1("\\caption{%1}\n").arg(normalizeText(data->title()));
    tableColumns_ = 0u;
    calculateTableColumnsCount(data);
    result += "\\begin{mdframed}\n";
    result += QString::fromLatin1("\\begin{tabularx}{\\linewidth}{");
    for (size_t c=0u; c<tableColumns_; c++)
        result += "*{7}{>{\\centering\\arraybackslash}X}";
    result += "}\n\\hline\n";
    ModelPtr head;
    ModelPtr body;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == THead)
            head = child;
        else if (child == TBody)
            body = child;
    }
    if (head && head->children().count() > 0 && head->children().at(0) == Row) {
        result += renderTableRow(head->children().at(0));
        result += " \\\\\n\\hline\n";
    }
    Q_FOREACH(ModelPtr row, body->children()) {
        if (row == Row) {
            result += renderTableRow(row);
            result += " \\\\[2ex]\n";
        }
    }
    result += "\\end{tabularx}\n";
    result += "\\end{mdframed}\n";
    result += "\\end{Table}\n";
    if (twoColumnLayout_) {
        sectionFloats_.push_back(result);
        return "";
    }
    else {
        return result;
    }
    return result;
}

QString generator::renderTableRow(ModelPtr data)
{
    QString result;
    int index = 0;
    addSpace_ = false;
    Q_FOREACH(ModelPtr col, data->children()) {
        if (col == Entry) {
            QString entry;
            Q_FOREACH(ModelPtr content, col->children()) {
                entry += renderElement(content);
            }
            entry = entry.trimmed();
            if (entry.length() == 0)
                result += "~";
            else
                result += "{" + entry + "}";
            if (index < data->children().size()-1)
                result += " & ";
            index ++;
        }
    }

    return result;
}

void generator::calculateTableColumnsCount(ModelPtr data)
{
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == Row)
            tableColumns_ = max(tableColumns_, calculateRowColumnsCount(child));
        else if (child == THead || child == TBody)
            calculateTableColumnsCount(child);
    }
}

size_t generator::calculateRowColumnsCount(ModelPtr data)
{
    size_t result = 0u;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == Entry) {
            result += 1u;
        }
    }
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
    result += QString::fromLatin1("\\title{%1}\n").arg(root_->title());
    result += "\\thispagestyle{empty}\n";
    result += "\\vspace*{\\fill}\n";
    result += "\\begin{center}\n";
    result += QString::fromLatin1("\\Large\\textbf{\\MakeUppercase{%1}}\n\n").arg(normalizeText(root_->title()));
    if (root_->subtitle().length() > 0) {
        result += "\\vspace*{18pt}\n";
        result += QString::fromLatin1("\\Large\\textbf{%1}\n\n").arg(normalizeText(root_->subtitle()));
    }
    result += "\\end{center}\n";
    result += "\\vspace*{\\fill}\n";
    result += "\\newpage\n";
    result += "\\tableofcontents\n\n";
    return result;
}

QString generator::renderArticleTitle()
{
    QString result;
    result += QString::fromLatin1("\\title{%1}\n").arg(root_->title());
    result += "\\thispagestyle{fancy}\n";

    result += "\\begin{center}\n";
    result += QString::fromLatin1("\\Large\\textbf{\\MakeUppercase{%1}}\n\n").arg(normalizeText(root_->title()));
    if (root_->subtitle().length() > 0) {
        result += "\\vspace*{18pt}\n";
        result += QString::fromLatin1("\\Large\\textbf{%1}\n\n").arg(normalizeText(root_->subtitle()));
    }
    result += "\\end{center}\n";

    ModelPtr abstract;
    Q_FOREACH(ModelPtr child, root_->children()) {
        if (child == Abstract || child == Preface)
            abstract = child;
    }
    if (abstract) {
        result += "\\begin{Abstract}\n";
        Q_FOREACH(ModelPtr achild, abstract->children()) result += renderElement(achild);
        result += "\\end{Abstract}\n";
    }
//    result += "\\end{@twocolumnfalse}\n]\n";
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
