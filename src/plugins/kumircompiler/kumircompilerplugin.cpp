#include "kumircompilerplugin.h"

#include <QtCore>
#include "kumiranalizer/analizer.h"
#include "kumiranalizer/kumiranalizerplugin.h"

#include <iostream>

using namespace KumirCompiler;
using namespace KumirAnalizer;


KumirCompilerPlugin::KumirCompilerPlugin()
{


}


KumirCompilerPlugin::~KumirCompilerPlugin()
{

}


QString KumirCompilerPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    m_analizer = qobject_cast<Shared::AnalizerInterface*>(myDependency("KumirAnalizer"));
    if (m_analizer)
        return "";
    else
        return "Could not find KumirAnalizer interface";
}

void KumirCompilerPlugin::start()
{
    QString filename;
    for (int i=1; i<qApp->argc(); i++) {
        const QString arg = qApp->arguments()[i];
        if ( !arg.startsWith("-") && !arg.startsWith("[") && arg.endsWith(".kum")) {
            filename = arg;
        }
    }
    if (!filename.isEmpty()) {
        filename = QFileInfo(filename).absoluteFilePath();
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setCodec("UTF-16");
            QString data = ts.readAll();
            f.close();
//            data = QString::fromUtf8("лог l\nl:=3>2>1");
            int id = m_analizer->newDocument();
            m_analizer->setSourceText(id, data);
            QList<Shared::Error> errors = m_analizer->errors(id);
            const AST::Data * ast = m_analizer->abstractSyntaxTree(id);
            const QString baseName = QFileInfo(filename).baseName();
            const QString outFileName = QFileInfo(filename).dir().absoluteFilePath(
                        baseName+".dump.json");
            QFile ff(outFileName);
            if (ff.open(QIODevice::WriteOnly|QIODevice::Text)) {
                QTextStream ots(&ff);
                ots.setCodec("UTF-8");
                ots.setGenerateByteOrderMark(true);
                QString modJs = ast->dump();
                modJs.replace("\t", "  ");
                ots << modJs;
                ff.close();;
            }
            for (int i=0; i<errors.size(); i++) {
                Shared::Error e = errors[i];
                std::cerr << filename.toLocal8Bit().data() << ":" << e.line << ": Error " << e.code << std::endl;
            }
            qApp->setProperty("returnCode", errors.isEmpty()? 0 : 1);
        }
    }
    else {
        qApp->setProperty("returnCode", 127);
    }
}

void KumirCompilerPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirCompilerPlugin)
