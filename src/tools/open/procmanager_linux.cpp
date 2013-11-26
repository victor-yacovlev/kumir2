#include "procmanager_linux.h"

#include <QDir>
#include <QFileInfo>

pid_t ProcManagerLinux::find(const QString &executable)
{
    pid_t result = 0;
    QDir procFS("/proc");
    QFileInfoList procs = procFS.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach (const QFileInfo procDirInfo, procs) {
        QFileInfo procExeFile(procDirInfo.absoluteFilePath() + "/exe");
        if (procExeFile.exists() && procExeFile.isReadable() && procExeFile.isSymLink()) {
            const QString target = procExeFile.symLinkTarget();
            if (target == executable) {
                result = procDirInfo.fileName().toULongLong();
                break;
            }
        }
    }
    return result;
}
