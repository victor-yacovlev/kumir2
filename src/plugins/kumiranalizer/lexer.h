#ifndef LEXER_H
#define LEXER_H

#include <kumir2-libs/dataformats/lexem.h>
#include <kumir2/lexemtype.h>
#include "statement.h"
#include <kumir2-libs/dataformats/ast_variabletype.h>

#include <QtCore>

namespace KumirAnalizer {

class Lexer : public QObject
{
    Q_OBJECT
public:
    explicit Lexer(QObject *parent = 0);
    ~Lexer();
    static void setLanguage(const QDir & resourcesRoot, const QLocale::Language & language);
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
    void setSourceDirName(const QString &dir);

public slots:
    int splitIntoStatements(const QStringList &lines
                             , int baseLineNo
                             , QList<TextStatementPtr> &statements
                             , const QStringList & extraTypeNames
                             ) const;
    void splitIntoLexems(const QString & text, QList<LexemPtr> & lexems,
                         const QStringList & extraTypeNames);
private /*methods*/:
    void splitLineIntoLexems(const QString &text
                             , QList<LexemPtr> & lexems
                             , const QStringList & extraTypeNames
                             ) const;

    void groupLexemsByStatements(const QList<LexemPtr> & lexems
                                 , QList<TextStatementPtr> & statements
                                 ) const;

    void preprocessIncludeStatements(QList<TextStatementPtr> & statements, const QStringList &) const;
    QList<TextStatementPtr> preprocessOneIncludeStatement(const TextStatementPtr include, const QStringList &) const;

    static void InitNormalizator(const QString &fileName);

private /*fields*/:
    QString _sourceDirName;

    static QStringList _KeyWords;
    static QStringList _Operators;
    static QStringList _TypeNames;
    static QStringList _ConstNames;
    static QStringList _Compounds;

    static QHash<QString,Shared::LexemType> _KwdMap;
    static QHash<QString,AST::VariableBaseType> _BaseTypes;
    static QHash<QString,AST::VariableBaseType> _BaseTypes0;
    static QHash<QString,bool> _BoolConstantValues;
    static QSet<QString> _ArrayTypes;
    static QString _RetvalKeyword;
    static QRegExp _RxCompound;
    static QRegExp _RxKeyWords;
    static QRegExp _RxConst;
    static QRegExp _RxTypes;

};

}

#endif // LEXER_H
