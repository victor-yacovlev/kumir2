#include "errormessages.h"

#include <QtCore>

namespace ErrorMessages {

typedef QPair<QString,QLocale::Language> Context;
typedef QHash<QString,QString> Database;

static QMap<Context, Database> database = QMap<Context,Database>();

extern bool loadMessages(const QString &plugin
                         , const QString &fileName)
{
    QFile f(fileName);

    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        ts.setAutoDetectUnicode(true);
        QStringList languages = ts.readLine().split(",",QString::SkipEmptyParts).mid(1);
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
            QStringList row = line.split(",",QString::KeepEmptyParts);
            if (row.size()>1) {
                QString key = row[0];
                QStringList values = row.mid(1);
                for (int i=0; i<values.size(); i++) {
                    Context context = contexts[i];
                    database[context].insert(key, values[i]);
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
    if (database.contains(context) && database[context].contains(key)) {
        return database[context][key];
    }
    else {
        qWarning() << "No message entry for " << plugin << " / " << language << " : " << key;
        return key;
    }
}


}
