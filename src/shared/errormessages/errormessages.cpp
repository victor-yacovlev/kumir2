#include "errormessages.h"

#include <QtCore>

namespace ErrorMessages {

typedef QPair<QString,QLocale::Language> Context;
typedef QHash<QString,QString> Database;

static QMap<Context, Database> database = QMap<Context,Database>();

QStringList readCSVRow(const QString &line)
{
    QStringList result;
    bool inQuote = false;
    static const QString DELIMS = ";,";
    QString current;
    for (int i=0; i<line.size(); i++) {
        if (inQuote) {
            if (line[i]=='"') {
                if (i>0 && line[i-1]=='\\')
                    current += "\"";
                else
                    inQuote = false;
            }
            else {
                current += line[i];
            }
        }
        else {
            if (line[i]=='"') {
                inQuote = true;
            }
            else if (DELIMS.contains(line[i])) {
                result << current;
                current = "";
            }
            else {
                current += line[i];
            }
        }
    }
    if (!current.isEmpty()) {
        result << current;
    }
    return result;
}

extern bool loadMessages(const QString &plugin)
{
    const QString fileName = qApp->property("sharePath").toString()
            + "/" + plugin.toLower() + "/messages.csv";
    QFile f(fileName);

    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        ts.setAutoDetectUnicode(true);
        QStringList languages = readCSVRow(ts.readLine()).mid(1);
        QList<Context> contexts;
        for (int i=0; i<languages.size(); i++) {
            QLocale loc(languages[i]);
            contexts << Context(plugin,loc.language());
        }
        if (contexts.isEmpty()) {
            qWarning() << fileName+": file data is empty or not valid CSV-file";
        }
        while (!ts.atEnd()) {
            QString line = ts.readLine().trimmed();
            if (line.isEmpty())
                break;
            QStringList row = readCSVRow(line);
            if (row.size()>1) {
                QString key = row[0];
                QStringList values = row.mid(1);
                for (int i=0; i<qMin(values.size(), contexts.size()); i++) {
                    Context context = contexts[i];
                    if (!values[i].isEmpty()) {
                        database[context].insert(key, values[i]);
                    }
                }
            }
        }
        f.close();
    }
    else {
        qWarning() << "Can't open file: "+fileName;
        return false;
    }
    return true;
}

extern QString message(const QString &plugin
                       , const QLocale::Language &language
                       , const QString &key)
{
    Context context(plugin, language);
    static const QRegExp arg1("\\\\1=\\{(\\S*)\\}");
    static const QRegExp arg2("\\\\2=\\{(\\S*)\\}");
    static const QRegExp arg3("\\\\3=\\{(\\S*)\\}");
    QString k = key;
    QStringList arguments;
    QString result;
    int p = arg1.indexIn(k);
    if (p!=-1) {
        arguments << arg1.cap(1);
        k.replace(p, arg1.matchedLength(), "%1");
    }
    p = arg2.indexIn(k);
    if (p!=-1) {
        arguments << arg2.cap(1);
        k.replace(p, arg2.matchedLength(), "%2");
    }
    p = arg3.indexIn(k);
    if (p!=-1) {
        arguments << arg3.cap(1);
        k.replace(p, arg3.matchedLength(), "%3");
    }
    if (database.contains(context) && database[context].contains(k)) {
        result = database[context][k];
    }
    else {
//        qWarning() << "No message entry for " << plugin << " / " << language << " : " << key;
        result = k;
    }
    if (arguments.size()==1)
        result = result.arg(arguments[0]);
    else if (arguments.size()==2)
        result = result.arg(arguments[0]).arg(arguments[1]);
    else if (arguments.size()==3)
        result = result.arg(arguments[0]).arg(arguments[1]).arg(arguments[2]);
    return result;
}


}
