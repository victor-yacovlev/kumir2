#ifndef PASCALANALIZER_PASPLUGIN_H
#define PASCALANALIZER_PASPLUGIN_H

#include <QtCore>
#include "extensionsystem/kplugin.h"
#include "interfaces/analizerinterface.h"
#include "dataformats/lexem.h"
#include "ippc.h"

namespace PascalAnalizer {

class Lexer;

struct PascalDocument {
    QList<Shared::Error> errors;
    QList<LineProp> lineProps;
    QList<QPoint> indentRanks;
    QTemporaryFile * buffer;
    QStringList text;
    QProcess * fpc;
    IPPC * ippc;
    IPPC::Names localnames;
    IPPC::Names usednames;
    bool fetchTables;

};

class PasPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::AnalizerInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::AnalizerInterface)
public:
    explicit PasPlugin();
    inline bool primaryAlphabetIsLatin() const { return true; }
    inline bool caseInsensitiveGrammatic() const { return true; }
    inline bool supportPartialCompiling() const { return true; }
    std::string rawSourceData(int documentId) const;
    int newDocument();
    void dropDocument(int documentId);
    void setSourceText(int documentId, const QString &text);
    inline void setHiddenText(int , const QString &, int ) {}
    inline void setHiddenTextBaseLine(int , int ) {}
    void changeSourceText(int documentId, const QList<Shared::ChangeTextTransaction> & changes);
    QList<Shared::Error> errors(int documentId) const;
    QList<Shared::LineProp> lineProperties(int documentId) const;
    QList<QPoint> lineRanks(int documentId) const;
    QStringList imports(int documentId) const;
    inline const AST::Data * abstractSyntaxTree(int) const { return 0; }
    Shared::LineProp lineProp(int documentId, const QString & text) const;
    QStringList algorhitmsAvailableFor(int documentId, int lineNo) const;
    QStringList globalsAvailableFor(int documentId, int lineNo) const;
    QStringList localsAvailableFor(int documentId, int lineNo) const;
    inline QList<QRegExp> supportedFileNamePattern() const {
        return QList<QRegExp>()
                << QRegExp("*.pas",Qt::CaseInsensitive,QRegExp::Wildcard)
                << QRegExp("*.pp",Qt::CaseInsensitive,QRegExp::Wildcard);
    }
    static QString getRTLUnitPath();
protected:
#ifdef Q_OS_UNIX
    static QString probeFPC();
    static QString s_fpcUnitsDir;
#endif
    QString initialize(const QStringList &arguments);
    void updateSourceText(int documentId, const Shared::ChangeTextTransaction & changes);
private:

    std::vector<PascalDocument*> v_documents;
    void doAnalisys(int documentId);
};

} // namespace PascalAnalizer

#endif // PASCALANALIZER_PASPLUGIN_H
