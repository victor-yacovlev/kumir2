#include "gdbrunplugin.h"

namespace GDBRun {

GDBRunPlugin::GDBRunPlugin()
    : gdb_(0)
    , programFile_(0)
    , variablesModel_(0)
{

}

QString GDBRunPlugin::initialize(const QStringList &, const CommandLine &)
{
    gdb_ = new QProcess(this);
    programFile_ = 0;
    variablesModel_ = 0;
    return "";
}

void GDBRunPlugin::updateSettings(const QStringList &)
{

}

bool GDBRunPlugin::loadProgram(const QString &fileName, const QByteArray &)
{
    if (programFile_) {
        programFile_->deleteLater();
    }
    if (variablesModel_) {
        variablesModel_->deleteLater();
    }
    programFile_ = new QFile(fileName, this);
    variablesModel_ = new QStandardItemModel(this);
    return programFile_->isReadable();
}

QDateTime GDBRunPlugin::loadedProgramVersion() const
{
    if (!programFile_ || !programFile_->exists()) {
        return QDateTime();
    }
    else {
        QFileInfo fi(*programFile_);
        return fi.lastModified();
    }
}

bool GDBRunPlugin::canStepOut() const
{
    return false;
}

void GDBRunPlugin::runBlind()
{

}

void GDBRunPlugin::runContinuous()
{

}

void GDBRunPlugin::runStepInto()
{

}

void GDBRunPlugin::runStepOver()
{

}

void GDBRunPlugin::runToEnd()
{

}

void GDBRunPlugin::runTesting()
{

}

bool GDBRunPlugin::isTestingRun() const
{
    return false;
}

void GDBRunPlugin::terminate()
{

}

bool GDBRunPlugin::hasMoreInstructions() const
{
    return true;
}

bool GDBRunPlugin::hasTestingEntryPoint() const
{
    return false;
}

unsigned long int GDBRunPlugin::stepsCounted() const
{
    return 0;
}

int GDBRunPlugin::currentLineNo() const
{
    return -1;
}

QPair<quint32,quint32> GDBRunPlugin::currentColumn() const
{
    return QPair<quint32,quint32>(0u, 0u);
}

QString GDBRunPlugin::error() const
{
    return "";
}

QVariant GDBRunPlugin::valueStackTopItem() const
{
    return 0;
}

}


Q_EXPORT_PLUGIN2(GDBRunPlugin, GDBRun::GDBRunPlugin)
