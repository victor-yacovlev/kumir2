#include "dialog.h"
#include "appmanager.h"
#include "settings.h"

#include <QApplication>
#include <QFileInfo>
#include <QDir>

int main(int argc, char ** argv)
{
    if (argc < 2) { return 127; }

    QApplication qapp(argc, argv); Q_UNUSED(qapp);

    QString absPath = QString::fromLocal8Bit(argv[1]);
    if (!QFileInfo(absPath).isAbsolute()) {
        absPath = QDir::current().absoluteFilePath(absPath);
    }
    const QUrl url = QUrl::fromLocalFile(absPath);

    ApplicationManager & appManager = ApplicationManager::get();
    if (appManager.applications().size() > 1) {

        const Settings & sett = Settings::get();

        if (sett.skipOpenDialog() &&
                appManager.find(sett.lastSelectedKey()).id)
        {
            const Application app = appManager.find(sett.lastSelectedKey());
            appManager.open(app, url);
        }
        else {
            OpenDialog dialog(appManager.applications());
            if (QDialog::Accepted == dialog.exec()) {
                appManager.open(dialog.selectedApplicationId(), url);
            }
        }

    }
    else if (appManager.applications().size() == 0) {
        const quintptr id = appManager.applications().at(0).id;
        appManager.open(id, url);
    }
    return 0;
}
