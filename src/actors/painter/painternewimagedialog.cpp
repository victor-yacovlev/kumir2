#include "painternewimagedialog.h"
#include "ui_painternewimagedialog.h"
#include "paintermodule.h"
#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace ActorPainter {

PainterNewImageDialog::PainterNewImageDialog(QWidget *parent, PainterModule * module) :
    QDialog(parent),
    ui(new Ui::PainterNewImageDialog)
{
    m_module = module;
    ui->setupUi(this);
    ui->scrollable->setLayout(new QVBoxLayout);
    connect(ui->colorBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleColorChanged(QString)));
    connect(ui->radioButtonTemplate, SIGNAL(toggled(bool)), ui->pageTemplate, SLOT(setVisible(bool)));
    connect(ui->radioButtonCustom, SIGNAL(toggled(bool)), ui->pageCustom, SLOT(setVisible(bool)));
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleTemplateClicked(QListWidgetItem*)));
//    connect(ui->radioButtonTemplate, SIGNAL(toggled(bool)), ui->listWidget, SLOT(setEnabled(bool)));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

int PainterNewImageDialog::w() const
{
    return ui->w->value();
}

int PainterNewImageDialog::h() const
{
    return ui->h->value();
}

QString PainterNewImageDialog::color() const
{
    if (ui->colorBox->currentIndex()==ui->colorBox->count()-1) {
        return s_color;
    }
    else {
        return ui->colorBox->currentText();
    }
}

int PainterNewImageDialog::exec()
{
    static const QString resourcesRoot = m_module->myResourcesDir().absolutePath();
    static const QString templatesDir = resourcesRoot;
    QDir dir(templatesDir);
    sl_templateFiles = dir.entryList(QStringList() << "*.png");
    const QString directory_ini = dir.absoluteFilePath("templates.ini");
    QSettings * s = NULL;
    if (QFile::exists(directory_ini)) {
        s = new QSettings(directory_ini, QSettings::IniFormat);
        s->setIniCodec("UTF-8");
    }
    sl_templateNames.clear();
    foreach (const QString &fileName, sl_templateFiles) {
        QString name;
        if (s)
            name = s->value("Names/"+fileName, fileName).toString();
        else
            name = fileName;
        sl_templateNames << name;
    }
    if (s)
        s->deleteLater();
    ui->listWidget->clear();
    ui->listWidget->addItems(sl_templateNames);
    ExtensionSystem::SettingsPtr  ss = m_module->mySettings();
    const QString defaultPath = dir.absoluteFilePath("default.png");
    bool useTemplate = ss->value("NewCanvas/UseTemplate", QFile::exists(defaultPath)).toBool();
    int lastWidth = ss->value("NewCanvas/CustomWidth", 640).toInt();
    int lastHeight = ss->value("NewCanvas/CustomHeight", 480).toInt();
    int lastColorIndex = ss->value("NewCanvas/CustomColorIndex", 0).toInt();
    QString lastFileName = ss->value("NewCanvas/TemplateFileName", "default.png").toString();
    lastColorIndex = qMax(lastColorIndex, 0);
    lastColorIndex = qMin(lastColorIndex, ui->colorBox->count()-1);
    ui->w->setValue(lastWidth);
    ui->h->setValue(lastHeight);
    if (!QFile::exists(dir.absoluteFilePath(lastFileName))) {
        lastFileName = "default.png";
    }
    if (sl_templateFiles.contains(lastFileName)) {
        int index = sl_templateFiles.indexOf(lastFileName);
        ui->listWidget->setCurrentRow(index);
    }
    ui->listWidget->setEnabled(useTemplate);
    if (useTemplate) {
        ui->radioButtonTemplate->setChecked(true);
        ui->stackedWidget->setCurrentWidget(ui->pageTemplate);
        handleTemplateClicked(ui->listWidget->currentItem());
    }
    else {
        ui->radioButtonCustom->setChecked(true);
        ui->stackedWidget->setCurrentWidget(ui->pageCustom);
    }
    int result = QDialog::exec();
    if (result==QDialog::Accepted) {
        ss->setValue("NewCanvas/UseTemplate", ui->radioButtonTemplate->isChecked());
        ss->setValue("NewCanvas/CustomWidth", ui->w->value());
        ss->setValue("NewCanvas/CustomHeight", ui->h->value());
        ss->setValue("NewCanvas/CustomColorIndex", ui->colorBox->currentIndex());
        if (ui->listWidget->currentRow()!=-1) {
            ss->setValue("NewCanvas/TemplateFileName", sl_templateFiles.at(ui->listWidget->currentRow()));
        }
    }
    return result;
}

void PainterNewImageDialog::handleTemplateClicked(QListWidgetItem *item)
{
    if (item) {
        int index = ui->listWidget->currentRow();
        const QString baseName = sl_templateFiles[index];
        static const QString resourcesRoot = m_module->myResourcesDir().absolutePath();
        static const QString templatesDir = resourcesRoot;
        QDir dir(templatesDir);
        const QString fileName = dir.absoluteFilePath(baseName);
        if (QFile::exists(fileName)) {
            QPixmap px = QPixmap(fileName);
            ui->preview->setPixmap(px);
            ui->preview->setFixedSize(px.size());
            ui->preview->move(0,0);
            ui->scrollable->setFixedSize(px.size());
        }
    }
}

void PainterNewImageDialog::handleColorChanged(const QString &text)
{
    if (text.endsWith("...")) {
        QColor newColor = QColorDialog::getColor(QColor(s_color),this);
        if (newColor.isValid()) {
            s_color = newColor.name();
        }
        else {
            ui->colorBox->setCurrentIndex(0);
        }
    }
}

bool PainterNewImageDialog::isTemplate() const
{
    return ui->radioButtonTemplate->isChecked();
}

QString PainterNewImageDialog::templateFileName() const
{
    int index = ui->listWidget->currentRow();
    if (index==-1)
        return "";
    Q_ASSERT( index>=0 );
    Q_ASSERT( index<sl_templateFiles.size() );
    return sl_templateFiles[index];
}

QString PainterNewImageDialog::templateName() const
{
    int index = ui->listWidget->currentRow();
    Q_ASSERT( index>=0 );
    if (index==-1)
        return "";
    Q_ASSERT( index<sl_templateFiles.size() );
    return sl_templateNames[index];
}

PainterNewImageDialog::~PainterNewImageDialog()
{
    delete ui;
}

}
