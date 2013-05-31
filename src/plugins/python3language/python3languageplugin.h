#ifndef PYTHON3_LANGUAGE_PLUGIN_H
#define PYTHON3_LANGUAGE_PLUGIN_H

#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
#include "extensionsystem/kplugin.h"
#include <QtCore>
#include "Python.h"

namespace Python3Language {

struct Python3Module;
using namespace Shared;

class Python3LanguagePlugin
        : public ExtensionSystem::KPlugin
        , public Shared::AnalizerInterface
        , public Shared::RunInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::AnalizerInterface Shared::RunInterface)
public:
    Python3LanguagePlugin();
    // Analizer interface methods
    int newDocument();
    inline bool primaryAlphabetIsLatin() const { return true; }
    inline bool caseInsensitiveGrammatic() const { return false; }
    inline bool supportPartialCompiling() const { return false; }
    inline virtual bool indentsSignificant() const { return true; }
    void dropDocument(int documentId);
    void setSourceText(int documentId, const QString &text);
    void setHiddenText(int documentId, const QString &text, int baseLine);
    void setHiddenTextBaseLine(int documentId, int baseLine);
    void changeSourceText(int documentId, const QList<ChangeTextTransaction> &changes);
    QList<Error> errors(int documentId) const;
    QList<LineProp> lineProperties(int documentId) const;
    QList<QPoint> lineRanks(int documentId) const;
    QStringList imports(int documentId) const;
    inline const AST::Data* abstractSyntaxTree(int documentId) const { return 0; }
    LineProp lineProp(int documentId, int lineNo, const QString &text) const;
    std::string rawSourceData(int documentId) const;
    QString createImportStatementLine(const QString &importName) const;

    // Runner interface methods
    bool loadProgram(const QString &fileName, const QByteArray & source, ProgramFormat format);
    QDateTime loadedProgramVersion() const;
    void setSourceDirName(int documentId, const QString &dirPath);
    QVariant valueStackTopItem() const;

    QMap<QString,QVariant> getScalarLocalValues(int frameNo) const;
    QMap<QString,QVariant> getScalarGlobalValues(const QString & moduleName) const;
    QVariantList getLocalTableValues(int frameNo, int maxCount, const QString & name,
                                     const QList< QPair<int,int> > & ranges, bool & complete) const;
    QVariantList getGlobalTableValues(const QString & moduleName, int maxCount, const QString & name,
                                      const QList< QPair<int,int> > & ranges, bool & complete) const;
    QVariant getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const;
    QVariant getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const;
    bool canStepOut() const;
    void runBlind();
    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runStepOut();
    void runTesting();
    void terminate();
    bool hasMoreInstructions() const;
    int currentLineNo() const;
    QString error() const;

protected:
    QString initialize(const QStringList &arguments);
private:
    ::PyObject * analizerModule_;
};


} // namespace

#endif
