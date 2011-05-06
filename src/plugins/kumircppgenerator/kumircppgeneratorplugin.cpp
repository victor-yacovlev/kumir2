#include "kumircppgeneratorplugin.h"
#include "abstractsyntaxtree/ast_module.h"

#include <QtCore>


namespace KumirCppGenerator {


struct Module {
    QString name;
    QString latinName;
    QString headerData;
    QString sourceData;
};

struct KumirCppGeneratorPrivate {
    QMap<QString,QString> nameTable;
    QList<Module*> modules;
    void addModule(const AST::Module * module);
    void compileModule(const AST::Module * module);
    inline bool nameFree(const QString & name) { return !nameTable.values().contains(name); }
};

KumirCppGeneratorPlugin::KumirCppGeneratorPlugin()
{
    d = new KumirCppGeneratorPrivate;
}

KumirCppGeneratorPlugin::~KumirCppGeneratorPlugin()
{
    for (int i=0; i<d->modules.size(); i++)
        delete d->modules[i];
    delete d;
}

QString KumirCppGeneratorPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    return "";
}

void KumirCppGeneratorPrivate::addModule(const AST::Module *module)
{

}

void KumirCppGeneratorPrivate::compileModule(const AST::Module *module)
{

}

void KumirCppGeneratorPlugin::start()
{

}

void KumirCppGeneratorPlugin::stop()
{

}

void KumirCppGeneratorPlugin::generateExecuable(const AST::Data *tree
                                                , QIODevice *out
                                                , Shared::GeneratorType &outType)
{
    for (int i=0; i<tree->modules.size(); i++) {
        d->addModule(tree->modules[i]);
    }
    for (int i=0; i<tree->modules.size(); i++) {
        d->compileModule(tree->modules[i]);
    }
    outType = Shared::GenNativeExecuable;
    const QString tempDir = QDir::tempPath()+"/kumir2-cc-"+QString::number(qApp->applicationPid());
    if (!QFile::exists(tempDir)) {
        QDir::temp().mkdir("kumir2-cc-"+QString::number(qApp->applicationPid()));
    }
    QDir temp = QDir(tempDir);
    const QStringList oldFiles = temp.entryList();
    foreach (const QString & f, oldFiles) {
        if (!f.startsWith(".")) {
            temp.remove(f);
        }
    }
    for (int i=0; i<d->modules.size(); i++) {
        QFile h(tempDir+"/"+d->modules[i]->latinName+".h");
        if (h.open(QIODevice::WriteOnly | QIODevice::Text)) {
            h.write(d->modules[i]->headerData.toLocal8Bit());
            h.close();
        }
        QFile c(tempDir+"/"+d->modules[i]->latinName+".c");
        if (c.open(QIODevice::WriteOnly | QIODevice::Text)) {
            c.write(d->modules[i]->sourceData.toLocal8Bit());
            c.close();
        }
    }
    const QString prevCurrent = QDir::currentPath();
    QString O;
    QDir::setCurrent(tempDir);
    for (int i=0; i<d->modules.size(); i++) {
        QProcess::execute("gcc -c -std=c99"
                          + d->modules[i]->latinName+".c"
                          );
        O += " "+d->modules[i]->latinName+".o";
    }
#ifdef Q_OS_WIN32
    const QString suffix = ".exe";
#else
    const QString suffix = ".bin";
#endif
    QString fileName = tempDir+"/execuable"+suffix;
    if (QString(out->metaObject()->className())==QString("QFile")) {
        QFile * outFile = qobject_cast<QFile*>(out);
        fileName = outFile->fileName();
    }
    QProcess::execute("gcc -o "
                      +fileName
                      +O
                      );
    QDir::setCurrent(prevCurrent);
}

}

Q_EXPORT_PLUGIN(KumirCppGenerator::KumirCppGeneratorPlugin)
