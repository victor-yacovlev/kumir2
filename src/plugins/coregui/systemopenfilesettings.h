#ifndef COREGUI_SYSTEMOPENFILESETTINGS_H
#define COREGUI_SYSTEMOPENFILESETTINGS_H

#include <QWidget>
#include <QIcon>
#include <QRadioButton>
#include <QSettings>

namespace CoreGUI {

namespace Ui {
class SystemOpenFileSettings;
}

class SystemOpenFileSettings : public QWidget
{
    Q_OBJECT

public:
    explicit SystemOpenFileSettings(QWidget *parent = 0);
    ~SystemOpenFileSettings();

public slots:
    void init();
    void accept();

private:
    struct Application {
        QString key;
        QString name;
        QIcon icon;
        QRadioButton * button;
    };

    void scanForApplications(const QString & appsDir, const QString & iconsDir);

    QList<Application> applications_;
    Ui::SystemOpenFileSettings *ui;
    QRadioButton *chooseAtRunOption_;
    QSettings * sett_;
};


} // namespace CoreGUI
#endif // COREGUI_SYSTEMOPENFILESETTINGS_H
