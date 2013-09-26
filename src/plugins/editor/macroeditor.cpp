#include "macroeditor.h"
#include "ui_macroeditor.h"
#include "macro.h"

namespace Editor {

MacroEditor::MacroEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacroEditor),
    macro_(nullptr)
{
    ui->setupUi(this);
    ui->tableWidget->setSpan(
                ui->tableWidget->rowCount()-1,
                0,
                1,
                ui->tableWidget->columnCount()
                );
    connect(ui->lineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(checkMacroTitle(QString)));
    connect(ui->tableWidget, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
            this, SLOT(changeMacroLetter(QTableWidgetItem*, QTableWidgetItem*)));
}

void MacroEditor::setUsedSymbols(const QString &symbols, const QStringList & names)
{
    static const Qt::ItemFlags Enabled = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    static const Qt::ItemFlags Disabled = Qt::NoItemFlags;
    for (int row=0; row < ui->tableWidget->rowCount() - 1; row++) {
        for (int column=0; column < ui->tableWidget->columnCount(); column++) {
            QTableWidgetItem * item = ui->tableWidget->item(row, column);
            if (item) {
                item->setFlags(item->text().trimmed().length() > 0 ? Enabled : Disabled);
                item->setToolTip(QString::fromAscii("Esc, %1").arg(item->text()));
            }
        }
    }
    for (int i=0; i<symbols.length(); i++) {
        const QString ch = symbols[i].toUpper();
        const QString toolTip = tr("Already used by '%1'").arg(names[i]);
        for (int row=0; row < ui->tableWidget->rowCount() - 1; row++) {
            for (int column=0; column < ui->tableWidget->columnCount(); column++) {
                QTableWidgetItem * item = ui->tableWidget->item(row, column);
                if (item) {
                    if (item->text().toUpper() == ch) {
                        item->setFlags(Disabled);
                        item->setToolTip(toolTip);
                    }                    
                }
            }
        }
    }
}

void MacroEditor::setMacro(Macro *macro)
{
    static const Qt::ItemFlags Enabled = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    macro_ = macro;
    QTableWidgetItem * defItem = ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0);
    QTableWidgetItem * item = defItem;
    if (!macro->key.isNull())
    {
        for (int row=0; row < ui->tableWidget->rowCount(); row++) {
            for (int column=0; column < ui->tableWidget->columnCount(); column++) {
                QTableWidgetItem * it = ui->tableWidget->item(row, column);
                if (it && it->text().toUpper() == QString(macro->key.toUpper())) {
                    item = it;
                }
            }
        }
    }
    if (item != defItem) {
        item->setFlags(Enabled);
        item->setToolTip(QString::fromAscii("Esc, %1").arg(item->text()));
    }
    ui->tableWidget->setCurrentItem(item);
    ui->lineEdit->setText(macro->title.trimmed());
    checkMacroTitle(ui->lineEdit->text());
}


void MacroEditor::checkMacroTitle(const QString &title)
{
    if (macro_ && title.trimmed().length() > 0) {
        macro_->title = title.trimmed();
        ui->btnSave->setEnabled(true);
    }
    else {
        ui->btnSave->setEnabled(false);
    }
}

void MacroEditor::changeMacroLetter(QTableWidgetItem *item, QTableWidgetItem *)
{
    if (!macro_)
        return;
    macro_->key = QChar::Null;
    if (item && item->text().length() == 1) {
        macro_->key = item->text().at(0);
    }
}

MacroEditor::~MacroEditor()
{
    delete ui;
}

} // namespace Editor
