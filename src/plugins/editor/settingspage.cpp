#include "settingspage.h"
#include "ui_settingspage.h"

#include "extensionsystem/pluginmanager.h"
#include "interfaces/analizerinterface.h"

namespace Editor {

QString SettingsPage::KeyInvertColorsIfDarkSystemTheme = "Highlight/InvertColorsIfDarkTheme";
bool SettingsPage::DefaultInvertColorsIfDarkSystemTheme = true;

QString SettingsPage::KeyColorKw = "Highlight/Keyword/Color";
QString SettingsPage::KeyColorType = "Highlight/Type/Color";
QString SettingsPage::KeyColorNumeric = "Highlight/Numeric/Color";
QString SettingsPage::KeyColorLiteral = "Highlight/Literal/Color";
QString SettingsPage::KeyColorAlg = "Highlight/Algorhitm/Color";
QString SettingsPage::KeyColorMod = "Highlight/Module/Color";
QString SettingsPage::KeyColorDoc = "Highlight/Doc/Color";
QString SettingsPage::KeyColorComment = "Highlight/Comment/Color";

QString SettingsPage::KeyBoldKw = "Highlight/Keyword/Bold";
QString SettingsPage::KeyBoldType = "Highlight/Type/Bold";
QString SettingsPage::KeyBoldNumeric = "Highlight/Numeric/Bold";
QString SettingsPage::KeyBoldLiteral = "Highlight/Literal/Bold";
QString SettingsPage::KeyBoldAlg = "Highlight/Algorhitm/Bold";
QString SettingsPage::KeyBoldMod = "Highlight/Module/Bold";
QString SettingsPage::KeyBoldDoc = "Highlight/Doc/Bold";
QString SettingsPage::KeyBoldComment = "Highlight/Comment/Bold";

QString SettingsPage::KeyFontName = "Font/Family";
QString SettingsPage::KeyFontSize = "Font/Size";

QString SettingsPage::KeyProgramTemplateFile = "Content/DefaultProgramFile";

QString SettingsPage::DefaultColorKw = "#000000";
QString SettingsPage::DefaultColorType = "#c05800";
QString SettingsPage::DefaultColorNumeric = "#0095ff";
QString SettingsPage::DefaultColorLiteral = "#0095ff";
QString SettingsPage::DefaultColorAlg = "#0000c8";
QString SettingsPage::DefaultColorMod = "#008c00";
QString SettingsPage::DefaultColorDoc = "#a3acff";
QString SettingsPage::DefaultColorComment = "#888888";

QString SettingsPage::DefaultProgramTemplateFile = "${RESOURCES}/editor/default";

bool SettingsPage::DefaultBoldKw = true;
bool SettingsPage::DefaultBoldType = true;
bool SettingsPage::DefaultBoldNumeric = true;
bool SettingsPage::DefaultBoldLiteral = true;
bool SettingsPage::DefaultBoldAlg = true;
bool SettingsPage::DefaultBoldMod = true;
bool SettingsPage::DefaultBoldDoc = true;
bool SettingsPage::DefaultBoldComment = true;

QString SettingsPage::KeyTempSwitchLayoutButton = "Keyboard/TemporarySwitchLayoutButton";
quint32 SettingsPage::DefaultTempSwitchLayoutButton = Qt::Key_Alt;

QString SettingsPage::KeyForcePressTextToLeft = "Other/PressTextToLeft";
bool SettingsPage::DefaultForcePressTextToLeft = false;

QString SettingsPage::KeyShowTrailingSpaces = "Other/ShowTrailingSpaces";
bool SettingsPage::DefaultShowTrailingSpaces = false;

QString SettingsPage::KeyAutoInsertPairingBraces = "Other/AutoInsertPairingBraces";
bool SettingsPage::DefaultAutoInsertPairingBraces = false;

QString SettingsPage::KeyFreeCursorMovement = "Other/FreeCursorMovement";
SettingsPage::FreeCursorMovementType SettingsPage::DefaultFreeCursorMovement = SettingsPage::CommentsAndTexts;

QString SettingsPage::defaultFontFamily()
{
    QString result = "Courier";
#ifdef Q_WS_X11
    result = "DejaVu Sans Mono";
#endif
#ifdef Q_OS_WIN32
    if (QSysInfo::WindowsVersion>=0x0080)
        result = "Consolas";
#endif
#ifdef Q_OS_MAC
    result = "Courier"; // TODO implement me
#endif
    return result;
}

int SettingsPage::defaultFontSize = 12;

SettingsPage::SettingsPage(ExtensionSystem::SettingsPtr  settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage),
    settings_(settings)
{
    ui->setupUi(this);
    connect(ui->fontFamily, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFontPreview()));
    connect(ui->fontSize, SIGNAL(valueChanged(int)), this, SLOT(updateFontPreview()));

    connect(ui->kwColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->typeColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->numericColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->literalColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->algorhitmColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->moduleColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->docColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->commentColor, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    connect(ui->templateFileName, SIGNAL(textChanged(QString)), this, SLOT(validateProgramTemplateFile()));
    connect(ui->btnBrowseTemplateFile, SIGNAL(clicked()), this, SLOT(browseInitialProgramTemplateFile()));
}

void SettingsPage::accept()
{
    settings_->setValue(KeyInvertColorsIfDarkSystemTheme, ui->invertColors->isChecked());

    settings_->setValue(KeyColorKw, buttonColor(ui->kwColor).name());
    settings_->setValue(KeyColorType, buttonColor(ui->typeColor).name());
    settings_->setValue(KeyColorNumeric, buttonColor(ui->numericColor).name());
    settings_->setValue(KeyColorLiteral, buttonColor(ui->literalColor).name());
    settings_->setValue(KeyColorAlg, buttonColor(ui->algorhitmColor).name());
    settings_->setValue(KeyColorMod, buttonColor(ui->moduleColor).name());
    settings_->setValue(KeyColorDoc, buttonColor(ui->docColor).name());
    settings_->setValue(KeyColorComment, buttonColor(ui->commentColor).name());

    settings_->setValue(KeyBoldKw, ui->kwBold->isChecked());
    settings_->setValue(KeyBoldType, ui->typeBold->isChecked());
    settings_->setValue(KeyBoldNumeric, ui->numericBold->isChecked());
    settings_->setValue(KeyBoldLiteral, ui->literalBold->isChecked());
    settings_->setValue(KeyBoldAlg, ui->algorhitmBold->isChecked());
    settings_->setValue(KeyBoldMod, ui->moduleBold->isChecked());
    settings_->setValue(KeyBoldDoc, ui->docBold->isChecked());
    settings_->setValue(KeyBoldComment, ui->commentBold->isChecked());

    settings_->setValue(KeyFontName, ui->fontFamily->currentFont().family());
    settings_->setValue(KeyFontSize, ui->fontSize->value());

    const QString layoutSwitchKey = ui->layoutSwitchKey->currentText();

    if (layoutSwitchKey=="AltGr")
        settings_->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_AltGr));
    else if (layoutSwitchKey=="Meta")
        settings_->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Meta));
    else if (layoutSwitchKey=="Menu")
        settings_->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Menu));
    else if (layoutSwitchKey=="ScrollLock")
        settings_->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_ScrollLock));
    else if (layoutSwitchKey=="Pause")
        settings_->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Pause));
    else
        settings_->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Alt));


    settings_->setValue(KeyForcePressTextToLeft, ui->pressTextLeft->isChecked());
    settings_->setValue(KeyShowTrailingSpaces, ui->showTrailingSpaces->isChecked());
    settings_->setValue(KeyAutoInsertPairingBraces,
                        ui->autoInsertClosingOperationalBrackets->isChecked());

    uint freeCursor = ui->freeCursorPositioning->currentIndex();

    settings_->setValue(KeyFreeCursorMovement, freeCursor);

    settings_->setValue("Settings/FontCollapsed", ui->groupFont->isCollapsed());
    settings_->setValue("Settings/KeyboardCollapsed", ui->groupKeyboard->isCollapsed());
    settings_->setValue("Settings/SyntaxCollapsed", ui->groupSyntax->isCollapsed());
    settings_->setValue("Settings/InitialCollapsed", ui->groupInitial->isCollapsed());
    settings_->setValue("Settings/OtherCollapsed", ui->groupOther->isCollapsed());

    settings_->setValue(KeyProgramTemplateFile,
                         QDir::fromNativeSeparators(ui->templateFileName->text())
                         );   


    emit settingsChanged(QStringList());
}

void SettingsPage::init()
{
    ui->invertColors->setChecked(settings_->value(KeyInvertColorsIfDarkSystemTheme,
                                                  DefaultInvertColorsIfDarkSystemTheme).toBool());

    ui->groupFont->setCollapsed(settings_->value("Settings/FontCollapsed", 1).toBool());
    ui->groupKeyboard->setCollapsed(settings_->value("Settings/KeyboardCollapsed", 1).toBool());
    ui->groupSyntax->setCollapsed(settings_->value("Settings/SyntaxCollapsed", 1).toBool());
    ui->groupOther->setCollapsed(settings_->value("Settings/OtherCollapsed", 1).toBool());
    ui->groupInitial->setCollapsed(settings_->value("Settings/InitialCollapsed", 1).toBool());

    setButtonColor(ui->kwColor, QColor(settings_->value(KeyColorKw, DefaultColorKw).toString()));
    setButtonColor(ui->typeColor, QColor(settings_->value(KeyColorType, DefaultColorType).toString()));
    setButtonColor(ui->numericColor, QColor(settings_->value(KeyColorNumeric, DefaultColorNumeric).toString()));
    setButtonColor(ui->literalColor, QColor(settings_->value(KeyColorLiteral, DefaultColorLiteral).toString()));
    setButtonColor(ui->algorhitmColor, QColor(settings_->value(KeyColorAlg, DefaultColorAlg).toString()));
    setButtonColor(ui->moduleColor, QColor(settings_->value(KeyColorMod, DefaultColorMod).toString()));
    setButtonColor(ui->docColor, QColor(settings_->value(KeyColorDoc, DefaultColorDoc).toString()));
    setButtonColor(ui->commentColor, QColor(settings_->value(KeyColorComment, DefaultColorComment).toString()));

    ui->kwBold->setChecked(settings_->value(KeyBoldKw, DefaultBoldKw).toBool());
    ui->typeBold->setChecked(settings_->value(KeyBoldType, DefaultBoldType).toBool());
    ui->numericBold->setChecked(settings_->value(KeyBoldNumeric, DefaultBoldNumeric).toBool());
    ui->literalBold->setChecked(settings_->value(KeyBoldLiteral, DefaultBoldLiteral).toBool());
    ui->algorhitmBold->setChecked(settings_->value(KeyBoldAlg, DefaultBoldAlg).toBool());
    ui->moduleBold->setChecked(settings_->value(KeyBoldMod, DefaultBoldMod).toBool());
    ui->docBold->setChecked(settings_->value(KeyBoldDoc, DefaultBoldDoc).toBool());
    ui->commentBold->setChecked(settings_->value(KeyBoldComment, DefaultBoldComment).toBool());

    QFont f;
    f.setFamily(settings_->value(KeyFontName, defaultFontFamily()).toString());
    ui->fontFamily->setCurrentFont(f);
    ui->fontSize->setValue(settings_->value(KeyFontSize, defaultFontSize).toInt());

    Qt::Key key = Qt::Key(settings_->value(KeyTempSwitchLayoutButton, DefaultTempSwitchLayoutButton).toUInt());
    QString keyText = "Alt";
    if (key==Qt::Key_AltGr)
        keyText = "AltGr";
    else if (key==Qt::Key_Meta)
        keyText = "Meta";
    else if (key==Qt::Key_Menu)
        keyText = "Menu";
    else if (key==Qt::Key_ScrollLock)
        keyText = "ScrollLock";
    else if (key==Qt::Key_Pause)
        keyText = "Pause";

    int index = 0;
    for (int i=0; i<ui->layoutSwitchKey->count(); i++) {
        if (ui->layoutSwitchKey->itemText(i)==keyText) {
            index = i;
            break;
        }
    }
    ui->layoutSwitchKey->setCurrentIndex(index);


    ui->pressTextLeft->setChecked(settings_->value(KeyForcePressTextToLeft, DefaultForcePressTextToLeft).toBool());
    ui->showTrailingSpaces->setChecked(settings_->value(KeyShowTrailingSpaces, DefaultShowTrailingSpaces).toBool());

    ui->autoInsertClosingOperationalBrackets->setChecked(
                settings_->value(
                    KeyAutoInsertPairingBraces,
                    DefaultAutoInsertPairingBraces
                    ).toBool()
                );

    uint freeCursorMovement =
            settings_->value(KeyFreeCursorMovement, DefaultFreeCursorMovement)
            .toUInt();

    if (freeCursorMovement < ui->freeCursorPositioning->count())
        ui->freeCursorPositioning->setCurrentIndex(freeCursorMovement);

    QString initialFileName = DefaultProgramTemplateFile;

    Shared::AnalizerInterface * analizer =
            ExtensionSystem::PluginManager::instance()
            ->findPlugin<Shared::AnalizerInterface>();

    if (analizer) {
        initialFileName += "." + analizer->defaultDocumentFileNameSuffix();
    }
    else {
        initialFileName += ".txt";
    }

    static const QString resourcesRoot = QDir(qApp->applicationDirPath()+"/../share/kumir2/").canonicalPath();
    initialFileName.replace("${RESOURCES}", resourcesRoot);
    ui->templateFileName->setText(QDir::toNativeSeparators(
                                      settings_->value(KeyProgramTemplateFile,
                                                    initialFileName
                                                    ).toString()
                                      )
                                  );
    validateProgramTemplateFile();
    updateFontPreview();
}

void SettingsPage::resetToDefaults()
{
    ui->invertColors->setChecked(DefaultInvertColorsIfDarkSystemTheme);

    setButtonColor(ui->kwColor, QColor(DefaultColorKw));
    setButtonColor(ui->typeColor, QColor(DefaultColorType));
    setButtonColor(ui->numericColor, QColor(DefaultColorNumeric));
    setButtonColor(ui->literalColor, QColor(DefaultColorLiteral));
    setButtonColor(ui->algorhitmColor, QColor(DefaultColorAlg));
    setButtonColor(ui->moduleColor, QColor(DefaultColorMod));
    setButtonColor(ui->docColor, QColor(DefaultColorDoc));
    setButtonColor(ui->commentColor, QColor(DefaultColorComment));

    ui->kwBold->setChecked(DefaultBoldKw);
    ui->typeBold->setChecked(DefaultBoldType);
    ui->numericBold->setChecked(DefaultBoldNumeric);
    ui->literalBold->setChecked(DefaultBoldLiteral);
    ui->algorhitmBold->setChecked(DefaultBoldAlg);
    ui->moduleBold->setChecked(DefaultBoldMod);
    ui->docBold->setChecked(DefaultBoldDoc);
    ui->commentBold->setChecked(DefaultBoldComment);

    QFont f;
    f.setFamily(defaultFontFamily());
    ui->fontFamily->setCurrentFont(f);
    ui->fontSize->setValue(defaultFontSize);

    Qt::Key key = Qt::Key(DefaultTempSwitchLayoutButton);
    QString keyText = "Alt";
    if (key==Qt::Key_AltGr)
        keyText = "AltGr";
    else if (key==Qt::Key_Meta)
        keyText = "Meta";
    else if (key==Qt::Key_Menu)
        keyText = "Menu";
    else if (key==Qt::Key_ScrollLock)
        keyText = "ScrollLock";
    else if (key==Qt::Key_Pause)
        keyText = "Pause";

    int index = 0;
    for (int i=0; i<ui->layoutSwitchKey->count(); i++) {
        if (ui->layoutSwitchKey->itemText(i)==keyText) {
            index = i;
            break;
        }
    }
    ui->layoutSwitchKey->setCurrentIndex(index);


    ui->pressTextLeft->setChecked(DefaultForcePressTextToLeft);
    ui->showTrailingSpaces->setChecked(DefaultShowTrailingSpaces);

    ui->autoInsertClosingOperationalBrackets->setChecked(DefaultAutoInsertPairingBraces);

    uint freeCursorMovement = DefaultFreeCursorMovement;

    if (freeCursorMovement < ui->freeCursorPositioning->count())
        ui->freeCursorPositioning->setCurrentIndex(freeCursorMovement);

    QString initialFileName = DefaultProgramTemplateFile;

    Shared::AnalizerInterface * analizer =
            ExtensionSystem::PluginManager::instance()
            ->findPlugin<Shared::AnalizerInterface>();

    if (analizer) {
        initialFileName += "." + analizer->defaultDocumentFileNameSuffix();
    }
    else {
        initialFileName += ".txt";
    }

    static const QString resourcesRoot = QDir(qApp->applicationDirPath()+"/../share/kumir2/").canonicalPath();
    initialFileName.replace("${RESOURCES}", resourcesRoot);
    ui->templateFileName->setText(QDir::toNativeSeparators(initialFileName));
    validateProgramTemplateFile();
    updateFontPreview();
}

void SettingsPage::validateProgramTemplateFile()
{
    const QString fileName = QDir::fromNativeSeparators(ui->templateFileName->text().trimmed());
    QFileInfo fileInfo(fileName);
    QLabel * errorPlace = ui->fileNotExistsLabel;
    QString error;
    if (!fileInfo.exists()) {
        error = tr("File not exists");
    }
    else if (!fileInfo.isReadable()) {
        error = tr("File access denied");
    }
    errorPlace->setText(error);
}

void SettingsPage::browseInitialProgramTemplateFile()
{
    QString fileName = QDir::fromNativeSeparators(ui->templateFileName->text().trimmed());
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Choose initial program file"),
                                            fileName,
                                            QString("Kumir programs (*.kum)")
                                            );
    if (!fileName.isEmpty()) {
        ui->templateFileName->setText(QDir::toNativeSeparators(fileName));
    }
}

void SettingsPage::setButtonColor(QToolButton *btn, const QColor &color)
{
    QString style = QString("QToolButton { border: 1px solid ") + color.darker().name() + QString("; ") +
            QString("              border-radius: 4px; ")+
            QString("              background-color: ") + color.name() +
            QString("            }")+
            QString("QToolButton::pressed { background-color: ") + color.darker(50).name() + QString(" }");

    btn->setStyleSheet(style);
}

QColor SettingsPage::buttonColor(const QToolButton *b) const
{
    QString style = b->styleSheet();
    QRegExp rxBgColor("background-color:\\s*(\\S+)");
    if ( rxBgColor.indexIn(style) != -1 ) {
        QString color = rxBgColor.cap(1);
        return QColor(color);
    }
    else {
        return QColor(Qt::black);
    }
}

void SettingsPage::changeSettings(ExtensionSystem::SettingsPtr settings)
{
    settings_ = settings;
    init();
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::updateFontPreview()
{
    QFont f = ui->fontFamily->currentFont();
    f.setPointSize(ui->fontSize->value());
    ui->fontPreview->setFont(f);
}

void SettingsPage::showColorDialog()
{
    QToolButton * b = qobject_cast<QToolButton*>(sender());
    if (!b)
        return;
    QColor c = QColorDialog::getColor(
                buttonColor(b),
                this,
                tr("Choose color")
                );
    if (c.isValid()) {
        setButtonColor(b, c);
    }
}

} // namespace Editor
