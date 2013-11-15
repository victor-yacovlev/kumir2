#ifndef LLVMCODEGENERATOR_NAMETRANSLATOR_H
#define LLVMCODEGENERATOR_NAMETRANSLATOR_H

#include <string>
#include <QString>
#include <QMap>
#include <QStack>

namespace LLVMCodeGenerator {

typedef std::string CString;

class NameTranslator
{
public:
    NameTranslator();
    void reset();
    void beginNamespace();
    void endNamespace();
    CString add(const QString & name);
    CString addGlobal(const QString &name);
    CString find(const QString & name) const;
    static QString suggestName(const QString &x);
private:
    typedef struct {
        QString sName;
        CString cName;
        QMap<CString,QString> rmap;
        QMap<QString,CString> map;
    } Namespace;

    Namespace& ns();

    bool inUse(const CString &x) const;
    static bool isReservedName(const QString &x);

    QStack<Namespace> l_;
    Namespace g_;
};

} // namespace LLVMCodeGenerator

#endif // LLVMCODEGENERATOR_NAMETRANSLATOR_H
