#include "systemopenfilesettings.h"
#include "ui_systemopenfilesettings.h"

#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QRadioButton>
#include <QSpacerItem>

namespace CoreGUI {

SystemOpenFileSettings::SystemOpenFileSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemOpenFileSettings)
{
    ui->setupUi(this);

    const QString appsDir =
            QDir::cleanPath(QApplication::applicationDirPath() +
                            "/../share/applications");
    const QString iconsDir =
            QDir::cleanPath(QApplication::applicationDirPath() +
                            "/../share/icons/hicolor");
    scanForApplications(appsDir, iconsDir);

    chooseAtRunOption_ = new QRadioButton(tr("Choose on run"), this);
    ui->groupBox->layout()->addWidget(chooseAtRunOption_);
    ui->groupBox->layout()->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    foreach (const Application & app, applications_) {
        ui->groupBox->layout()->addWidget(app.button);
    }

    ui->groupBox->layout()->addItem(new QSpacerItem(10, 50, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));

    sett_ = new QSettings("kumir2", "kumir2-open");
    sett_->setIniCodec("UTF-8");
}

void SystemOpenFileSettings::init()
{
    const bool skipDialog = sett_->value("SkipOpenDialog", false).toBool();
    const QString lastKey = sett_->value("LastSelected", "").toString();
    QRadioButton * btn = chooseAtRunOption_;
    if (skipDialog && lastKey.length() > 0) {
        foreach (const Application & app, applications_) {
            if (app.key == lastKey) {
                btn = app.button;
                break;
            }
        }
    }
    btn->setChecked(true);
}

void SystemOpenFileSettings::accept()
{
    if (chooseAtRunOption_->isChecked()) {
        sett_->setValue("SkipOpenDialog", false);
    }
    else {
        sett_->setValue("SkipOpenDialog", true);
        foreach (const Application & app, applications_) {
            if (app.button->isChecked()) {
                sett_->setValue("LastSelected", app.key);
                break;
            }
        }
    }
}

void SystemOpenFileSettings::scanForApplications(const QString &appsDirPath,
                                             const QString &iconsDirPath)
{
    QDir appsDir(appsDirPath);
    QDir iconsDir(iconsDirPath);
    QStringList files = appsDir.entryList(QStringList() << "kumir2-*.desktop");
    foreach (const QString & fileName, files) {
        QFile f(appsDir.absoluteFilePath(fileName));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream ts(&f);
            ts.setCodec("UTF-8");
            QStringList lines = ts.readAll().split("\n");
            f.close();
            Application app;
            bool isApp = false;
            foreach (const QString & line, lines) {
                int eqPos = line.indexOf('=');
                if (eqPos != -1) {
                    const QString key = line.left(eqPos).trimmed().toLower();
                    const QString value = line.mid(eqPos + 1).trimmed();
                    if (key == "type") {
                        isApp = value.toLower() == "application";
                    }
                    else if (key.startsWith("name")) {
                        if (app.name.isEmpty() || key=="name[ru]") {
                            app.name = value;
                        }
                    }
                    else if (key == "icon") {
                        const QString iconFileName =
                                iconsDir.absoluteFilePath(
                                    QString("32x32/apps/%1.png").arg(value)
                                    );
                        app.icon = QIcon(iconFileName);
                    }
                    else if (key == "exec") {
                        QString executableFileName;
                        QString cleanName = value;
                        int spacePos = cleanName.indexOf(' ');
                        if (spacePos != -1) {
                            cleanName = cleanName.left(spacePos);
                        }
                        executableFileName =
                                QDir::cleanPath(
                                    QApplication::applicationDirPath() +
                                    "/" + cleanName
                                    );
#ifdef Q_OS_WIN32
                        executableFileName += ".exe";
#endif
                        app.key = QFileInfo(executableFileName).baseName();
                    }
                }
            }
            if (isApp) {
                applications_.push_back(app);
            }
        }
    }

    for (int i=0 ; i<applications_.size(); i++) {
        Application & app = applications_[i];
        app.button = new QRadioButton(app.name, this);
    }
}

SystemOpenFileSettings::~SystemOpenFileSettings()
{
    delete ui;
}

} // namespace CoreGUI
