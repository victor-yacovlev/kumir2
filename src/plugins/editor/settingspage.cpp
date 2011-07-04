#include "settingspage.h"
#include "ui_settingspage.h"

namespace Editor {

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

QString SettingsPage::DefaultColorKw = "#000000";
QString SettingsPage::DefaultColorType = "#c05800";
QString SettingsPage::DefaultColorNumeric = "#0095ff";
QString SettingsPage::DefaultColorLiteral = "#0095ff";
QString SettingsPage::DefaultColorAlg = "#0000c8";
QString SettingsPage::DefaultColorMod = "#00aa00";
QString SettingsPage::DefaultColorDoc = "#a3acff";
QString SettingsPage::DefaultColorComment = "#888888";

bool SettingsPage::DefaultBoldKw = true;
bool SettingsPage::DefaultBoldType = true;
bool SettingsPage::DefaultBoldNumeric = true;
bool SettingsPage::DefaultBoldLiteral = true;
bool SettingsPage::DefaultBoldAlg = true;
bool SettingsPage::DefaultBoldMod = true;
bool SettingsPage::DefaultBoldDoc = true;
bool SettingsPage::DefaultBoldComment = false;

QString SettingsPage::KeyTempSwitchLayoutButton = "Keyboard/TemporarySwitchLayoutButton";
quint32 SettingsPage::DefaultTempSwitchLayoutButton = Qt::Key_Alt;

QString SettingsPage::KeyPlayMacroShortcut = "Keyboard/PlayMacroShortcut";
QString SettingsPage::DefaultPlayMacroShortcut = "Esc";


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

SettingsPage::SettingsPage(QSettings * settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage),
    m_settings(settings)
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
}

void SettingsPage::accept()
{
    m_settings->setValue(KeyColorKw, buttonColor(ui->kwColor).name());
    m_settings->setValue(KeyColorType, buttonColor(ui->typeColor).name());
    m_settings->setValue(KeyColorNumeric, buttonColor(ui->numericColor).name());
    m_settings->setValue(KeyColorLiteral, buttonColor(ui->literalColor).name());
    m_settings->setValue(KeyColorAlg, buttonColor(ui->algorhitmColor).name());
    m_settings->setValue(KeyColorMod, buttonColor(ui->moduleColor).name());
    m_settings->setValue(KeyColorDoc, buttonColor(ui->docColor).name());
    m_settings->setValue(KeyColorComment, buttonColor(ui->commentColor).name());

    m_settings->setValue(KeyBoldKw, ui->kwBold->isChecked());
    m_settings->setValue(KeyBoldType, ui->typeBold->isChecked());
    m_settings->setValue(KeyBoldNumeric, ui->numericBold->isChecked());
    m_settings->setValue(KeyBoldLiteral, ui->literalBold->isChecked());
    m_settings->setValue(KeyBoldAlg, ui->algorhitmBold->isChecked());
    m_settings->setValue(KeyBoldMod, ui->moduleBold->isChecked());
    m_settings->setValue(KeyBoldDoc, ui->docBold->isChecked());
    m_settings->setValue(KeyBoldComment, ui->commentBold->isChecked());

    m_settings->setValue(KeyFontName, ui->fontFamily->currentFont().family());
    m_settings->setValue(KeyFontSize, ui->fontSize->value());

    const QString layoutSwitchKey = ui->layoutSwitchKey->currentText();

    if (layoutSwitchKey=="AltGr")
        m_settings->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_AltGr));
    else if (layoutSwitchKey=="Meta")
        m_settings->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Meta));
    else if (layoutSwitchKey=="Menu")
        m_settings->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Menu));
    else if (layoutSwitchKey=="ScrollLock")
        m_settings->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_ScrollLock));
    else if (layoutSwitchKey=="Pause")
        m_settings->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Pause));
    else
        m_settings->setValue(KeyTempSwitchLayoutButton, quint32(Qt::Key_Alt));

    const QString play = ui->macroPlayKey->currentText();
    m_settings->setValue(KeyPlayMacroShortcut, play);

    emit settingsChanged();
}

void SettingsPage::init()
{
    setButtonColor(ui->kwColor, QColor(m_settings->value(KeyColorKw, DefaultColorKw).toString()));
    setButtonColor(ui->typeColor, QColor(m_settings->value(KeyColorType, DefaultColorType).toString()));
    setButtonColor(ui->numericColor, QColor(m_settings->value(KeyColorNumeric, DefaultColorNumeric).toString()));
    setButtonColor(ui->literalColor, QColor(m_settings->value(KeyColorLiteral, DefaultColorLiteral).toString()));
    setButtonColor(ui->algorhitmColor, QColor(m_settings->value(KeyColorAlg, DefaultColorAlg).toString()));
    setButtonColor(ui->moduleColor, QColor(m_settings->value(KeyColorMod, DefaultColorMod).toString()));
    setButtonColor(ui->docColor, QColor(m_settings->value(KeyColorDoc, DefaultColorDoc).toString()));
    setButtonColor(ui->commentColor, QColor(m_settings->value(KeyColorComment, DefaultColorComment).toString()));

    ui->kwBold->setChecked(m_settings->value(KeyBoldKw, DefaultBoldKw).toBool());
    ui->typeBold->setChecked(m_settings->value(KeyBoldType, DefaultBoldType).toBool());
    ui->numericBold->setChecked(m_settings->value(KeyBoldNumeric, DefaultBoldNumeric).toBool());
    ui->literalBold->setChecked(m_settings->value(KeyBoldLiteral, DefaultBoldLiteral).toBool());
    ui->algorhitmBold->setChecked(m_settings->value(KeyBoldAlg, DefaultBoldAlg).toBool());
    ui->moduleBold->setChecked(m_settings->value(KeyBoldMod, DefaultBoldMod).toBool());
    ui->docBold->setChecked(m_settings->value(KeyBoldDoc, DefaultBoldDoc).toBool());
    ui->commentBold->setChecked(m_settings->value(KeyBoldComment, DefaultBoldComment).toBool());

    QFont f;
    f.setFamily(m_settings->value(KeyFontName, defaultFontFamily()).toString());
    ui->fontFamily->setCurrentFont(f);
    ui->fontSize->setValue(m_settings->value(KeyFontSize, defaultFontSize).toInt());

    Qt::Key key = Qt::Key(m_settings->value(KeyTempSwitchLayoutButton, DefaultTempSwitchLayoutButton).toUInt());
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

    QString play = m_settings->value(KeyPlayMacroShortcut, DefaultPlayMacroShortcut).toString();
    index = 0;
    for (int i=0; i<ui->macroPlayKey->count(); i++) {
        if (ui->macroPlayKey->itemText(i)==play) {
            index = i;
            break;
        }
    }
    ui->macroPlayKey->setCurrentIndex(index);

    updateFontPreview();
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

void SettingsPage::changeSettings(QSettings *settings)
{
    m_settings = settings;
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
