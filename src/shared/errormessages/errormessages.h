#ifndef ERRORMESSAGES_H
#define ERRORMESSAGES_H

#include <QString>
#include <QLocale>


inline QString _(const char * x) {
    return QString::fromAscii(x);
}

inline QString _(const char * x, const QString &a) {
    QString result = QString::fromAscii(x);
    result.replace("%1","\\1={%1}");
    return result.arg(a);
}

inline QString _(const char * x, const QString &a, const QString &b) {
    QString result = QString::fromAscii(x);
    result.replace("%1","\\1={%1}");
    result.replace("%2","\\2={%2}");
    return result.arg(a).arg(b);
}

inline QString _(const char * x, const QString &a, const QString &b, const QString &c) {
    QString result = QString::fromAscii(x);
    result.replace("%1","\\1={%1}");
    result.replace("%2","\\2={%2}");
    result.replace("%3","\\3={%3}");
    return result.arg(a).arg(b).arg(c);
}

namespace ErrorMessages {

    extern bool loadMessages(const QString &plugin);

    extern QString message(const QString &plugin
                           , const QLocale::Language &language
                           , const QString &key);
}

#endif
