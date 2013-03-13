#include "debuggerwindow.h"

namespace CoreGUI {

#define ROLE_MOD_NAME 0xfa00
#define ROLE_NAME 0xfa01
#define ROLE_BASETYPE 0xfa02
#define ROLE_CUR_DIMENSION 0xfa03
#define ROLE_VAR_DIMENSION 0xfa04
#define ROLE_VALUE 0xfa04
#define ROLE_BOUNDS 0xfb00
#define ROLE_REFERENCE 0xff01
#define ROLE_REFERENCE_INDECES 0xff02
#define ROLE_BACK_REFERENCES 0xff03

DebuggerWindow::DebuggerWindow(
        Shared::RunInterface * run,
        QWidget *parent
        )
    : QTreeWidget(parent)
    , runner(run)
{
    setColumnCount(1);
    setHeaderHidden(true);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(handleItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(handleItemCollapsed(QTreeWidgetItem*)));
}

void DebuggerWindow::reset()
{
    if (topLevelItemCount()>0)
        clear();
    globals.clear();
    QTreeWidget::reset();
    update();
}

void DebuggerWindow::pushContext(
        /** context header */       const QString & contextName,
        /** variable names */         const QStringList & names,
        /** variable base types */const QStringList & baseTypes,
        /** variable dimensions */ const QList<int> & dimensions
        )
{
    QTreeWidgetItem * root = new QTreeWidgetItem(this);
    root->setText(0, contextName.isEmpty()
                  ? tr("Main algorithm")
                  : tr("Algorithm '%1'").arg(contextName));
    QFont bf = root->font(0);
    bf.setBold(true);
    root->setFont(0, bf);
    addTopLevelItem(root);

    Q_ASSERT(names.size()==baseTypes.size());
    Q_ASSERT(names.size()==dimensions.size());
    int cnt = names.size();
    for (int i=0; i<cnt; i++) {
        QString text;
        text = baseTypes[i];
        if (dimensions[i])
            text += QString::fromUtf8("таб");
        QTreeWidgetItem * varItem = new QTreeWidgetItem(root);
        text += " "+names[i];
        varItem->setText(0, text);
        varItem->setData(0, ROLE_NAME, names[i]);
        varItem->setData(0, ROLE_BASETYPE, baseTypes[i]);
        varItem->setData(0, ROLE_CUR_DIMENSION, dimensions[i]);
        varItem->setData(0, ROLE_VAR_DIMENSION, dimensions[i]);
        root->addChild(varItem);
    }
    Q_ASSERT(topLevelItemCount()>0);
    root->setExpanded(true);
    clearSelection();
    root->setSelected(true);
}

void DebuggerWindow::setGlobals(
        /** module name */           const QString & moduleName,
        /** variable names */         const QStringList & names,
        /** variable base types */const QStringList & baseTypes,
        /** variable dimensions */ const QList<int> & dimensions
        )
{
    QTreeWidgetItem * root = new QTreeWidgetItem(this);
    root->setText(0, moduleName.isEmpty()
                  ? tr("Globals")
                  : tr("Globals of '%1'").arg(moduleName));
    QFont bf = root->font(0);
    bf.setBold(true);
    root->setFont(0, bf);
    addTopLevelItem(root);

    Q_ASSERT(names.size()==baseTypes.size());
    Q_ASSERT(names.size()==dimensions.size());
    int cnt = names.size();
    for (int i=0; i<cnt; i++) {
        QString text;
        text = baseTypes[i];
        if (dimensions[i])
            text += QString::fromUtf8("таб");
        QTreeWidgetItem * varItem = new QTreeWidgetItem(root);
        text += " "+names[i];
        varItem->setText(0, text);
        varItem->setData(0, ROLE_MOD_NAME, moduleName);
        varItem->setData(0, ROLE_NAME, names[i]);
        varItem->setData(0, ROLE_BASETYPE, baseTypes[i]);
        varItem->setData(0, ROLE_CUR_DIMENSION, dimensions[i]);
        varItem->setData(0, ROLE_VAR_DIMENSION, dimensions[i]);
        root->addChild(varItem);
    }
    Q_ASSERT(topLevelItemCount()>0);
    root->setExpanded(true);
    clearSelection();
    globals.insert(moduleName, root);
}

inline void removeAllBackReference(QTreeWidgetItem * item) {
    if (item->data(0, ROLE_REFERENCE).isValid()) {
        quintptr ptr = item->data(0, ROLE_REFERENCE).toULongLong();
        QTreeWidgetItem * ref = reinterpret_cast<QTreeWidgetItem*>(ptr);
        if (ref->data(0, ROLE_BACK_REFERENCES).isValid()) {
            QVariantList alist = ref->data(0, ROLE_BACK_REFERENCES).toList();
            quintptr me = reinterpret_cast<quintptr>(item);
            alist.removeAll(me);
            ref->setData(0, ROLE_BACK_REFERENCES, alist);
        }
    }
}

void DebuggerWindow::updateAllValues()
{
    return;
    // Update variables -- it is possible that
    //  called function changed something while running
    //  not step-in mode (via out-parameters)
    int framesCount = topLevelItemCount()-globals.size();
    int framesOffset = globals.size();
    for (int i=0; i<framesCount; i++) {
        const QTreeWidgetItem * frameContext = topLevelItem(i+framesOffset);
        const QMap<QString,QVariant> locals = runner->getScalarLocalValues(i);
        for ( int k=0; k<frameContext->childCount(); k++ ) {
            QTreeWidgetItem * var = frameContext->child(k);
            const QString varName = var->data(0, ROLE_NAME).toString();
            if (locals.contains(varName) && locals[varName].isValid()) {
                const QString value = locals[varName].toString();
                var->setData(0, ROLE_VALUE, value);
                const QString text = var->data(0, ROLE_BASETYPE).toString() +
                        " " + varName + " = " + value;
                var->setText(0, text);
            }
        }
    }
}

void DebuggerWindow::popContext()
{
    Q_ASSERT(topLevelItemCount()>0);
    QTreeWidgetItem * context = takeTopLevelItem(topLevelItemCount()-1);

    // Remove all backreferences!
    for ( int i=0; i<context->childCount(); i++) {
        QTreeWidgetItem * child = context->child(i);
        removeAllBackReference(child);
    }
    delete context;

    clearSelection();
    if (topLevelItemCount()>0) {
        topLevelItem(topLevelItemCount()-1)->setExpanded(true);
        topLevelItem(topLevelItemCount()-1)->setSelected(true);
    }
}

inline QList<QTreeWidgetItem*> findAllBackReferences(QTreeWidgetItem* root)
{
    QList<QTreeWidgetItem*> result;
    if (root->data(0, ROLE_BACK_REFERENCES).isValid()) {
        QVariantList alist = root->data(0, ROLE_BACK_REFERENCES).toList();
        foreach ( const QVariant & v , alist ) {
            quintptr ptr = v.toULongLong();
            QTreeWidgetItem * item = reinterpret_cast<QTreeWidgetItem*>(ptr);
            result.push_back(item);
        }
    }
    return result;
}

void DebuggerWindow::updateLocalVariable(
        /** variable name */       const QString & name,
        /** value */              const QString & value
        )
{
    Q_ASSERT(topLevelItemCount()>0);
    QTreeWidgetItem * context = topLevelItem(topLevelItemCount()-1);
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<context->childCount(); i++) {
        if (name==context->child(i)->data(0, ROLE_NAME).toString()) {
            variable = context->child(i);
            break;
        }
    }
    Q_CHECK_PTR(variable);
    QList<QTreeWidgetItem *> allReferences = findAllBackReferences(variable);
    if (variable->data(0, ROLE_REFERENCE).isValid()) {
        quintptr ptr = variable->data(0, ROLE_REFERENCE).toULongLong();
        QTreeWidgetItem * ref = reinterpret_cast<QTreeWidgetItem*>(ptr);
        allReferences += ref;
        allReferences += findAllBackReferences(ref);
    }
    QString text;
    text = variable->data(0, ROLE_BASETYPE).toString();
    text += " "+variable->data(0, ROLE_NAME).toString();
    text += " = "+value;
    variable->setData(0, ROLE_VALUE, value);
    variable->setText(0, text);
    foreach ( QTreeWidgetItem * ref , allReferences ) {
        if (ref!=variable) {
            text = ref->data(0, ROLE_BASETYPE).toString();
            text += " "+ref->data(0, ROLE_NAME).toString();
            text += " = " + value;
            ref->setText(0, text);
        }
    }
    clearSelection();
    variable->setSelected(true);
}

void DebuggerWindow::updateGlobalVariable(
        /** module qualified name */ const QString & moduleName,
        /** variable name */       const QString & name,
        /** value */              const QString & value
        )
{
    Q_ASSERT(topLevelItemCount()>0);
    Q_ASSERT(globals.size()>0);
    Q_ASSERT(globals.contains(moduleName));
    QTreeWidgetItem * module = globals[moduleName];
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<module->childCount(); i++) {
        if (name==module->child(i)->data(0, ROLE_NAME).toString()) {
            variable = module->child(i);
            break;
        }
    }
    Q_CHECK_PTR(variable);
    QList<QTreeWidgetItem *> allReferences = findAllBackReferences(variable);
    if (variable->data(0, ROLE_REFERENCE).isValid()) {
        quintptr ptr = variable->data(0, ROLE_REFERENCE).toULongLong();
        QTreeWidgetItem * ref = reinterpret_cast<QTreeWidgetItem*>(ptr);
        allReferences += ref;
        allReferences += findAllBackReferences(ref);
    }
    QString text;
    text = variable->data(0, ROLE_BASETYPE).toString();
    text += " "+variable->data(0, ROLE_NAME).toString();
    text += " = "+value;
    variable->setData(0, ROLE_VALUE, value);
    variable->setText(0, text);
    foreach ( QTreeWidgetItem * ref , allReferences ) {
        if (ref!=variable) {
            text = ref->data(0, ROLE_BASETYPE).toString();
            text += " "+ref->data(0, ROLE_NAME).toString();
            text += " = " + value;
            ref->setText(0, text);
        }
    }
    clearSelection();
    variable->setSelected(true);
}

void DebuggerWindow::updateLocalTableValue(
        const QString &name,
        const QList<int> & indeces
        )
{
    Q_ASSERT_X(topLevelItemCount()>0, "updateLocalTableBounds", "topLevelItemCount()>0");
    QTreeWidgetItem * context = topLevelItem(topLevelItemCount()-1);
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<context->childCount(); i++) {
        if (name==context->child(i)->data(0, ROLE_NAME).toString()) {
            variable = context->child(i);
            break;
        }
    }
    Q_ASSERT_X(variable!=nullptr, "updateLocalTableBounds", "variable!=nullptr");
    setupTableValue(variable);
    clearSelection();
    QTreeWidgetItem * itemToSelect = variable;
    findExpandedTableItem(indeces, itemToSelect);
    itemToSelect->setSelected(true);
}

void DebuggerWindow::updateGlobalTableValue(
        const QString & moduleName,
        const QString &name,
        const QList<int> & indeces
        )
{
    Q_ASSERT(topLevelItemCount()>0);
    Q_ASSERT(globals.size()>0);
    Q_ASSERT(globals.contains(moduleName));
    QTreeWidgetItem * context = globals[moduleName];
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<context->childCount(); i++) {
        if (name==context->child(i)->data(0, ROLE_NAME).toString()) {
            variable = context->child(i);
            break;
        }
    }
    Q_CHECK_PTR(variable);
    setupTableValue(variable);
    clearSelection();
    QTreeWidgetItem * itemToSelect = variable;
    findExpandedTableItem(indeces, itemToSelect);
    itemToSelect->setSelected(true);
}

void DebuggerWindow::findExpandedTableItem(
        const QList<int> &indeces,
        QTreeWidgetItem *&item
        )
{
    if (!item->isExpanded() || item->childCount()==0)
        return;
    int myDim = item->data(0, ROLE_CUR_DIMENSION).toInt();
    for (int i=0; i<item->childCount(); i++) {
        QTreeWidgetItem * child = item->child(i);
        int childDim = child->data(0, ROLE_CUR_DIMENSION).toInt();
        int varDim   = child->data(0, ROLE_VAR_DIMENSION).toInt();
        int boundIndexOffset = ROLE_BOUNDS + 2 * (varDim-myDim);
        if (childDim==myDim) {
            // child is a table slice
            int childStart = child->data(0, boundIndexOffset    ).toInt();
            int childEnd   = child->data(0, boundIndexOffset + 1).toInt();
            int thisIndex = indeces.first();
            if (childStart<=thisIndex && thisIndex<=childEnd) {
                item = child; // this slice is more precise than whole table
                findExpandedTableItem(indeces, item);
                    // try to find in a more details
                return;
            }
        }
        else if (childDim<myDim) {
            // child is a table element
            int childStart = child->data(0, boundIndexOffset    ).toInt();
            int childEnd   = child->data(0, boundIndexOffset + 1).toInt();
            Q_ASSERT(childStart==childEnd);
            int childIndex = childStart;
            int thisIndex = indeces.first();
            if (childIndex==thisIndex) {
                item = child;
                    // found distinct element (1D array) or
                    // a subtree (2D or 3D) for an element
                if (childDim>0) {
                    // if 2D or 3D array then try to find in more details
                    findExpandedTableItem(indeces.mid(1), item);
                }
                return;
            }
        }
    }
}

void DebuggerWindow::updateLocalTableBounds(
        /** variable name */             const QString & name,
        /** a list of bound pairs */ const QList<int> & bounds
        )
{
    if (bounds.isEmpty())
        return;
    Q_ASSERT_X(topLevelItemCount()>0, "updateLocalTableBounds", "topLevelItemCount()>0");
    QTreeWidgetItem * context = topLevelItem(topLevelItemCount()-1);
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<context->childCount(); i++) {
        if (name==context->child(i)->data(0, ROLE_NAME).toString()) {
            variable = context->child(i);
            break;
        }
    }
    Q_ASSERT_X(variable!=nullptr, "updateLocalTableBounds", "variable!=nullptr");
    QString text;
    text = variable->data(0, ROLE_BASETYPE).toString()+QString::fromUtf8("таб");
    text += " "+variable->data(0, ROLE_NAME).toString()+"[";
    int dim = variable->data(0, ROLE_VAR_DIMENSION).toInt();
    for (int i=0; i<dim; i++) {
        text += QString::fromAscii("%1:%2")
                .arg(bounds[i*2]).arg(bounds[i*2+1]);
        if (i<dim-1)
            text+=",";
    }
    text += "]";
    for (int i=0; i<bounds.size(); i++) {
        variable->setData(0, ROLE_BOUNDS + i, bounds[i]);
    }
    setupTableValue(variable);
    clearSelection();
    variable->setSelected(true);
}

void DebuggerWindow::updateGlobalTableBounds(
        /** module name */         const QString & moduleName,
        /** variable name */             const QString & name,
        /** a list of bound pairs */ const QList<int> & bounds
        )
{
    if (bounds.isEmpty())
        return;
    Q_ASSERT(topLevelItemCount()>0);
    Q_ASSERT(globals.size()>0);
    Q_ASSERT(globals.contains(moduleName));
    QTreeWidgetItem * context = globals[moduleName];
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<context->childCount(); i++) {
        if (name==context->child(i)->data(0, ROLE_NAME).toString()) {
            variable = context->child(i);
            break;
        }
    }
    Q_CHECK_PTR(variable);
    QString text;
    text = variable->data(0, ROLE_BASETYPE).toString()+QString::fromUtf8("таб");
    text += " "+variable->data(0, ROLE_NAME).toString()+"[";
    int dim = variable->data(0, ROLE_VAR_DIMENSION).toInt();
    for (int i=0; i<dim; i++) {
        text += QString::fromAscii("%1:%2")
                .arg(bounds[i*2]).arg(bounds[i*2+1]);
        if (i<dim-1)
            text+=",";
    }
    text += "]";
    for (int i=0; i<bounds.size(); i++) {
        variable->setData(0, ROLE_BOUNDS + i, bounds[i]);
    }
    setupTableValue(variable);
    clearSelection();
    variable->setSelected(true);
}

void DebuggerWindow::valuesListToString(
        const QVariant & vals,
        int maxItems,
        int & counter,
        QString & result
        )
{
    if (vals.type()==QVariant::Invalid) {
        result += "?";
        counter += 1;
    }
    else if (vals.type()==QVariant::List) {
        result += "{ ";
        QVariantList alist = vals.toList();
        bool completed = false;
        for (int i=0; i<alist.size(); i++) {
            valuesListToString(alist[i], maxItems, counter, result);
            if (i<alist.size()-1)
                result += ", ";
            counter += 1;
            if (maxItems!=-1 && counter>=maxItems) {
                break;
            }
        }
        result += " }";
    }
    else {
        result += vals.toString();
    }
}


void DebuggerWindow::setupTableValue(QTreeWidgetItem *item)
{
    int dimension = item->data(0, ROLE_CUR_DIMENSION).toInt();
    int varDimension = item->data(0, ROLE_VAR_DIMENSION).toInt();
    if (dimension>0) {
        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }

    bool isRoot = item->parent()->parent()==nullptr;
        // nullptr ---> Scope ---> TableRoot

    int deep = 0;
    int frameNo = -1;
    QTreeWidgetItem * p = item->parent();
    bool isGlobalVariable = item->data(0, ROLE_MOD_NAME).isValid();
    QString moduleName = item->data(0, ROLE_MOD_NAME).toString();
    while (p!=nullptr) {
        deep += 1;
        if (p->parent()==nullptr) {
            for (int k=0; k<topLevelItemCount(); k++) {
                if (p==topLevelItem(k)) {
                    frameNo = k - globals.size();
                    break;
                }
            }
        }
        p = p->parent();
    }

    QString text;
    QList<int> bounds;
    QList<int> indeces;
    QList< QPair<int,int> > ranges;
    for (int i=0; i<varDimension; i++) {
        int a = item->data(0, ROLE_BOUNDS+i*2).toInt();
        int b = item->data(0, ROLE_BOUNDS+i*2+1).toInt();
        bounds.push_back(a);
        bounds.push_back(b);
        ranges.push_back(QPair<int,int>(a,b));
        if (a==b) {
            indeces.push_back(a);
        }
    }
    if (isRoot) {
        text = item->data(0, ROLE_BASETYPE).toString() +
                QString::fromUtf8("таб ") +
                item->data(0, ROLE_NAME).toString() +
                boundsToText(bounds);
    }
    else {
        text = boundsToText(bounds);
    }


    const int usedExtraSymbols =
            deep * 3;  // measured on X11 system
    QFontMetrics fm(item->font(0));
    const int usedSpace = fm.width('n')*usedExtraSymbols + fm.width(text);
    const int freeSpace = width() - usedSpace;
    const int freeSpaceSymbols = freeSpace / fm.width('n');
    const int itemsCountToShowHint = freeSpaceSymbols / 3;
    bool complete = false;
    if (dimension>0) {
        text += " = ";
        int itemsToShow = item->data(0, ROLE_BASETYPE).toString()
                .startsWith(QString::fromUtf8("лит"))
                ? 100
                : 1000; // TODO implement me!
        QVariantList valsList;
        if (isGlobalVariable) {
            valsList = runner->getGlobalTableValues(
                        moduleName,
                        itemsToShow,
                        item->data(0,ROLE_NAME).toString(),
                        ranges,
                        complete
                        );
        }
        else {
            valsList = runner->getLocalTableValues(
                        frameNo,
                        itemsToShow,
                        item->data(0,ROLE_NAME).toString(),
                        ranges,
                        complete
                        );
        }
        int counter = 0;
        QString valText;
        valuesListToString(valsList, itemsToShow, counter, valText);
        int valTextDimension = 0;
        for (int k=0; k<valText.length(); k++) {
            if (valText[k]=='{')
                valTextDimension += 1;
            else if (!valText[k].isSpace())
                break;
        }
        for (int k=0; k<valTextDimension-dimension; k++) {
            if (valText.startsWith('{'))
                valText.remove(0, 1);
            if (valText.endsWith('}'))
                valText.remove(valText.length()-1, 1);
            valText = valText.trimmed();
        }
        text += valText;
    }
    else {
        text = item->data(0,ROLE_NAME).toString() + text;
        QVariant concreteValue;
        if (isGlobalVariable) {
            concreteValue = runner->getGlobalTableValue(
                        moduleName,
                        item->data(0,ROLE_NAME).toString(),
                        indeces
                        );
        }
        else {
            concreteValue = runner->getLocalTableValue(
                        frameNo,
                        item->data(0,ROLE_NAME).toString(),
                        indeces
                        );
        }
        if (concreteValue.isValid())
            text += " = "+concreteValue.toString();
    }
    item->setText(0,text);
    if (item->isExpanded()) {
        for (int i=0; i<item->childCount(); i++) {
            setupTableValue(item->child(i));
        }
    }
}

QString DebuggerWindow::boundsToText(const QList<int> &bounds)
{
    QString result = "[";
    int dim = bounds.size()/2;
    Q_ASSERT(bounds.size()%2==0);
    for (int i=0; i<dim; i++) {
        int first = bounds[i*2];
        int second = bounds[i*2+1];
        if (first==second) {
            // exact index, not a slice
            result += QString::number(first);
        }
        else {
            result += QString::number(first)+":"+QString::number(second);
        }
        if (i<dim-1)
            result += ",";
    }
    result += "]";
    return result;
}

void DebuggerWindow::handleItemCollapsed(QTreeWidgetItem *item)
{
    bool isFrame = item->parent()==nullptr;
        // nullptr ---> Scope ---> TableRoot
    if (!isFrame) {
        while (item->childCount()>0) {
            QTreeWidgetItem * child = item->child(0);
            item->removeChild(child);
            delete child;
        }
    }
}

void DebuggerWindow::handleItemExpanded(QTreeWidgetItem *item)
{
    int dimension = item->data(0, ROLE_CUR_DIMENSION).isValid()
            ? item->data(0, ROLE_CUR_DIMENSION).toInt()
            : 0;
    const QVariant baseType = item->data(0, ROLE_BASETYPE);
    const QVariant varName  = item->data(0, ROLE_NAME);
    const QVariant modName  = item->data(0, ROLE_MOD_NAME);
    const int varDimension  = item->data(0, ROLE_VAR_DIMENSION).toInt();
    if (dimension>0) {
        QList<int> bounds;
        int boundsIndexOffset = (varDimension-dimension) * 2;
        for (int i=0; i<varDimension*2; i++) {
            bounds.push_back(item->data(0, ROLE_BOUNDS+i).toInt());
        }
        int start = bounds[boundsIndexOffset];
        int end = bounds[boundsIndexOffset+1];
        int size = end-start+1;
        int groups = size%optimalGroupSize(size)==0
                ? size/optimalGroupSize(size)
                : size/optimalGroupSize(size)+1;
        int curstart = start;
        if (groups>1) {
            for (int i=0; i<groups; i++) {
                QTreeWidgetItem * groupRoot = nullptr;
                groupRoot = new QTreeWidgetItem(item);
                QList<int> localBounds = bounds;
                localBounds[boundsIndexOffset]   = curstart;
                localBounds[boundsIndexOffset+1] = qMin(
                            end,
                            curstart+optimalGroupSize(size)-1
                            );
                groupRoot->setData(0, ROLE_CUR_DIMENSION, dimension);
                groupRoot->setData(0, ROLE_VAR_DIMENSION, varDimension);
                groupRoot->setData(0, ROLE_BASETYPE, baseType);
                groupRoot->setData(0, ROLE_NAME, varName);
                groupRoot->setData(0, ROLE_MOD_NAME, modName);
                for (int k=0; k<localBounds.size(); k++) {
                    groupRoot->setData(0, ROLE_BOUNDS+k, localBounds[k]);
                }
                item->addChild(groupRoot);
                setupTableValue(groupRoot);
                curstart += optimalGroupSize(size);
            }
        }
        else {
            for (int i=0; i<size; i++) {
                int index = start + i;
                QTreeWidgetItem * element = new QTreeWidgetItem(item);
                element->setData(0, ROLE_CUR_DIMENSION, dimension-1);
                element->setData(0, ROLE_VAR_DIMENSION, varDimension);
                element->setData(0, ROLE_BASETYPE, baseType);
                element->setData(0, ROLE_NAME, varName);
                element->setData(0, ROLE_MOD_NAME, modName);
                QList<int> localBounds = bounds;
                localBounds[boundsIndexOffset] =
                        localBounds[boundsIndexOffset+1] = index;
                for (int k=0; k<bounds.size(); k++) {
                    element->setData(0, ROLE_BOUNDS+k, localBounds[k]);
                }
                item->addChild(element);
                setupTableValue(element);
            }
        }
    }
}

int DebuggerWindow::optimalGroupSize(int size)
{
    int q = size / 10;
    return qMax(q, 10);
}

void DebuggerWindow::setLocalReference(
        /** variable name */             const QString & name,
        /** target name */         const QString & targetName,
        /** target array indeces */const QList<int> & indeces,
        /** stack frames back, or -1 for global */   int back,
        /** global value module name */ const QString & moduleName
        )
{
    Q_ASSERT(topLevelItemCount()>0);
    QTreeWidgetItem * context = topLevelItem(topLevelItemCount()-1);
    QTreeWidgetItem * variable = nullptr;
    for (int i=0; i<context->childCount(); i++) {
        if (name==context->child(i)->data(0, ROLE_NAME).toString()) {
            variable = context->child(i);
            break;
        }
    }
    Q_CHECK_PTR(variable);
    QFont fnt = variable->font(0);
    fnt.setItalic(true);
    variable->setFont(0, fnt);
    variable->setData(0, Qt::ToolTipRole,
                      tr("This variable is a reference.\nRight click to navigate target"));
    QTreeWidgetItem * targetContext  = nullptr;
    QTreeWidgetItem * targetVariable = nullptr;
    if (back<0) {
        Q_ASSERT(globals.size()>0);
        Q_ASSERT(globals.contains(moduleName));
        targetContext = globals[moduleName];
    }
    else {
        int targetFrameNo = topLevelItemCount()-1 - back;
        if (targetFrameNo>=0)
            targetContext = topLevelItem(targetFrameNo);
    }
    if (targetContext) {
        for (int i=0; i<targetContext->childCount(); i++) {
            if (targetName==targetContext->child(i)->data(0, ROLE_NAME).toString()) {
                targetVariable = targetContext->child(i);
                break;
            }
        }
    }

    if (targetVariable) {
        // Set reference and back reference to update values automatically
        QVariantList backReferenceList;
        if (targetVariable->data(0, ROLE_BACK_REFERENCES).isValid()) {
            backReferenceList = targetVariable->data(0, ROLE_BACK_REFERENCES).toList();
        }
        backReferenceList.push_back(reinterpret_cast<quintptr>(variable));
        targetVariable->setData(0, ROLE_BACK_REFERENCES, backReferenceList);
        variable->setData(0, ROLE_REFERENCE, reinterpret_cast<quintptr>(targetVariable));

        QString text;
        text = variable->data(0, ROLE_BASETYPE).toString();
        text += " "+variable->data(0, ROLE_NAME).toString();
        if (targetVariable->data(0, ROLE_VALUE).isValid()) {
            text += " = "+targetVariable->data(0, ROLE_VALUE).toString();
        }
        variable->setText(0, text);
    }
}

void DebuggerWindow::paintEvent(QPaintEvent *event)
{
    if (topLevelItemCount()>0) {
        QTreeWidget::paintEvent(event);
    }
    else {
        static const QString message =
                tr("Current values available only while running program in step-by-step mode");
        event->accept();
        QPainter p(viewport());
        QStyleOptionFrame opt;
        opt.initFrom(viewport());
        opt.rect = viewport()->rect();
        opt.palette.setCurrentColorGroup(QPalette::Disabled);
        style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, viewport());
        QTextOption textOpt;
        textOpt.setAlignment(Qt::AlignCenter);
        textOpt.setWrapMode(QTextOption::WordWrap);
        p.setPen(QPen(opt.palette.buttonText().color()));
        p.drawText(viewport()->rect().adjusted(5,5,-10,-10),
                   message,
                   textOpt
                   );


    }
}

} // namespace CoreGUI
