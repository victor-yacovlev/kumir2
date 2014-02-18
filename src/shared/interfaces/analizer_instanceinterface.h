#ifndef ANALIZER_COMMONINTERFACE_H
#define ANALIZER_COMMONINTERFACE_H


#include "analizer_compilerinterface.h"
#include "analizer_helperinterface.h"

#include "dataformats/ast.h"
#include "error.h"
#include "lineprop.h"

#include <QString>
#include <QList>

#include <string>

namespace Shared { namespace Analizer {

struct Error
{
    int line; // line number from 0
    int start; // position in line from 0
    int len; // error markup length
    QString code; // error code
};

typedef QVector<LexemType> LineProp;


class InstanceInterface {
public:
    virtual void setSourceDirName(const QString & path) = 0;
    virtual void setSourceText(const QString & plainText) = 0;
    virtual std::string rawSourceData() const = 0;
    virtual QList<Error> errors() const = 0;
    virtual QList<LineProp> lineProperties() const = 0;
    virtual QList<QPoint> lineRanks() const = 0;
    virtual LineProp lineProp(int lineNo, const QString & text) const = 0;
    virtual QString correctCapitalization(const QString & name, LexemType lxType) const { return name; }

    inline virtual ASTCompilerInterface * compiler() {
        QObject * me = dynamic_cast<QObject*>(this);
        if (!me) return 0;
        ASTCompilerInterface* i = qobject_cast<ASTCompilerInterface*>(me);
        return i;
    }

    inline virtual ExternalExecutableCompilerInterface * externalExecutableCompiler() {
        QObject * me = dynamic_cast<QObject*>(this);
        if (!me) return 0;
        ExternalExecutableCompilerInterface* i = qobject_cast<ExternalExecutableCompilerInterface*>(me);
        return i;
    }

    inline virtual HelperInterface * helper() {
        QObject * me = dynamic_cast<QObject*>(this);
        if (!me) return 0;
        return qobject_cast<HelperInterface*>(me);
    }

};


}}

Q_DECLARE_INTERFACE(Shared::Analizer::InstanceInterface,
                    "kumir2.Analizer.InstanceInterface")


#endif // ANALIZER_COMMONINTERFACE_H
