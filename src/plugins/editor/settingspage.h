#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QtGui>

namespace Editor {

namespace Ui {
    class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QSettings * settings, QWidget *parent = 0);
    ~SettingsPage();

    static QString KeyColorKw;
    static QString KeyColorType;
    static QString KeyColorNumeric;
    static QString KeyColorLiteral;
    static QString KeyColorAlg;
    static QString KeyColorMod;
    static QString KeyColorDoc;
    static QString KeyColorComment;

    static QString KeyBoldKw;
    static QString KeyBoldType;
    static QString KeyBoldNumeric;
    static QString KeyBoldLiteral;
    static QString KeyBoldAlg;
    static QString KeyBoldMod;
    static QString KeyBoldDoc;
    static QString KeyBoldComment;
    static QString KeyFontName;
    static QString KeyFontSize;

    static QString DefaultColorKw;
    static QString DefaultColorType;
    static QString DefaultColorNumeric;
    static QString DefaultColorLiteral;
    static QString DefaultColorAlg;
    static QString DefaultColorMod;
    static QString DefaultColorDoc;
    static QString DefaultColorComment;

    static bool DefaultBoldKw;
    static bool DefaultBoldType;
    static bool DefaultBoldNumeric;
    static bool DefaultBoldLiteral;
    static bool DefaultBoldAlg;
    static bool DefaultBoldMod;
    static bool DefaultBoldDoc;
    static bool DefaultBoldComment;
    static bool DefaultFontName;
    static bool DefaultFontSize;

    static QString defaultFontFamily();
    static int defaultFontSize;

    static QString KeyTempSwitchLayoutButton;
    static quint32 DefaultTempSwitchLayoutButton;

public slots:
    void accept();
    void init();
signals:
    void settingsChanged();
private slots:
    void showColorDialog();
    void setButtonColor(QToolButton * b, const QColor &color);
    QColor buttonColor(const QToolButton *b) const;
    void updateFontPreview();

private:

    Ui::SettingsPage *ui;
    QSettings * m_settings;
};


} // namespace Editor
#endif // SETTINGSPAGE_H
