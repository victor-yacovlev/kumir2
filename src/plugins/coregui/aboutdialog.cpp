#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QSysInfo>
#include <QtCore>
#include <QtGui>


namespace CoreGUI {

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    QString svnRev = "unknown";
    if (qApp->property("svnRev").isValid())
        svnRev = qApp->property("svnRev").toString();
    ui->label->setText(ui->label->text().arg(qApp->applicationVersion()).arg(svnRev));
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(qVersion()));
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
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(winVer));
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
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(linuxDist));
#endif
#ifdef Q_OS_MAC
    QString macVersion = "MacOS X ";
    if (QSysInfo::macVersion()==QSysInfo::MV_TIGER)
        macVersion += "Tiger";
    else if (QSysInfo::macVersion()==QSysInfo::MV_LEOPARD)
        macVersion += "Leopard";
    else if (QSysInfo::macVersion()==QSysInfo::MV_SNOWLEOPARD)
        macVersion += "Snow Leopard";
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(macVersion));
#endif

    ui->tableWidget->setItem(2, 0, new QTableWidgetItem(qApp->applicationFilePath()));

}

AboutDialog::~AboutDialog()
{
    delete ui;
}

} // namespace CoreGUI
