#include "switchworkspacedialog.h"
#include "ui_switchworkspacedialog.h"

namespace ExtensionSystem {

QString SwitchWorkspaceDialog::WorkspacesListKey = "WorkspacesList";
QString SwitchWorkspaceDialog::CurrentWorkspaceKey = "CurrentWorkspace";
QString SwitchWorkspaceDialog::SkipChooseWorkspaceKey = "SkipChooseWorkspace";

SwitchWorkspaceDialog::SwitchWorkspaceDialog(QSettings * settings) :
    QDialog(0),
    ui(new Ui::SwitchWorkspaceDialog),
    m_settings(settings)
{
    ui->setupUi(this);
    QStringList list = m_settings->value(WorkspacesListKey, QStringList() << QDir::homePath()+"/Kumir/").toStringList();
    for (int i=0; i<list.size(); i++) {
        list[i] = QDir::toNativeSeparators(list[i]);
    }
    ui->comboBox->addItems(list);
    ui->checkBox->setChecked(m_settings->value(SkipChooseWorkspaceKey, false).toBool());
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(handleBrowseClicked()));
    connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
}

void SwitchWorkspaceDialog::setCurrentWorkspace(const QString &path)
{
    const QString nativeName = QDir::toNativeSeparators(path.endsWith("/")? path : path + "/");
    int index = -1;
    for (int i=0; i<ui->comboBox->count(); i++) {
        if (ui->comboBox->itemText(i).trimmed()==nativeName) {
            index = i;
            break;
        }
    }
    if (index!=-1) {
        ui->comboBox->setCurrentIndex(index);
    }
    else {
        ui->comboBox->insertItem(0, nativeName);
        ui->comboBox->setCurrentIndex(0);
    }
}

QString SwitchWorkspaceDialog::currentWorkspace() const
{
    return QDir::cleanPath(QDir::fromNativeSeparators(ui->comboBox->currentText()));
}

void SwitchWorkspaceDialog::handleBrowseClicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this
                                                        , tr("Select directory for use as workspace")
                                                        , ui->comboBox->currentText()
                                                        );
    if (!dirName.isEmpty()) {
        setCurrentWorkspace(dirName);
    }
}

void SwitchWorkspaceDialog::handleAccepted()
{
    QStringList list;
    for (int i=0; i<ui->comboBox->count(); i++) {
        list << QDir::fromNativeSeparators(ui->comboBox->itemText(i));
    }
    m_settings->setValue(WorkspacesListKey, list);
    m_settings->setValue(CurrentWorkspaceKey, currentWorkspace());
    m_settings->setValue(SkipChooseWorkspaceKey, ui->checkBox->isChecked());
}

SwitchWorkspaceDialog::~SwitchWorkspaceDialog()
{
    delete ui;
}

} // namespace CoreGUI
