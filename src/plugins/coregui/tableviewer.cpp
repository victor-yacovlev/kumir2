#include <QtCore>
#include <QtGui>

#include "tableviewer.h"

namespace CoreGUI {

typedef QPair<int,int> Size;

struct TableViewerPrivate
{
    int moduleId;
    int algId;
    int variableId;
    AST::Data * ast;
    class TableViewer * q;
    int getDimension() const;
    Size getSize(int dimension) const;
    QString getValue(int dim, int slice, int row, int col) const;
    void updateTableSizes();
    void updateTableContents();
    QList<QTableWidget*> tables;
};

int TableViewerPrivate::getDimension() const
{
//    int result = 0;
//    KumSingleModule * module = modules->module(moduleId);
//    if (module) {
//        symbol_table * table = module->Values();
//        if (table->isUsed(variableId))
//            result = table->symb_table[variableId].razm;
//    }
//    return result;
}

Size TableViewerPrivate::getSize(int dimension) const
{
    Size result(0,0);
//    KumSingleModule * module = modules->module(moduleId);
//    if (module) {
//        symbol_table * table = module->Values();
//        int a = 0, b = 0;
//        if (dimension==1) {
//            a = table->symb_table[variableId].size0_start;
//            b = table->symb_table[variableId].size0_end;
//        }
//        else if (dimension==2) {
//            a = table->symb_table[variableId].size1_start;
//            b = table->symb_table[variableId].size1_end;
//        }
//        else if (dimension==3) {
//            a = table->symb_table[variableId].size2_start;
//            b = table->symb_table[variableId].size2_end;
//        }
//        result = Size(a,b);
//    }
    return result;
}

void TableViewerPrivate::updateTableSizes()
{
    int dimension = getDimension();
    if (dimension==3) {
        int oldTabsCount = q->count();
        Size newSize = getSize(3);
        int newTabsCount = newSize.second - newSize.first + 1;
        while (oldTabsCount>newTabsCount && oldTabsCount>0) {
            QWidget * tabToRemove = q->widget(q->count()-1);
            q->removeTab(q->count()-1);
            tabToRemove->deleteLater();
            oldTabsCount = q->count();
            if (!tables.isEmpty()) {
                QTableWidget * table = tables.last();
                tables.pop_back();
                if (table)
                    table->deleteLater();
            }
        }
        while (oldTabsCount<newTabsCount) {
            QWidget * widget = new QWidget(q);
            q->addTab(widget, "");
            QVBoxLayout * l = new QVBoxLayout;
            l->setContentsMargins(0,0,0,0);
            widget->setLayout(l);
            oldTabsCount = q->count();
            tables << NULL;
        }
        q->tabBar()->setVisible(true);
        q->setTabPosition(QTabWidget::South);
        for (int i=0 ; i<q->count() ; i++) {
            q->setTabText(i, QString::number(newSize.first+i));
        }
    }
    else {
        while (q->count()>1) {
            QWidget * tabToRemove = q->widget(q->count()-1);
            q->removeTab(q->count()-1);
            tabToRemove->deleteLater();
            if (!tables.isEmpty()) {
                QTableWidget * table = tables.last();
                tables.pop_back();
                if (table)
                    table->deleteLater();
            }
        }
        if (tables.isEmpty()) {
            QWidget * widget = new QWidget(q);
            q->addTab(widget, "");
            QVBoxLayout * l = new QVBoxLayout;
            l->setContentsMargins(0,0,0,0);
            widget->setLayout(l);
            tables << NULL;
        }
        q->tabBar()->setVisible(false);
    }

    if (dimension==0) {
        tables[0]->deleteLater();
        q->tabBar()->setVisible(false);
        return;
    }

    Q_CHECK_PTR(q);

    for (int tabNo=0; tabNo<q->count(); tabNo++) {
        Size s;
        int newRowsCount = 0;
        if (dimension==1) {
            newRowsCount = 1;
        }
        else if (dimension==2) {
            s = getSize(1);
            newRowsCount = s.second - s.first + 1;
        }
        else {
            s = getSize(2);
            newRowsCount = s.second - s.first + 1;
        }

        int oldRowsCount = 0;
        QTableWidget * table = tables.at(tabNo);
        if (table)
            oldRowsCount = table->rowCount();
        int newColsCount = 0;

        if (dimension==1) {
            s = getSize(1);
            newColsCount = s.second - s.first + 1;
        }
        else if (dimension==2) {
            s = getSize(2);
            newColsCount = s.second - s.first + 1;
        }
        else if (dimension==3) {
            s = getSize(3);
            newColsCount = s.second - s.first + 1;
        }
        int oldColsCount = 0;
        if (table)
            oldColsCount = table->columnCount();
        if (newRowsCount!=newColsCount || newColsCount!=oldColsCount) {
            if (table) {
                table->deleteLater();
                table = NULL;
            }
            Q_CHECK_PTR(q);
            QWidget * container = q->widget(tabNo);
            Q_CHECK_PTR(container);
            if (newRowsCount>0 && newColsCount>0) {
                table = new QTableWidget(newRowsCount, newColsCount, container);
                container->layout()->addWidget(table);
            }
            tables[tabNo] = table;
        }
        if (dimension==1) {
            s = getSize(1);
        }
        else if (dimension==2) {
            s = getSize(2);
        }
        else if (dimension==3) {
            s = getSize(3);
        }
        if (table) {
            QStringList headingsForColumns;
            for (int colNo=0; colNo < table->columnCount(); colNo++) {
                headingsForColumns << QString::number(s.first+colNo);
            }
            if (dimension==1) {
                s = Size(1,1);
            }
            else if (dimension==2) {
                s = getSize(1);
            }
            else if (dimension==3) {
                s = getSize(2);
            }
            QStringList headingsForRows;
            for (int rowNo=0; rowNo < table->rowCount(); rowNo++) {
                headingsForRows << QString::number(s.first+rowNo);
            }
            table->setHorizontalHeaderLabels(headingsForColumns);
            table->setVerticalHeaderLabels(headingsForRows);
        }
    }
}

void TableViewerPrivate::updateTableContents()
{
    int dimension = getDimension();
    if (dimension==1) {
        QTableWidget * table = tables[0];
        if (!table)
            return;
        for (int colNo=0; colNo<table->columnCount(); colNo++) {
            const QString text = getValue(1,0,0,colNo);
            if (!text.isEmpty()) {
                QTableWidgetItem * item = table->item(0, colNo);
                if (!item) {
                    item = new QTableWidgetItem;
                    table->setItem(0, colNo, item);
                }
                if (text!=item->text())
                    item->setText(text);
            }
        }
    }
    else if (dimension==2) {
        QTableWidget * table = tables[0];
        if (!table)
            return;
        for (int rowNo=0; rowNo<table->rowCount(); rowNo++) {
            for (int colNo=0; colNo<table->columnCount(); colNo++) {
                const QString text = getValue(2,0,rowNo,colNo);
                if (!text.isEmpty()) {
                    QTableWidgetItem * item = table->item(rowNo, colNo);
                    if (!item) {
                        item = new QTableWidgetItem(text);
                        table->setItem(rowNo, colNo, item);
                    }
                    else if (text!=item->text()) {
                        item->setText(text);
                    }
                }
            }
        }
    }
    else if (dimension==3) {
        for (int tabNo=0; tabNo<q->count(); tabNo++) {
            QTableWidget * table = tables[tabNo];
            if (!table)
                continue;
//            if (q->currentIndex()!=tabNo)
//                continue;
            for (int rowNo=0; rowNo<table->rowCount(); rowNo++) {
                for (int colNo=0; colNo<table->columnCount(); colNo++) {
                    const QString text = getValue(3,tabNo,rowNo,colNo);
                    if (!text.isEmpty()) {
                        QTableWidgetItem * item = table->item(rowNo, colNo);
                        if (!item) {
                            item = new QTableWidgetItem;
                            table->setItem(rowNo, colNo, item);
                        }
                        if (text!=item->text())
                            item->setText(text);
                    }
                }
            }
        }
    }
}

QString TableViewerPrivate::getValue(int dim, int slice, int row, int col) const
{
    QString result;
    QVariant value;
//    KumSingleModule * module = modules->module(moduleId);
//    if (module) {
//        symbol_table * table = module->Values();
//        if (dim==1) {
//            value = table->symb_table[variableId].value.getVariantMassValue(col,0,0);
//        }
//        if (dim==2) {
//            value = table->symb_table[variableId].value.getVariantMassValue(row,col,0);
//        }
//        if (dim==3) {
//            value = table->symb_table[variableId].value.getVariantMassValue(slice,row,col);
//        }
//        if (value.type()==QVariant::Int) {
//            result = QString::number(value.toInt());
//        }
//        else if (value.type()==QVariant::Double) {
//            result = QString::number(value.toDouble());
//        }
//        else if (value.type()==QVariant::Bool) {
//            result = value.toBool()? QString::fromUtf8("Да") : QString::fromUtf8("Нет");
//        }
//        else if (value.type()==QVariant::Char) {
//            result = QString(value.toChar());
//        }
//        else {
//            result = value.toString();
//        }
//    }

    return result;
}


TableViewer::TableViewer(AST::Data *ast, int modId, int algId, int varId, QWidget *parent)
    : QTabWidget(parent)
{
    d = new TableViewerPrivate;
    d->q = this;
    d->moduleId = modId;
    d->algId = algId;
    d->variableId = varId;
    d->ast = ast;
}


int TableViewer::moduleId() const
{
    return d->moduleId;
}

int TableViewer::variableId() const
{
    return d->variableId;
}

void TableViewer::updateContent()
{
    d->updateTableSizes();
    d->updateTableContents();
}

TableViewer::~TableViewer()
{
    delete d;
}

}
