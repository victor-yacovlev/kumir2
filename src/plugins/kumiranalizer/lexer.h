#ifndef LEXER_H
#define LEXER_H

#include "interfaces/lineprop.h"
#include "statement.h"
#include "interfaces/error.h"
#include "dataformats/ast_variabletype.h"

#include <QtCore>

namespace KumirAnalizer {

class Lexer : public QObject
{
    Q_OBJECT
public:
    explicit Lexer(QObject *parent = 0);
    ~Lexer();
    static void setLanguage(const QLocale::Language & language);
    static QString testingAlgorhitmName();
    static QString importKeyword();
    QString testName( const QString &stroka );
    AST::VariableBaseType baseTypeByClassName(const QString &clazz) const;
    QString classNameByBaseType(const AST::VariableBaseType &type) const;
    bool isArrayClassName(const QString &clazz) const;
    bool boolConstantValue(const QString &val) const;
    bool isReturnVariable(const QString & name) const;
    inline QString inputLexemName() const { return QString::fromUtf8("ввод"); }
    inline QString outputLexemName() const{ return QString::fromUtf8("вывод"); }
public slots:
    int splitIntoStatements(const QStringList &lines
                             , int baseLineNo
                             , QList<TextStatementPtr> &statements
                             , const QStringList & extraTypeNames
                             ) const;
    void splitIntoLexems(const QString & text, QList<LexemPtr> & lexems,
                         const QStringList & extraTypeNames);
private:
    struct LexerPrivate * d;

};

}

#endif // LEXER_H
