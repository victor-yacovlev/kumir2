#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QtGui>

namespace Editor {

namespace Ui {
    class SettingsPage;
}

class SettingsPage : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsPage(QSettings * settings, QWidget *parent = 0);
    ~SettingsPage();
public slots:
    void accept();

private:
    Ui::SettingsPage *ui;
    QSettings * m_settings;
};


} // namespace Editor
#endif // SETTINGSPAGE_H
