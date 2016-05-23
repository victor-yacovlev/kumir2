#include "kumvariablesmodel.h"

extern "C" {
#include <string.h>
}
#include <QFont>
#include <QPalette>
#include <QApplication>


namespace KumirCodeRun {

static const int MAXIMUM_SHOWN_TABLE_ITEMS_COUNT = 255;

KumVariablesModel::KumVariablesModel(
        std::shared_ptr<VM::KumirVM> vm,
        std::shared_ptr<VM::CriticalSectionLocker> mutex,
        QObject *parent)

        : QAbstractItemModel(parent)
        , _vm(vm)
        , mutex_(mutex)
{
}

void KumVariablesModel::clear()
{
    beginResetModel();
    parents_.clear();
    cache_.clear();
    endResetModel();
}

QModelIndex KumVariablesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && column > 0) {
        return QModelIndex();
    }
    QModelIndex result;
    if (!parent.isValid()) {
        result = topLevelIndex(row);
    }
    else {
        KumVariableItem * parentItem =
                static_cast<KumVariableItem*>(parent.internalPointer());
        if (parentItem->itemType() == KumVariableItem::GlobalsTable ||
                parentItem->itemType() == KumVariableItem::LocalsTable)
        {
            result = valueIndex(row, column, parentItem->table());
        }        
        else if (parentItem->itemType() == KumVariableItem::Variable ||
                 parentItem->itemType() == KumVariableItem::ArrayItem)
        {
            if (parentItem->variable()->dimension() > 0) {
                result = arrayIndex(row, column, parentItem->variable(), parentItem->arrayIndeces());
            }
        }
    }
    return result;
}

QModelIndex KumVariablesModel::topLevelIndex(int row) const
{
    mutex_->lock();
    TableOfVariables * globalsTable = _vm->getMainModuleGlobals();
    bool hasGlobals = globalsTable && globalsTable->size() > 0;
    mutex_->unlock();
    size_t globalsOffset = hasGlobals? 1u : 0u;
    KumVariableItem * result = nullptr;
    if (hasGlobals && row == 0) {
        for (int i=0; i<cache_.size(); i++) {
            KumVariableItem * item = cache_[i];
            if (item->itemType() == KumVariableItem::GlobalsTable) {
                result = item;
                break;
            }
        }
        if (result == nullptr) {
            mutex_->lock();
            result = new KumVariableItem(_vm->getMainModuleGlobals(), row);
            mutex_->unlock();
            cache_.push_back(result);
        }
    }
    else {
        const VM::Context * mainContext = nullptr;
        mutex_->lock();
        const VM::Stack<VM::Context> & stack = _vm->callStack();
        for (int i=0; i<stack.reservedSize(); i++) {
            const VM::Context & context = stack.at(i);
            if (context.type == Bytecode::EL_MAIN) {
                mainContext = &context;
                break;
            }
        }
        QString algorithmName;
        TableOfVariables * locals = nullptr;
        quint64 framePointer = 0u;
        if ( (row - globalsOffset == 0) && mainContext ) {
            algorithmName = QString::fromStdWString(mainContext->name);
            locals = &(mainContext->locals);
        }
        else {
            size_t offset = globalsOffset;
//            if (mainContext)
//                offset += 1;
            size_t index = row - offset;
            size_t counter = 0;
            for (int i=0; i<stack.size(); i++) {
                const VM::Context & context = stack.at(i);
                if (context.type == Bytecode::EL_FUNCTION) {
                    counter += 1u;
                    if (index == counter) {
                        algorithmName = QString::fromStdWString(context.name);
                        locals = &(context.locals);
                        framePointer = static_cast<quint64>(i);
                        break;
                    }
                }
            }
        }
        mutex_->unlock();
        for (int i=0; i<cache_.size(); i++) {
            KumVariableItem * item = cache_[i];
            if (KumVariableItem::LocalsTable==item->itemType() &&
                    item->framePointer()==framePointer &&
                    item->name()==algorithmName)
            {
                result = item;
                break;
            }
        }
        if (result == nullptr) {
            result = new KumVariableItem(locals, row, algorithmName);
            result->setFramePointer(framePointer);
            cache_.push_back(result);
        }
    }
    return createIndex(row, 0, result);
}

QModelIndex KumVariablesModel::valueIndex(int row, int column, TableOfVariables *table) const
{
    size_t indexInTable = size_t(row);
    mutex_->lock();
    size_t tableSize = table->size();
    mutex_->unlock();
    if (indexInTable >= tableSize) {
        return QModelIndex();
    }
    mutex_->lock();
    const VM::Variable * var = & table->at(indexInTable);
    mutex_->unlock();
    KumVariableItem * result = nullptr;
    for (int i=0; i<cache_.size(); i++) {
        KumVariableItem * item = cache_[i];
        if (item->itemType() == KumVariableItem::Variable &&
                item->variable() == var)
        {
            result = item;
            break;
        }

    }
    if (result == nullptr) {
        result = new KumVariableItem(var, row, table);
        cache_.push_back(result);
    }
    QModelIndex resultIndex;
    if (modelIndeces_.contains(result)) {
        resultIndex = modelIndeces_[result];
    }
    else {
        resultIndex = createIndex(row, column, result);
        modelIndeces_[result] = resultIndex;
    }
    return resultIndex;
}

QModelIndex KumVariablesModel::arrayIndex(
        int row, int column,
        const VM::Variable * variable,
        const QVector<int> &prevIndeces
        ) const
{
    QVector<int> newIndeces(prevIndeces.size() + 1);
    ::memcpy(newIndeces.data(), prevIndeces.constData(), prevIndeces.size() * sizeof(int));
    int bounds[7];
    mutex_->lock();
    variable->getEffectiveBounds(bounds);
    mutex_->unlock();
    int newIndex = row + bounds[2 * prevIndeces.size()];
    newIndeces.last() = newIndex;
    KumVariableItem * result = nullptr;
    for (int i=0; i<cache_.size(); i++) {
        KumVariableItem * item = cache_[i];
        if (item->itemType() == KumVariableItem::ArrayItem &&
                item->variable() == variable &&
                item->arrayIndeces() == newIndeces)
        {
            result = item;
            break;
        }
    }
    if (result == nullptr) {
        result = new KumVariableItem(variable, row, newIndeces);
        cache_.push_back(result);
    }
    QModelIndex resultIndex;
    if (modelIndeces_.contains(result)) {
        resultIndex = modelIndeces_[result];
    }
    else {
        resultIndex = createIndex(row, column, result);
        modelIndeces_[result] = resultIndex;
    }
    return resultIndex;
}

QModelIndex KumVariablesModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }
    KumVariableItem * item =
            static_cast<KumVariableItem*>(child.internalPointer());

    if (item == nullptr) {
        return QModelIndex();
    }

    if (item->itemType() == KumVariableItem::GlobalsTable ||
            item->itemType() == KumVariableItem::LocalsTable)
    {
        return QModelIndex();
    }
    KumVariableItem * result = nullptr;
    if (item->itemType() == KumVariableItem::Variable) {
        for (int i=0; i<cache_.size(); i++) {
            KumVariableItem * it = cache_[i];
            if (it->itemType() == KumVariableItem::GlobalsTable ||
                    it->itemType() == KumVariableItem::LocalsTable)
            {
                if (it->table() == item->table()) {
                    result = it;
                    break;
                }
            }
        }
    }
    else if (item->itemType() == KumVariableItem::ArrayItem) {
        QVector<int> indeces = item->arrayIndeces();
        indeces.pop_back();
        if (indeces.isEmpty()) {
            for (int i=0; i<cache_.size(); i++) {
                KumVariableItem * it = cache_[i];
                if (it->itemType() == KumVariableItem::Variable
                        && it->variable() == item->variable())
                {
                    result = it;
                    break;
                }
            }
        }
        else {
            for (int i=0; i<cache_.size(); i++) {
                KumVariableItem * it = cache_[i];
                if (it->itemType() == KumVariableItem::ArrayItem
                        && it->variable() == item->variable()
                        && it->arrayIndeces() == indeces
                        )
                {
                    result = it;
                    break;
                }
            }
        }
    }
    return createIndex(result->numberInTable(), 0, result);
}

int KumVariablesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        // top level item
        mutex_->lock();
        int topLevelItemsCount = 0;
        const VM::Stack<VM::Context> & stack = _vm->callStack();
        bool hasMain =false;
        for (size_t i=0; i<stack.size(); i++) {
            // Calculate function calls
            const VM::Context & context = stack.at(i);
            if (context.type == Bytecode::EL_FUNCTION) {
                topLevelItemsCount += 1;
            }
        }
        for (size_t i=0; i<stack.reservedSize(); i++) {
            const VM::Context & context = stack.at(i);
            if (context.type == Bytecode::EL_MAIN) {
                hasMain = true;
                break;
            }
        }
        if (hasMain) {
            topLevelItemsCount += 1;  // Always show main context
        }
        TableOfVariables * globalsTable = _vm->getMainModuleGlobals();
        if (globalsTable && globalsTable->size() > 0)
            topLevelItemsCount ++;
        mutex_->unlock();
        return topLevelItemsCount;
    }
    KumVariableItem * item =
            static_cast<KumVariableItem*>(parent.internalPointer());
    if (item->itemType() == KumVariableItem::GlobalsTable ||
            item->itemType() == KumVariableItem::LocalsTable)
    {
        mutex_->lock();
        size_t size = item->table()? item->table()->size() : 0;
        mutex_->unlock();
        return size;
    }
    else if (item->itemType() == KumVariableItem::Variable && item->hasValue()) {
        quint8 dim = item->variable()->dimension();
        if (dim > 0) {
            mutex_->lock();
            int bounds[7];
            item->variable()->getEffectiveBounds(bounds);
            mutex_->unlock();
            return bounds[1] - bounds[0] + 1;
        }
    }
    else if (item->itemType() == KumVariableItem::ArrayItem) {
        int dim = item->variable()->dimension();
        int parentDim = item->arrayIndeces().size();
        if (dim - parentDim > 0) {
            mutex_->lock();
            int bounds[7];
            item->variable()->getEffectiveBounds(bounds);
            mutex_->unlock();
            return bounds[parentDim * 2 + 1] - bounds[parentDim * 2] + 1;
        }
    }
    return 0;
}

int KumVariablesModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant KumVariablesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();  // nothing to show
    }
    else {
        KumVariableItem * item =
                static_cast<KumVariableItem*>(index.internalPointer());
        if (item == nullptr) {
            return QVariant();
        }
        if (item->itemType() == KumVariableItem::GlobalsTable) {
            if (role == Qt::DisplayRole) {
                return tr("Globals");
            }
            else if (role == Qt::FontRole) {
                QFont fnt;
                fnt.setBold(true);
                return fnt;
            }
        }
        else if (item->itemType() == KumVariableItem::LocalsTable) {
            if (role == Qt::DisplayRole) {
                QString text;
                mutex_->lock();
                if (item->name().isEmpty())
                    text = tr("Main algorithm locals");
                else
                    text = tr("Algorithm '%1' locals").arg(item->name());
                mutex_->unlock();
                return text;
            }
            else if (role == Qt::FontRole) {
                QFont fnt;
                fnt.setBold(true);
                return fnt;
            }
        }
        else if (item->itemType() == KumVariableItem::Variable) {
            if (role == Qt::DisplayRole) {
                QString text;
                mutex_->lock();

                text = item->variableTypeName() + " " +item->name();
                if (item->hasValue() && item->variable()->dimension() == 0)
                    text += " = " + item->valueRepresentation();
                else if (item->hasValue() && item->variable()->dimension() > 0)
                    text += " = " + item->arrayRepresentation();
                mutex_->unlock();
                return text;
            }
            else if (role == Qt::FontRole) {
                mutex_->lock();
                bool isReference = item->isReference();
                mutex_->unlock();
                if (isReference) {
                    QFont fnt;
                    fnt.setItalic(true);
                    return fnt;
                }
            }
        }
        else if (item->itemType() == KumVariableItem::ArrayItem) {
            if (role == Qt::DisplayRole) {
                QString text;
                mutex_->lock();
                text = item->name();
                if (item->hasValue() &&
                        item->variable()->dimension() == item->arrayIndeces().size()
                        )
                    text += " = " + item->valueRepresentation();
                else if (item->hasValue())
                    text += " = " + item->arrayRepresentation();
                mutex_->unlock();
                return text;
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags KumVariablesModel::flags(const QModelIndex & index) const
{
    static const Qt::ItemFlags enabled = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    static const Qt::ItemFlags disabled = Qt::ItemIsSelectable;
    Qt::ItemFlags result = enabled;
    if (index.isValid()) {
        KumVariableItem * item =
                static_cast<KumVariableItem*>(index.internalPointer());
        if ( item != nullptr &&
             (item->itemType() == KumVariableItem::Variable ||
                item->itemType() == KumVariableItem::ArrayItem)
           )
        {
            mutex_->lock();
            if (!item->hasValue()) {
                result = disabled;
            }
            int dim = item->variable()->dimension();
            if (dim > 0 &&
                    item->itemType() == KumVariableItem::ArrayItem &&
                    item->arrayIndeces().size() < dim
                    )
            {
                result = enabled;
            }
            mutex_->unlock();
        }
    }
    return result;
}

void KumVariablesModel::emitValueChanged(
        const VM::Variable &variable,
        const QVector<int> &indeces
        )
{
    QModelIndex modelIndex;
    KumVariableItem * item = nullptr;
    if (indeces.size() == 0) {
        for (int i=0; i<cache_.size(); i++) {
            KumVariableItem * it = cache_[i];
            if (it->itemType() == KumVariableItem::Variable
                    && it->variable() == &variable)
            {
                item = it;
                break;
            }
        }
    }
    else {
        for (int i=0; i<cache_.size(); i++) {
            KumVariableItem * it = cache_[i];
            if (it->itemType() == KumVariableItem::ArrayItem
                    && it->variable() == &variable
                    && it->arrayIndeces() == indeces
                    )
            {
                item = it;
                break;
            }
        }
    }
    if (item && modelIndeces_.contains(item)) {
        modelIndex = modelIndeces_[item];
    }
    if (modelIndex.isValid()) {
        emit dataChanged(modelIndex, modelIndex);
    }
    if (indeces.size() > 0) {
        QVector<int> lessIndeces(indeces.size() - 1);
        ::memcpy(lessIndeces.data(), indeces.constData(), (indeces.size()-1) * sizeof(int));
        emitValueChanged(variable, lessIndeces);
    }
}

QString KumVariableItem::name() const
{
    QString result;
    if (_type == Variable) {
        result = QString::fromStdWString(_variable->myName());
        if (_variable->name() == _variable->algorhitmName()) {
            result = QString::fromUtf8("знач");
        }
        if (_variable->dimension() > 0) {
            int bounds[7];
            result += "[";
            _variable->getEffectiveBounds(bounds);
            for (uint d=0; d<_variable->dimension(); d++) {
                if (d > 0) {
                    result += ",";
                }
                if (_variable->hasValue()) {
                    int left = bounds[d*2];
                    int right = bounds[d*2+1];
                    result += QString::number(left)
                            + ":"
                            + QString::number(right);
                }
                else {
                    result += "?:?";
                }
            }
            result += "]";
        }
    }
    else if (_type == ArrayItem) {
        result = QString::fromStdWString(_variable->myName());
        result += "[";
        for (int i=0; i<_indeces.size(); i++) {
            if (i > 0)
                result += ",";
            result += QString::number(_indeces[i]);
        }
        result += "]";
    }
    else if (_type == LocalsTable) {
        result = _algorithmName;
    }
    return result;
}

QString KumVariableItem::variableTypeName() const
{
    QString result;
    if (_variable) {
        if (_variable->baseType() == VM::VT_record) {
            result = QString::fromStdWString(_variable->recordClassLocalizedName());
        }
        else if (_variable->baseType() == VM::VT_int) {
            result = QString::fromUtf8("цел");
        }
        else if (_variable->baseType() == VM::VT_real) {
            result = QString::fromUtf8("вещ");
        }
        else if (_variable->baseType() == VM::VT_bool) {
            result = QString::fromUtf8("лог");
        }
        else if (_variable->baseType() == VM::VT_char) {
            result = QString::fromUtf8("сим");
        }
        else if (_variable->baseType() == VM::VT_string) {
            result = QString::fromUtf8("лит");
        }
        if (_variable->dimension() > 0) {
            result += QString::fromUtf8("таб");
        }
    }
    return result;
}

QString KumVariableItem::array1Representation(
        const QVector<int> & indeces,
        int maxItems,
        int & readItems) const
{
    QString result = "{";
    readItems = 0;
    int bounds[7];
    _variable->getEffectiveBounds(bounds);
    int ind[4];
    ind[3] = _variable->dimension();
    int indexToVary = indeces.size();
    if (indeces.size() > 0)
        ::memcpy(ind, indeces.constData(), indeces.size() * sizeof(int));
    int start = bounds[2 * indexToVary];
    int end = bounds[2 * indexToVary + 1];
    for (int i=start; i<=end; i++) {
        if (i > start)
            result += ", ";
        ind[indexToVary] = i;
        if (_variable->hasValue(ind)) {
            QString itm = QString::fromStdWString(_variable->toString(ind));
            if (_variable->baseType() == Bytecode::VT_string) {
                itm.push_back('"');
                itm.push_front('"');
            }
            else if (_variable->baseType() == Bytecode::VT_char) {
                itm.push_back('\'');
                itm.push_front('\'');
            }
            result += itm;
        }
        else if (result.length() == 1) {
            result += " ";
        }
        readItems += 1;
        if (readItems >= maxItems)
            break;
    }
    if (readItems < (end - start + 1)) {
        result += "...";
    }
    else {
        result += "}";
    }
    return result;
}

QString KumVariableItem::array2Representation(
        const QVector<int> & indeces,
        int maxItems,
        int & readItems) const
{
    QString result = "{";
    readItems = 0;
    int bounds[7];
    _variable->getEffectiveBounds(bounds);
    int indexToVary = indeces.size();
    int start = bounds[2 * indexToVary];
    int end = bounds[2 * indexToVary + 1];
    QVector<int> ind(indeces.size() + 1);
    if (indeces.size() > 0)
        ::memcpy(ind.data(), indeces.constData(), indeces.size() * sizeof(int));
    for (int i=start; i<=end; i++) {
        ind[indexToVary] = i;
        const int blockMaxItems = maxItems - readItems;
        int blockReadItems = 0;
        if (i > start)
            result += ", ";
        result += array1Representation(ind, blockMaxItems, blockReadItems);
        readItems += blockReadItems;
        if (result.endsWith("...") || readItems >= maxItems)
            break;
    }
    if (!result.endsWith("...")) {
        result += "}";
    }
    return result;
}

QString KumVariableItem::array3Representation(
        const QVector<int> & indeces,
        int maxItems,
        int & readItems) const
{
    QString result = "{";
    readItems = 0;
    int bounds[7];
    _variable->getEffectiveBounds(bounds);
    int indexToVary = indeces.size();
    int start = bounds[2 * indexToVary];
    int end = bounds[2 * indexToVary + 1];
    QVector<int> ind(indeces.size() + 1);
    if (indeces.size() > 0)
        ::memcpy(ind.data(), indeces.constData(), indeces.size() * sizeof(int));
    for (int i=start; i<=end; i++) {
        ind[indexToVary] = i;
        const int blockMaxItems = maxItems - readItems;
        int blockReadItems = 0;
        if (i > start)
            result += ", ";
        result += array2Representation(ind, blockMaxItems, blockReadItems);
        readItems += blockReadItems;
        if (result.endsWith("...") || readItems >= maxItems)
            break;
    }
    if (!result.endsWith("...")) {
        result += "}";
    }
    return result;
}

QString KumVariableItem::arrayRepresentation() const
{
    QString result;
    int dim = _variable->dimension();
    int realDim = dim - _indeces.size();
    int dummy;
    if (realDim == 1) {
        result = array1Representation(_indeces, MAXIMUM_SHOWN_TABLE_ITEMS_COUNT, dummy);
    }
    else if (realDim == 2) {
        result = array2Representation(_indeces, MAXIMUM_SHOWN_TABLE_ITEMS_COUNT, dummy);
    }
    else if (realDim == 3) {
        result = array3Representation(_indeces, MAXIMUM_SHOWN_TABLE_ITEMS_COUNT, dummy);
    }
    return result;
}


QString KumVariableItem::valueRepresentation() const
{
    QString result;
    if (_type == Variable && hasValue()) {
        if (_variable->dimension() == 0) {
            result = QString::fromStdWString(_variable->toString());
            if (_variable->baseType() == Bytecode::VT_string) {
                result.push_back('"');
                result.push_front('"');
            }
            else if (_variable->baseType() == Bytecode::VT_char) {
                result.push_back('\'');
                result.push_front('\'');
            }
        }
    }
    else if (_type == ArrayItem && hasValue()) {
        int ind[4];
        ::memcpy(ind, _indeces.data(), _indeces.size() * sizeof(int));
        ind[3] = _indeces.size();
        result += QString::fromStdWString(_variable->toString(ind));
        if (_variable->baseType() == Bytecode::VT_string) {
            result.push_back('"');
            result.push_front('"');
        }
        else if (_variable->baseType() == Bytecode::VT_char) {
            result.push_back('\'');
            result.push_front('\'');
        }
    }
    return result;
}

bool KumVariableItem::hasValue() const
{
    if (_type == Variable) {
        return _variable->hasValue();
    }
    else if (_type == ArrayItem) {
        int ind[4];
        ind[3] = _indeces.size();
        memcpy(ind, _indeces.data(), _indeces.size() * sizeof(int));
        return _variable->hasValue(ind);
    }
    else {
        return false;
    }
}

bool KumVariableItem::isReference() const
{
    if (_variable) {
        return _variable->isReference();
    }
    else {
        return false;
    }
}

KumVariableItem::KumVariableItem(TableOfVariables *table, int row)
    : _type(GlobalsTable)
    , _variable(nullptr)
    , _table(table)
    , _tableNumber(row)
    , _framePointer(0)
{
}

KumVariableItem::KumVariableItem(TableOfVariables *table, int row, const QString &name)
    : _type(LocalsTable)
    , _variable(nullptr)
    , _table(table)
    , _tableNumber(row)
    , _algorithmName(name)
    , _framePointer(0)
{
}

KumVariableItem::KumVariableItem(const VM::Variable *variable, int row,
                                 TableOfVariables * table)
    : _type(Variable)
    , _variable(variable)
    , _table(table)
    , _tableNumber(row)
    , _framePointer(0)
{
}

KumVariableItem::KumVariableItem(const VM::Variable *variable, int row,
                                 const QVector<int> &indeces)
    : _type(ArrayItem)
    , _variable(variable)
    , _table(nullptr)
    , _tableNumber(row)
    , _indeces(indeces)
    , _framePointer(0)
{
}

} // namespace KumirCodeRun
