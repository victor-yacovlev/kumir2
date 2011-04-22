#ifndef LEXER_H
#define LEXER_H

#include "interfaces/lineprop.h"
#include "statement.h"
#include "interfaces/error.h"

#include <QtCore>

namespace KumirAnalizer {

class Lexer : public QObject
{
    Q_OBJECT
public:
    explicit Lexer(QObject *parent = 0);
    static void setLanguage(const QLocale::Language & language);
public slots:
    int splitIntoStatements(const QString &text
                             , int baseLineNo
                             , QList<Shared::LineProp> &props
                             , QList<Statement> &statements
                             ) const;
private:
    class LexerPrivate * d;

};

}

#endif // LEXER_H
