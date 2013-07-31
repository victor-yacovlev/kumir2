#include "macrolisteditor.h"
#include "ui_macrolisteditor.h"

#include "macroeditor.h"

namespace Editor {

MacroListEditor::MacroListEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacroListEditor)
{
    ui->setupUi(this);
    static const QString iconsRoot =
            qApp->property("sharePath").toString() + "/editor/";
    ui->btnRemove->setIcon(QIcon(iconsRoot+"find-close.png"));
    ui->btnEdit->setIcon(QIcon(iconsRoot+"find-options.png"));
    ui->btnRemove->setEnabled(false);
    ui->btnEdit->setEnabled(false);

    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(handleItemChanged(QListWidgetItem*,QListWidgetItem*)));

    connect(ui->btnRemove, SIGNAL(clicked()), this, SLOT(removeMacro()));
    connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(editMacro()));
}

void MacroListEditor::handleItemChanged(QListWidgetItem *current, QListWidgetItem * )
{
    ui->btnRemove->setEnabled(current != nullptr);
    ui->btnEdit->setEnabled(current != nullptr);
}

void MacroListEditor::initialize(const QList<Macro> &macros,
                                 const QList<Macro> &systemMacros,
                                 const QString & keyPrefix)
{
    ui->btnRemove->setEnabled(false);
    ui->btnEdit->setEnabled(false);
    ui->listWidget->clear();
    macros_ = macros;
    systemMacros_ = systemMacros;
    prefix_ = keyPrefix;
    for (int i=0; i<macros_.size(); i++) {
        Macro & macro = macros_[i];
        QListWidgetItem * item = new QListWidgetItem;
        QString text = macro.title.trimmed();
        if (!macro.key.isNull()) {
            text += " (" + keyPrefix + macro.key + ")";
        }
        item->setText(text);
        ui->listWidget->addItem(item);
    }
    if (ui->listWidget->count() > 0) {
        ui->listWidget->setCurrentRow(0);
    }
}

void MacroListEditor::removeMacro()
{
    if (!ui->listWidget->currentItem())
        return;
    int index = ui->listWidget->currentRow();
    QListWidgetItem * item = ui->listWidget->takeItem(index);
    delete item;
    macros_.removeAt(index);
}

void MacroListEditor::editMacro()
{
    if (!ui->listWidget->currentItem())
        return;

    QListWidgetItem * item = ui->listWidget->currentItem();
    int index = ui->listWidget->currentRow();

    Macro macro = macros_[index];

    MacroEditor * editor = new MacroEditor(this);
    editor->setWindowTitle(tr("Edit recorded keyboard sequence..."));
    QList<Macro> allMacros = systemMacros_ + macros_;
    QString usedLetters;
    QStringList usedNames;
    foreach(Macro m, allMacros) {
        if (!m.key.isNull()) {
            usedLetters.push_back(m.key);
            usedNames.push_back(m.title);
        }
    }
    editor->setMacroPrefix(prefix_);
    editor->setUsedSymbols(usedLetters, usedNames);
    editor->setMacro(&macro);

    if (editor->exec() == QDialog::Accepted) {
        QString text = macro.title.trimmed();
        if (!macro.key.isNull()) {
            text += " (" + prefix_ + macro.key + ")";
        }
        item->setText(text);
        macros_[index] = macro;
    }
}


QList<Macro> MacroListEditor::result() const
{
    return macros_;
}


MacroListEditor::~MacroListEditor()
{
    delete ui;
}

} // namespace Editor
