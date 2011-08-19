#ifndef KUMIRCPPGENERATOR_NAMEPROVIDER_H
#define KUMIRCPPGENERATOR_NAMEPROVIDER_H

#include <QtCore>
#include "dataformats/ast_variabletype.h"
#include "dataformats/ast_expression.h"

namespace KumirNativeGenerator {

typedef QMap<QString, QMap<QString,QString> > NamesDatabase;

class NameProvider : public QObject
{
    Q_OBJECT
public:
    explicit NameProvider(QObject *parent = 0);
    static QString baseTypeName(AST::VariableBaseType type);
    static QString operatorName(AST::ExpressionOperator op);
    inline QString name(const QString &name, const QString &ns) const
    {
        return m_database[ns][name];
    }
    QString findVariable(const QString &modName,
                         const QString &algName,
                         const QString &varName) const;

public slots:
    inline void reset() { m_database.clear(); }
    QString addName(const QString & name, const QString & ns);
    inline void insertName(const QString & name, const QString & ns)
    {
        m_database[ns][name] = name;
    }

private:
    static QString suggestCName(const QString &name);
    NamesDatabase m_database;

};

} // namespace KumirCppGenerator

#endif // KUMIRCPPGENERATOR_NAMEPROVIDER_H
