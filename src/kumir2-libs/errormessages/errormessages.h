#ifndef ERRORMESSAGES_H
#define ERRORMESSAGES_H

#include <QString>
#include <QLocale>
#include <QPair>

#undef EM_EXPORT
#ifdef ERRORMESSAGES_LIBRARY
#define EM_EXPORT Q_DECL_EXPORT
#else
#define EM_EXPORT Q_DECL_IMPORT
#endif

namespace Shared {

inline QString _(const char * x) {
    return QString::fromLatin1(x);
}

inline QString _(const char * x, const QString &a) {
    QString result = QString::fromLatin1(x);
    result.replace("%1","\\1={%1}");
    return result.arg(a);
}

inline QString _(const char * x, const QString &a, const QString &b) {
    QString result = QString::fromLatin1(x);
    result.replace("%1","\\1={%1}");
    result.replace("%2","\\2={%2}");
    return result.arg(a).arg(b);
}

inline QString _(const char * x, const QString &a, const QString &b, const QString &c) {
    QString result = QString::fromLatin1(x);
    result.replace("%1","\\1={%1}");
    result.replace("%2","\\2={%2}");
    result.replace("%3","\\3={%3}");
    return result.arg(a).arg(b).arg(c);
}


class EM_EXPORT ErrorMessages {
public:
    static bool loadMessages(const QString &plugin);

    static QString message(const QString &plugin
                           , const QLocale::Language &language
                           , const QString &key);
private:
    static QStringList readCSVRow(const QString &line);
    typedef QPair<QString,QLocale::Language> Context;
    typedef QHash<QString,QString> Database;
    static QMap<Context, Database> database;
};

}

#endif
