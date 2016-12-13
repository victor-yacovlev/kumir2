#ifndef LLVMCODEGENERATOR_NAMETRANSLATOR_H
#define LLVMCODEGENERATOR_NAMETRANSLATOR_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QStack>

namespace LLVMCodeGenerator {

class NameTranslator
{
public:
    NameTranslator();
    void reset();
    void beginNamespace();
    void endNamespace();
    QByteArray add(const QString & name);
    QByteArray addGlobal(const QString &name);
    QByteArray find(const QString & name) const;
    static QString suggestName(const QString &x);
    static QByteArray screenUtf8Name(const QString &x);
private:
    typedef struct {
        QString sName;
        QByteArray cName;
        QMap<QByteArray,QString> rmap;
        QMap<QString,QByteArray> map;
    } Namespace;

    Namespace& ns();

    bool inUse(const QByteArray &x) const;
    static bool isReservedName(const QString &x);

    QStack<Namespace> l_;
    Namespace g_;
};

} // namespace LLVMCodeGenerator

#endif // LLVMCODEGENERATOR_NAMETRANSLATOR_H
