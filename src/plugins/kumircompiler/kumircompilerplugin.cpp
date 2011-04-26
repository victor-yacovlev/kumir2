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
    m_analizer = dynamic_cast<Shared::AnalizerInterface*>(myDependency("KumirAnalizer"));
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
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setCodec("UTF-16");
            QString data = ts.readAll();
            f.close();

            int id = m_analizer->newDocument();
            m_analizer->setSourceText(id, data);
            QList<Shared::Error> errors = m_analizer->errors(id);
            for (int i=0; i<errors.size(); i++) {
                Shared::Error e = errors[i];
                std::cerr << filename.toLocal8Bit().data() << ":" << e.line << ": Error " << e.code << std::endl;
            }
        }
    }
}

void KumirCompilerPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirCompilerPlugin)
