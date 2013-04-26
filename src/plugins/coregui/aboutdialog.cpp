#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QSysInfo>
#include <QtCore>
#include <QtGui>
#include "extensionsystem/pluginmanager.h"

namespace CoreGUI {

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->version->setText(qApp->applicationVersion());
    if (qApp->property("gitHash").isValid())
        ui->gitHash->setText(qApp->property("gitHash").toString());
    if (qApp->property("lastModified").isValid())
        ui->lastModified->setText(qApp->property("lastModified").toString());

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(1, 1000);
    ui->tableWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);

    addExecuablePath();
    addOsVersion();
    addQtVersion();
    addLoadedModules();

    connect(ui->btnCopyEnvironmentAndVersion,
            SIGNAL(clicked()), this, SLOT(copySystemInformationToClipboard()));
}

void AboutDialog::copySystemInformationToClipboard()
{
    QString textToCopy;
    textToCopy += "Version: "+ui->version->text()+"\n";
    textToCopy += "Git: "+ui->gitHash->text() + "\n";
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        const QString key = ui->tableWidget->item(i, 0)->text();
        const QString value = ui->tableWidget->item(i, 1)->text();
        textToCopy += key + ": " + value + "\n";
    }
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText(textToCopy);
    QMessageBox::information(
                this,
                tr("Copied to clipboard"),
                tr("<b>The following text has been copied to clipboard:</b>\n\n%1")
                .arg(textToCopy).replace("\n", "<br/>"),
                QMessageBox::Ok
                );
}

void AboutDialog::addQtVersion()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(tr("Qt Version")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(qVersion()));
}

void AboutDialog::addOsVersion()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(tr("Operating System")));
#ifdef Q_OS_WIN
    QString winVer = "Windows ";
    if (QSysInfo::windowsVersion()==QSysInfo::WV_XP)
        winVer += "XP";
    else if (QSysInfo::windowsVersion()==QSysInfo::WV_2003)
        winVer += "2003";
    else if (QSysInfo::windowsVersion()==QSysInfo::WV_VISTA)
        winVer += "Vista";
    else if (QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS7)
        winVer += "7";
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem(winVer));
#endif
    QString linuxDist = "Linux";
    QDir d("/etc");
    QStringList els = d.entryList();
#ifdef Q_OS_LINUX
    foreach ( const QString & s, els ) {
        if (s.endsWith("-release") && !s.startsWith("lsb")) {
            QFile f("/etc/"+s);
            if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
                QStringList lines = QString::fromLocal8Bit(f.readAll()).split("\n");
                f.close();
                linuxDist = lines.join("; ");
                break;
            }
        }
    }
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem(linuxDist));
#endif
#ifdef Q_OS_MAC
    QString macVersion = "MacOS X ";
    if (QSysInfo::MacVersion()==QSysInfo::MV_TIGER)
        macVersion += "Tiger";
    else if (QSysInfo::MacVersion()==QSysInfo::MV_LEOPARD)
        macVersion += "Leopard";
    else if (QSysInfo::MacVersion()==QSysInfo::MV_SNOWLEOPARD)
        macVersion += "Snow Leopard";
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem(macVersion));
#endif
}

void AboutDialog::addExecuablePath()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(tr("Execuable Path")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(qApp->applicationFilePath()));
}

void AboutDialog::addLoadedModules()
{
    const ExtensionSystem::PluginManager * pm = ExtensionSystem::PluginManager::instance();
    QList<const ExtensionSystem::KPlugin*> all = pm->loadedConstPlugins();
    QStringList names;
    for (int i=0; i<all.size(); i++) {
        names << all[i]->pluginSpec().name;
    }
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(tr("Loaded Modules")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(names.join(", ")));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

} // namespace CoreGUI
