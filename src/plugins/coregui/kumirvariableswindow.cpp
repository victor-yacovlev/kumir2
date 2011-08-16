/****************************************************************************
**
** Copyright (C) 2004-2011 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "kumirvariableswindow.h"
#include "tableviewer.h"


namespace CoreGUI {

KumirVariablesWindow::KumirVariablesWindow ( QWidget* parent )
    : QWidget ( parent )
    , Ui::KumirVariablesWindow()
{
    setupUi ( this );
    setHeaders();

    QFont bigFont = treeWidget->font();
    bigFont.setPointSize(bigFont.pointSize()+1);
    treeWidget->setFont(bigFont);
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(handleItemDoubleClicked(QTreeWidgetItem*,int)));
    //	treeWidget->setMouseTracking(true);
    //	connect(treeWidget, SIGNAL(itemEntered(QTreeWidgetItem*,int)),
    //			this, SLOT(handleItemHovered(QTreeWidgetItem*,int)));
}

void KumirVariablesWindow::setHeaders()
{
    treeWidget->setHeaderLabels( QStringList() << tr("Name") << tr("Type") << tr("Value") );
}

KumirVariablesWindow::~KumirVariablesWindow()
{
}

void KumirVariablesWindow::init(AST::Data *ast)
{
    m_ast = ast;
}

QVariant KumirVariablesWindow::getValue(int mod, int alg, int id) const
{
    QVariant result = QVariant::Invalid;

//    KumSingleModule * module = modules->module(mod);
//    Q_CHECK_PTR(module);
//    symbol_table * symbols = module->Values();
//    Q_CHECK_PTR(symbols);

//    if (symbols->isRes(id) || symbols->isArgRes(id)) {
//        PeremPrt link = symbols->pointTo(id);
//        if (link.mod_id>-1 && link.perem_id>-1) {
//            module = modules->module(link.mod_id);
//            Q_CHECK_PTR(module);
//            symbol_table * symbols = module->Values();
//            Q_CHECK_PTR(symbols);
//            id = link.perem_id;
//        }
//    }

//    if (symbols->isUsed(id)) {
//        result = symbols->symb_table[id].value.getValueAsVariant();
//    }
    return result;
}


bool KumirVariablesWindow::isFunction(const QString& name)
{
    bool result = false;
//    for (int i=0; i<modules->count(); i++) {
//        const function_table * functions = modules->module(i)->Functions();
//        result = functions->idByName(name) > -1;
//        if (result)
//            break;
//    }
    return result;
}

QString KumirVariablesWindow::unserialize1D(const QList<QVariant> l, int size, int &index)
{
    QString result;
    QVariant val;
    for (int x=0; x<size; x++) {
        result += " ";
        val = l[index];
        if (val.type()==QVariant::Invalid) {
            result += tr("?");
        }
        else {
            if (val.type()==QVariant::String) {
                result += "\""+val.toString()+"\"";
            }
            else if (val.type()==QVariant::Char) {
                result += "'"+val.toString()+"'";
            }
            else if (val.type()==QVariant::Bool) {
                result += val.toBool()? QString::fromUtf8("да") : QString::fromUtf8("нет");
            }
            else {
                result += val.toString();
            }
        }
        if (x<size-1) {
            result += ",";
        }
        index++;
    }
    return result;
}

QString KumirVariablesWindow::unserializeArray(const QVariant &data)
{
    QString result;
    QList<QVariant> l = data.toList();
    int dim = l[0].toInt();
    int start_x = 0;
    int size_x = 0;
    int start_y = 0;
    int size_y = 0;
    int start_z = 0;
    int size_z = 0;
    int offset = 0;
    if (dim>=1) {
        start_x = l[1].toInt();
        size_x = l[2].toInt();
        offset = 3;
    }
    if (dim>=2) {
        start_y = l[3].toInt();
        size_y = l[4].toInt();
        offset = 5;
    }
    if (dim>=3) {
        start_z = l[5].toInt();
        size_z = l[6].toInt();
        offset = 7;
    }
    //	K_ASSERT(size_x * size_y * size_z == l.size()-offset);
    int index = offset;
    QVariant val;
    if (dim == 1) {
        result += "[";
        result += unserialize1D(l, size_x, index);
        result += " ]";
    }
    if (dim == 2) {
        result += "[";
        for (int x=0; x<size_x; x++) {
            result += " [";
            result += unserialize1D(l, size_y, index);
            result += " ]";
            if (x<size_x-1)  {
                result += ",";
            }
        }
        result += " ]";
    }
    if (dim==3) {
        result += "[";
        for (int x=0; x<size_x; x++) {
            result += " [";
            for (int y=0; y<size_y; y++) {
                result += " [";
                result += unserialize1D(l, size_z, index);
                result += " ]";
                if (y<size_y-1) {
                    result += ",";
                }
            }
            result += " ]";
            if (x<size_x-1)  {
                result += ",";
            }
        }
        result += " ]";
    }

    return result;
}

void KumirVariablesWindow::refreshVariables()
{
    //qDebug()<<"KumirVariablesWindow::refreshVariables";

//    foreach (const QString &key, l_tables.keys()) {
//        if (l_childWindows[key]->isVisible()) {
//            l_tables[key]->updateContent();
//        }
//    }

//    if (!this->isVisible())
//        return;

//    QString module, moduleVisName, function, functionVisName, type, name;
//    QVariant value;
//    QString valueStr;

//    function_table * functions = NULL;
//    symbol_table * symbols = NULL;

//    for(int i=0; i<modules->count(); i++)
//    {
//        module = modules->module(i)->Name();
//        if (modules->module(i)->isEnabled() && modules->module(i)->isKumIsp())
//        {
//            if (module=="@main")
//                moduleVisName = tr("User algorithms");
//            else
//                moduleVisName = module;
//            QTreeWidgetItem* modItem;
//            QList<QTreeWidgetItem*> l = treeWidget->findItems(moduleVisName, Qt::MatchExactly, 0);
//            if (l.isEmpty())
//            {
//                modItem = new QTreeWidgetItem(treeWidget, QStringList(moduleVisName));
//                modItem->setExpanded(true);
//                QFont font(modItem->font(0));
//                font.setBold(true);
//                modItem->setFont(0, font);
//            }
//            else
//            {
//                modItem=l.first();
//            }

//            functions = modules->module(i)->Functions();
//            symbols = modules->module(i)->Values();
//            for(int j=0; j<functions->size()+1; ++j)
//            {
//                if (j==functions->size())
//                    function = "global";
//                else
//                    function = functions->nameById(j);
//                if (function.startsWith("@"))
//                    continue;
//                else if (function=="global")
//                    functionVisName = tr("Global vars");
//                else if (function.isEmpty())
//                    functionVisName = tr("Main alg");
//                else
//                    functionVisName = function;
//                QTreeWidgetItem* funcItem = NULL;
//                for(int k=0; k<modItem->childCount(); ++k)
//                {
//                    if (modItem->child(k)->text(0)==functionVisName)
//                    {
//                        funcItem=modItem->child(k);
//                        break;
//                    }
//                }
//                if (!funcItem)
//                {
//                    funcItem = new QTreeWidgetItem(modItem, QStringList(functionVisName));
//                    funcItem->setExpanded(true);
//                    QFont font(funcItem->font(0));
//                    font.setItalic(true);
//                    funcItem->setFont(0, font);
//                    funcItem->setFont(1, font);
//                    funcItem->setFont(2, font);
//                }

//                for(int k=0; k<symbols->symb_table.size(); ++k)
//                {
//                    if (function == symbols->symb_table[k].alg &&
//                            symbols->symb_table[k].name!="!KUM_CONST" &&
//                            !symbols->symb_table[k].name.startsWith("@") &&
//                            !symbols->symb_table[k].value.has_read)
//                    {
//                        symbols->symb_table[k].value.has_read = true;
//                        name = symbols->symb_table[k].name;
//                        QTreeWidgetItem* symbItem = NULL;
//                        if (isFunction(name))
//                        {
//                            for(int y=0; y<modItem->childCount(); ++y)
//                            {
//                                if (modItem->child(y)->text(0)==name)
//                                {
//                                    symbItem=modItem->child(y);
//                                    break;
//                                }
//                            }
//                        }
//                        else
//                        {
//                            for(int y=0; y<funcItem->childCount(); ++y)
//                            {
//                                if (funcItem->child(y)->text(0)==name)
//                                {
//                                    symbItem = funcItem->child(y);
//                                    break;
//                                }
//                            }
//                        }
//                        if (!symbItem)
//                        {
//                            symbItem = new QTreeWidgetItem(funcItem, QStringList(name));
//                            int modId = i;
//                            int varId = k;
//                            QString link = QString("%1.%2").arg(modId).arg(varId);
//                            symbItem->setData(0,Qt::UserRole,link);
//                        }

//                        value = getValue(i, k);
//                        if (!value.isValid()) {
//                            valueStr = tr("Not initialized");
//                            symbItem->setForeground(2, QBrush(QColor(Qt::gray)));
//                        }
//                        else if (value.type()!=QVariant::List ) {
//                            symbItem->setForeground(2, QBrush(QColor(Qt::black)));
//                            if (value.type()==QVariant::Bool) {
//                                valueStr = value.toBool()? QString::fromUtf8("да") : QString::fromUtf8("нет");
//                            }
//                            else {
//                                valueStr = value.toString();
//                            }
//                        }
//                        else if (value.type()==QVariant::List) {
//                            valueStr = unserializeArray(value);
//                        }
//                        PeremType ptype = symbols->getTypeByID(k);
//                        if (ptype==mass_integer ||
//                                ptype==mass_real ||
//                                ptype==mass_bool ||
//                                ptype==mass_charect ||
//                                ptype==mass_string)
//                        {
//                            const QString toolTip = tr("Double click to show table \"%1\" in separate window")
//                                    .arg(symbols->symb_table[k].name);
//                            symbItem->setToolTip(0,toolTip);
//                            symbItem->setToolTip(1,toolTip);
//                            symbItem->setToolTip(2,toolTip);
//                        }
//                        switch ( symbols->getTypeByID(k) )
//                        {
//                        case integer:
//                            type = tr("int"); break;
//                            break;
//                        case real:
//                            type = tr("real"); break;
//                            break;
//                        case kumBoolean:
//                            type = tr("bool"); break;
//                            break;
//                        case charect:
//                            type = tr("char"); break;
//                            break;
//                        case kumString:
//                            type = tr("str"); break;
//                            break;
//                        case mass_integer:
//                            type = tr("int tab");
//                            break;
//                        case mass_real:
//                            type = tr("real tab");
//                            break;
//                        case mass_bool:
//                            type = tr("bool tab");
//                            break;
//                        case mass_charect:
//                            type = tr("char tab");
//                            break;
//                        case mass_string:
//                            type = tr("str tab");
//                            break;
//                        default:
//                            type = "?";
//                            value = "?";
//                        }
//                        symbItem->setText(1,type);
//                        symbItem->setText(2,valueStr);
//                    }
//                }
//            }
//        }
//    }


//    // Удаляем из дерева элементы, которых больше нет в таблицах компилятора
//    foreach( QTreeWidgetItem* modItem, treeWidget->findItems(QString(), Qt::MatchContains) )
//    {
//        bool thisModuleDoesntExist = true;
//        for(int i=0; i<=modules->lastModuleId(); ++i)
//        {
//            module = modules->module(i)->Name();
//            if (modules->module(i)->isEnabled() && module!="internal" && module!="system" && module!="Robot" && module!="Draw" && module!="Files")
//            {
//                if (module=="@main")
//                    moduleVisName = tr("User algorithms");
//                else
//                    moduleVisName = module;
//                if (modItem->text(0) == moduleVisName)
//                {
//                    thisModuleDoesntExist = false;
//                    functions = modules->module(i)->Functions();
//                    symbols = modules->module(i)->Values();
//                    break;
//                }
//            }
//        }
//        if (thisModuleDoesntExist)
//        {
//            delete treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(modItem));
//        }
//        else
//        {
//            for(int i=0; i<modItem->childCount(); )
//            {
//                QTreeWidgetItem* funcItem = modItem->child(i);
//                bool thisFunctionDoesntExist = true;
//                for(int j=-1; j<functions->size(); ++j)
//                {
//                    if (j==-1)
//                        function = "global";
//                    else
//                        function = functions->nameById(j);
//                    if (function=="global")
//                        functionVisName = tr("Global vars");
//                    else if (function.isEmpty())
//                        functionVisName = tr("Main alg");
//                    else
//                        functionVisName = function;
//                    if (funcItem->text(0) == functionVisName)
//                    {
//                        thisFunctionDoesntExist = false;
//                        if (j>=0 && functions->typeById(j)==none)
//                        {
//                            funcItem->setText(1,"");
//                            funcItem->setText(2,"");
//                        }
//                        break;
//                    }
//                }

//                if (thisFunctionDoesntExist)
//                {
//                    delete modItem->takeChild(i);
//                }
//                else
//                {
//                    for(int j=0; j<funcItem->childCount(); )
//                    {
//                        name = funcItem->child(j)->text(0);
//                        if (symbols->inTable(name, function)==-1)
//                            delete funcItem->takeChild(j);
//                        else
//                            ++j;
//                    }
//                    if (/*funcItem->text(0)==tr("Global vars") &&*/ funcItem->childCount()==0)
//                        delete modItem->takeChild(i);
//                    else
//                        ++i;
//                }
//            }
//        }
//    }

//    treeWidget->sortByColumn(0, Qt::AscendingOrder);
//    QTreeWidgetItem* itemUserAlgs = NULL;
//    QList<QTreeWidgetItem*> list = treeWidget->findItems(tr("User algorithms"), Qt::MatchExactly);
//    if (!list.isEmpty())
//    {
//        if (treeWidget->indexOfTopLevelItem(list.first()) != 0)
//        {
//            treeWidget->insertTopLevelItem(0, treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(list.first())));
//            itemUserAlgs = treeWidget->topLevelItem(0);
//            itemUserAlgs->setExpanded(true);
//            for(int i=0; i<itemUserAlgs->childCount(); ++i)
//            {
//                itemUserAlgs->child(i)->setExpanded(true);
//            }
//        }
//    }

//    foreach( QTreeWidgetItem* modItem, treeWidget->findItems(QString(), Qt::MatchContains) )
//    {
//        for(int i=1; i<modItem->childCount(); ++i)
//        {
//            QTreeWidgetItem* item = modItem->child(i);
//            if (item->text(0)==tr("Global vars"))
//            {
//                modItem->insertChild(0, modItem->takeChild(modItem->indexOfChild(item)));
//                item->setExpanded(true);
//                break;
//            }
//        }
//    }

//    if (itemUserAlgs)
//    {
//        int offset=0;
//        if ( itemUserAlgs->child(0)->text(0) == tr("Global vars") )
//            offset = 1;
//        for(int i=1+offset; i<itemUserAlgs->childCount(); ++i)
//        {
//            QTreeWidgetItem* item = itemUserAlgs->child(i);
//            if (item->text(0)==tr("Main alg"))
//            {
//                itemUserAlgs->insertChild(0+offset, itemUserAlgs->takeChild(itemUserAlgs->indexOfChild(item)));
//                item->setExpanded(true);
//                break;
//            }
//        }
//    }
}

void KumirVariablesWindow::handleItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (item) {
        QVariant d = item->data(0, Qt::UserRole);
        if (d.isValid()) {
            QString link = d.toString();
            if (l_childWindows.contains(link)) {
                QWidget * w = l_childWindows[link];
                if (w->isVisible()) {
                    w->raise();
                }
                else {
                    w->show();
                }
            }
            else {
//                QStringList pair = link.split(".");
//                int modId = pair.at(0).toInt();
//                int varId = pair.at(1).toInt();
//                KumSingleModule * module = modules->module(modId);
//                symbol_table * table = module->Values();
//                PeremType ptype = table->getTypeByID(varId);
//                if (ptype==mass_integer ||
//                        ptype==mass_real ||
//                        ptype==mass_bool ||
//                        ptype==mass_charect ||
//                        ptype==mass_string)
//                {
//                    createSeparateTableViewer(modId, varId);
//                }
            }
        }
    }
}

void KumirVariablesWindow::handleItemHovered(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    label->setText("");
    if (item) {
        QVariant d = item->data(0, Qt::UserRole);
        if (d.isValid()) {
            QString link = d.toString();
            QStringList pair = link.split(".");
            int modId = pair.at(0).toInt();
            int varId = pair.at(1).toInt();
//            KumSingleModule * module = modules->module(modId);
//            if (module) {
//                symbol_table * table = module->Values();
//                PeremType ptype = table->getTypeByID(varId);
//                if (ptype==mass_integer ||
//                        ptype==mass_real ||
//                        ptype==mass_bool ||
//                        ptype==mass_charect ||
//                        ptype==mass_string)
//                {
//                    //					label->setText(tr("Double click to show table \"%1\" in separate window").arg(table->symb_table[varId].name));
//                }
//            }
        }
    }
}


void KumirVariablesWindow::handleTableViewerClosed()
{
    QString keyName;
    QWidget * w = qobject_cast<QWidget*>(sender());
    for (int i=0; i<l_childWindows.keys().size() ; i++) {
        if (l_childWindows[l_childWindows.keys().at(i)]==w) {
            keyName = l_childWindows.keys().at(i);
            break;
        }
    }
    if (!keyName.isEmpty()) {
        if (l_childWindows.contains(keyName)) {
            l_childWindows[keyName]->deleteLater();
            l_childWindows.remove(keyName);
        }
        if (l_tables.contains(keyName)) {
            l_tables[keyName]->deleteLater();
            l_tables.remove(keyName);
        }
    }
}

void KumirVariablesWindow::createSeparateTableViewer(int moduleId, int variableId)
{
//    TableViewer * viewer = new TableViewer(modules, moduleId, variableId);
//    SecondaryWindow * w = new SecondaryWindow(viewer, tr("Table View"));
//    w->setWindowIcon(QIcon(":/icons/table_window.png"));
//    KumSingleModule * module = modules->module(moduleId);
//    symbol_table * st = module->Values();
//    const QString name = st->symb_table[variableId].name;
//    viewer->setWindowTitle(tr("Watch table \"%1\"").arg(name));
//    l_childWindows[QString("%1.%2").arg(moduleId).arg(variableId)] = w;
//    l_tables[QString("%1.%2").arg(moduleId).arg(variableId)] = viewer;
//    connect (w, SIGNAL(shown()), viewer, SLOT(updateContent()));
//    connect (w, SIGNAL(closed()), this, SLOT(handleTableViewerClosed()));
//    w->show();
}


void KumirVariablesWindow::showEvent(QShowEvent *event)
{
//    treeWidget->setColumnWidth(0, app()->settings->value("DebugWindow/w0",200).toInt());
//    treeWidget->setColumnWidth(1, app()->settings->value("DebugWindow/w1",70).toInt());
//    treeWidget->setColumnWidth(2, app()->settings->value("DebugWindow/w2",100).toInt());
    refreshVariables();
    QWidget::showEvent(event);
}

void KumirVariablesWindow::closeEvent ( QCloseEvent * event )
{
//    app()->settings->setValue("DebugWindow/w0",treeWidget->columnWidth(0));
//    app()->settings->setValue("DebugWindow/w1",treeWidget->columnWidth(1));
//    app()->settings->setValue("DebugWindow/w2",treeWidget->columnWidth(2));
    QWidget::closeEvent(event);
}

void KumirVariablesWindow::reset()
{
//    QStringList toRemove;
//    foreach (const QString &key, l_tables.keys()) {
//        TableViewer * viewer = l_tables[key];
//        int oldModId = viewer->moduleId();
//        int oldVarId = viewer->variableId();
//        bool keepIt = false;
//        KumSingleModule * module = modules->module(oldModId);
//        if (module) {
//            symbol_table * table = module->Values();
//            if (table->symb_table.size() > oldVarId) {
//                PeremType ptype = table->symb_table[oldVarId].type;
//                if (ptype==mass_integer||
//                        ptype==mass_real||
//                        ptype==mass_bool||
//                        ptype==mass_charect||
//                        ptype==mass_string)
//                {
//                    keepIt = true;
//                }
//            }
//        }
//        if (!keepIt) {
//            toRemove << key;
//        }
//    }
//    foreach (const QString &key, toRemove) {
//        TableViewer * viewer = l_tables[key];
//        SecondaryWindow * window = l_childWindows[key];
//        viewer->deleteLater();
//        window->deleteLater();
//        l_tables.remove(key);
//        l_childWindows.remove(key);
//    }
}

}
