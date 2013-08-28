#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "extensionsystem/settings.h"

#include <QtGui>

namespace Editor {



namespace Ui {
    class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT
    Q_ENUMS(FreeCursorMovementType)

public:

    enum FreeCursorMovementType {
        CommentsAndTexts = 0,
        Always = 1,
        TextsOnly = 2
    };

    explicit SettingsPage(ExtensionSystem::SettingsPtr settings, QWidget *parent = 0);
    void changeSettings(ExtensionSystem::SettingsPtr settings);
    ~SettingsPage();

    static QString KeyInvertColorsIfDarkSystemTheme;
    static bool DefaultInvertColorsIfDarkSystemTheme;

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
    static QString KeyProgramTemplateFile;

    static QString KeyForcePressTextToLeft;
    static bool DefaultForcePressTextToLeft;

    static QString DefaultColorKw;
    static QString DefaultColorType;
    static QString DefaultColorNumeric;
    static QString DefaultColorLiteral;
    static QString DefaultColorAlg;
    static QString DefaultColorMod;
    static QString DefaultColorDoc;
    static QString DefaultColorComment;
    static QString DefaultProgramTemplateFile;

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
    static QString KeyPlayMacroShortcut;
    static QString DefaultPlayMacroShortcut;
    static quint32 DefaultTempSwitchLayoutButton;

    static QString KeyShowTrailingSpaces;
    static bool DefaultShowTrailingSpaces;

    static QString KeyAutoInsertPairingBraces;
    static bool DefaultAutoInsertPairingBraces;

    static QString KeyFreeCursorMovement;
    static FreeCursorMovementType DefaultFreeCursorMovement;

public slots:
    void accept();
    void init();
    void resetToDefaults();
signals:
    void settingsChanged(const QStringList & keys);
private slots:
    void showColorDialog();
    void setButtonColor(QToolButton * b, const QColor &color);
    QColor buttonColor(const QToolButton *b) const;
    void updateFontPreview();
    void validateProgramTemplateFile();
    void browseInitialProgramTemplateFile();

private:

    Ui::SettingsPage *ui;
    ExtensionSystem::SettingsPtr settings_;
};


} // namespace Editor
#endif // SETTINGSPAGE_H
