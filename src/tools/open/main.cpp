#include "dialog.h"
#include "appmanager.h"

#include <QApplication>
#include <QFileInfo>
#include <QDir>

int main(int argc, char ** argv)
{
    if (argc < 2) { return 127; }
    QApplication qapp(argc, argv);
    Q_UNUSED(qapp);
    ApplicationManager & appManager = ApplicationManager::get();
    if (appManager.applications().size() > 1) {
        OpenDialog dialog(appManager.applications());
        if (QDialog::Accepted == dialog.exec()) {
            QString absPath = QString::fromLocal8Bit(argv[1]);
            if (!QFileInfo(absPath).isAbsolute()) {
                absPath = QDir::current().absoluteFilePath(absPath);
            }
            appManager.open(dialog.selectedApplicationId(),
                            QUrl::fromLocalFile(absPath));
        }
    }
    return 0;
}
