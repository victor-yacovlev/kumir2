#include "variant.h"

namespace KumirCodeRun {

QString Variant::error = "";
bool Variant::ignoreUndefinedError = false;

bool Variant::hasValue() const
{
    if (m_reference) {
        if (l_referenceIndeces[3]==0)
            return m_reference->hasValue();
        else if (l_referenceIndeces[3]==1)
            return m_reference->hasValue(l_referenceIndeces[0]);
        else if (l_referenceIndeces[3]==2)
            return m_reference->hasValue(l_referenceIndeces[0], l_referenceIndeces[1]);
        else
            return m_reference->hasValue(l_referenceIndeces[0], l_referenceIndeces[1], l_referenceIndeces[2]);
    }
    else
        return m_value.isValid();
}


bool Variant::hasValue(const QList<int> & indeces) const
{
    if (indeces.size()==1)
        return hasValue(indeces[0]);
    else if (indeces.size()==2)
        return hasValue(indeces[0], indeces[1]);
    else if (indeces.size()==3)
        return hasValue(indeces[0], indeces[1], indeces[2]);
    else
        return hasValue();
}

QVariant Variant::value() const
{
    if (m_reference) {
        if (l_referenceIndeces[3]==0) {
            return m_reference->value();
        }
        else if (l_referenceIndeces[3]==1) {
            return m_reference->value(l_referenceIndeces[0]);
        }
        else if (l_referenceIndeces[3]==2) {
            return m_reference->value(l_referenceIndeces[0], l_referenceIndeces[1]);
        }
        else if (l_referenceIndeces[3]==3) {
            return m_reference->value(l_referenceIndeces[0], l_referenceIndeces[1], l_referenceIndeces[2]);
        }
    }
    else {
        if (m_value==QVariant::Invalid && !ignoreUndefinedError)
            error = QObject::tr("Variable not initialized", "Variant");
        return m_value;
    }
}

void Variant::setValue(const QVariant &v)
{
    if (m_reference) {
        if (l_referenceIndeces[3]==0) {
            m_reference->setValue(v);
        }
        else if (l_referenceIndeces[3]==1) {
            m_reference->setValue(l_referenceIndeces[0], v);
        }
        else if (l_referenceIndeces[3]==2) {
            m_reference->setValue(l_referenceIndeces[0],l_referenceIndeces[1], v);
        }
        else if (l_referenceIndeces[3]==3) {
            m_reference->setValue(l_referenceIndeces[0],l_referenceIndeces[1],l_referenceIndeces[2], v);
        }

    }
    else {
        m_value = v;
    }

}


QString Variant::toString() const
{
    QString result;
    switch (e_baseType)
    {
    case VT_bool:
        if (value().toBool())
            result = QObject::tr("true", "Variant");
        else
            result = QObject::tr("false", "Variant");
        break;
    case VT_float:
        result = QString::number(value().toDouble(), 'g', 7);
        if (!result.contains("."))
            result += ".0";
        break;
    default:
        result = value().toString();
        break;
    }
    return result;
}

QString Variant::toString(const QList<int> & indeces) const
{
    QString result;
    switch (e_baseType)
    {
    case VT_bool:
        if (value(indeces).toBool())
            result = QObject::tr("true", "Variant");
        else
            result = QObject::tr("false", "Variant");
        break;
    case VT_float:
        result = QString::number(value(indeces).toDouble(), 'g', 7);
        if (!result.contains("."))
            result += ".0";
        break;
    default:
        result = value(indeces).toString();
        break;
    }
    return result;
}

QVariant Variant::value(const QList<int> & indeces) const
{
    if (indeces.size()==1)
        return value(indeces[0]);
    else if (indeces.size()==2)
        return value(indeces[0], indeces[1]);
    else if (indeces.size()==3)
        return value(indeces[0], indeces[1], indeces[2]);
    else
        return value();
}

void Variant::setValue(const QList<int> &indeces, const QVariant &value)
{
    if (indeces.size()==1)
        setValue(indeces[0], value);
    else if (indeces.size()==2)
        setValue(indeces[0], indeces[1], value);
    else if (indeces.size()==3)
        setValue(indeces[0], indeces[1], indeces[2], value);
    else
        setValue(value);
}

QList<int> Variant::bounds() const
{
    QList<int> result;
    for (int i=0; i<l_bounds[6]; i++) {
        result << l_bounds[i];
    }
    return result;
}

QList<int> Variant::effectiveBounds() const
{
    QList<int> result;
    for (int i=0; i<l_restrictedBounds[6]; i++) {
        result << l_restrictedBounds[i];
    }
    return result;
}

int Variant::linearIndex(int a) const
{
    return a-l_bounds[0];
}

int Variant::linearIndex(int a, int b) const
{
    int size0 = l_bounds[3]-l_bounds[2]+1;
    int offset0 = (a - l_bounds[0]) * size0;
    int result = offset0 + b-l_bounds[2];
    return result;
}

int Variant::linearIndex(int a, int b, int c) const
{
    int size0 = l_bounds[3]-l_bounds[2]+1;
    int size1 = l_bounds[5]-l_bounds[4]+1;
    return (a-l_bounds[0])*size0*size1 + (b-l_bounds[2])*size1 + c-l_bounds[4];
}

// DIM = 1

bool Variant::hasValue(int index0) const
{
    if (m_reference)
        return m_reference->hasValue(index0);
    if (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<1) {
        return false;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1]) {
        return false;
    }
    int index = linearIndex(index0);
    return m_value.isValid() && m_value.toList()[index].isValid();
}

QVariant Variant::value(int index0) const
{
    if (m_reference)
        return m_reference->value(index0);
    if (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<1) {
        error = QObject::tr("Array not initialized", "Variant");
        return QVariant::Invalid;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1]) {
        static const QString err = QObject::tr("Index out of range", "Variant");
        error = err;
        return QVariant::Invalid;
    }
    int index = linearIndex(index0);
    if (m_value.toList()[index].type()==QVariant::Invalid) {
        error = QObject::tr("Array element not defined");
        return QVariant::Invalid;
    }
    return m_value.toList()[index];
}

void Variant::setValue(int index0, const QVariant &value)
{
    if (!m_reference && (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<1)) {
        error = QObject::tr("Array not initialized", "Variant");
        return ;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1]) {
        error = QObject::tr("Index out of range", "Variant");
        return ;
    }
    if (m_reference) {
        m_reference->setValue(index0, value);
        return;
    }
    int index = linearIndex(index0);
    QVariantList list = m_value.toList();
    list[index] = value;
    m_value = list;
}

// DIM = 2

bool Variant::hasValue(int index0, int index1) const
{
    if (m_reference)
        return m_reference->hasValue(index0, index1);
    if (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<2) {
        return false;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2]|| index1>l_restrictedBounds[3]) {
        return false;
    }
    int index = linearIndex(index0, index1);
    return m_value.isValid() && m_value.toList()[index].isValid();
}

QVariant Variant::value(int index0, int index1) const
{
    if (m_reference)
        return m_reference->value(index0, index1);
    if (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<2) {
        error = QObject::tr("Array not initialized", "Variant");
        return QVariant::Invalid;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]) {
        error = QObject::tr("Index out of range", "Variant");
        return QVariant::Invalid;
    }
    int index = linearIndex(index0, index1);
    if (m_value.toList()[index].type()==QVariant::Invalid) {
        error = QObject::tr("Array element not defined");
        return QVariant::Invalid;
    }
    return m_value.toList()[index];
}

void Variant::setValue(int index0, int index1, const QVariant &value)
{
    if (!m_reference && (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<2)) {
        error = QObject::tr("Array not initialized", "Variant");
        return ;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]) {
        error = QObject::tr("Index out of range", "Variant");
        return ;
    }
    if (m_reference) {
        m_reference->setValue(index0, index1, value);
        return;
    }
    int index = linearIndex(index0, index1);
    QVariantList list = m_value.toList();
    list[index] = value;
    m_value = list;
}


// DIM = 3

bool Variant::hasValue(int index0, int index1, int index2) const
{
    if (m_reference)
        return m_reference->hasValue(index0, index1, index2);
    if (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<3) {
        return false;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2]|| index1>l_restrictedBounds[3]|| index2<l_restrictedBounds[4] || index2>l_restrictedBounds[5]) {
        return false;
    }
    int index = linearIndex(index0, index1, index2);
    return m_value.isValid() && m_value.toList()[index].isValid();
}

QVariant Variant::value(int index0, int index1, int index2) const
{
    if (m_reference)
        return m_reference->value(index0, index1, index2);
    if (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<3) {
        error = QObject::tr("Array not initialized", "Variant");
        return QVariant::Invalid;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]|| index2<l_restrictedBounds[4] || index2>l_restrictedBounds[5]) {
        error = QObject::tr("Index out of range", "Variant");
        return QVariant::Invalid;
    }
    int index = linearIndex(index0, index1, index2);
    if (m_value.toList()[index].type()==QVariant::Invalid) {
        error = QObject::tr("Array element not defined");
        return QVariant::Invalid;
    }
    return m_value.toList()[index];
}

void Variant::setValue(int index0, int index1, int index2, const QVariant &value)
{
    if (!m_reference && (m_value.type()==QVariant::Invalid || l_restrictedBounds[6]<3)) {
        error = QObject::tr("Array not initialized", "Variant");
        return ;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]|| index2<l_restrictedBounds[4] || index2>l_restrictedBounds[5]) {
        error = QObject::tr("Index out of range", "Variant");
        return ;
    }
    if (m_reference) {
        m_reference->setValue(index0, index1, index2, value);
        return;
    }
    int index = linearIndex(index0, index1, index2);
    QVariantList list = m_value.toList();
    list[index] = value;
    m_value = list;
}

void Variant::init()
{
    if (m_reference) {
        m_reference->init();
        return;
    }
    if (i_dimension==0) {
        m_value = QVariant::Invalid;
    }
    else if (m_value.type()==QVariant::List) {
        if (i_dimension==1) {
            for (int x=l_restrictedBounds[0]; x<=l_restrictedBounds[1]; x++) {
                int index = linearIndex(x);
                m_value.toList()[index] = QVariant::Invalid;
            }
        }
        else if (i_dimension==2) {
            for (int y=l_restrictedBounds[0]; y<=l_restrictedBounds[1]; y++) {
                for (int x=l_restrictedBounds[2]; x<=l_restrictedBounds[3]; x++) {
                    int index = linearIndex(y, x);
                    m_value.toList()[index] = QVariant::Invalid;
                }
            }
        }
        else if (i_dimension==3) {
            for (int z=l_restrictedBounds[0]; z<=l_restrictedBounds[1]; z++) {
                for (int y=l_restrictedBounds[2]; y<=l_restrictedBounds[3]; y++) {
                    for (int x=l_restrictedBounds[4]; x<=l_restrictedBounds[5]; x++) {
                        int index = linearIndex(z, y, x);
                        m_value.toList()[index] = QVariant::Invalid;
                    }
                }
            }
        }

    }
}

void Variant::setBounds(const QList<int> &bounds)
{
//    if (m_reference)
//        return;
    int size = 0;
    i_dimension = bounds.size()/2;
    if (i_dimension>=1) {
        size = bounds[1]-bounds[0]+1;
        if (size<=0) {
            error = QObject::tr("Array of negative size", "Variant");
            return;
        }
    }
    if (i_dimension>=2) {
        size *= bounds[3]-bounds[2]+1;
        if (size<=0) {
            error = QObject::tr("Array of negative size", "Variant");
            return;
        }
    }
    if (i_dimension>=3) {
        size *= bounds[5]-bounds[4]+1;
        if (size<=0) {
            error = QObject::tr("Array of negative size", "Variant");
            return;
        }
    }
    QVariantList data;
    if (m_value.type()==QVariant::List)
        data = m_value.toList();
    while (data.size()<size) {
        data.append(QVariant::Invalid);
    }
    m_value = data;
    l_bounds[6] = bounds.size();
    for (int i=0; i<bounds.size(); i++) {
        l_bounds[i] = bounds[i];
    }

//    if (m_reference) {
//        bool allEqual = true;
//        for (int i=0; i<l_bounds[6]; i++) {
//            allEqual = allEqual &&
//                    l_bounds[i]==m_reference->l_bounds[i] ;
//        }
//        if (!allEqual) {
//            error = QObject::tr("Array bounds mismatch");
//        }
//    }
    memcpy(l_restrictedBounds, l_bounds, 7*sizeof(int));
}

void Variant::updateBounds(const QList<int> &bounds)
{
//    if (m_reference)
//        return;
    int size = 0;
    i_dimension = bounds.size()/2;
    if (i_dimension>=1) {
        size = bounds[1]-bounds[0]+1;
        if (size<=0) {
            error = QObject::tr("Array of negative size", "Variant");
            return;
        }
    }
    if (i_dimension>=2) {
        size *= bounds[3]-bounds[2]+1;
        if (size<=0) {
            error = QObject::tr("Array of negative size", "Variant");
            return;
        }
    }
    if (i_dimension>=3) {
        size *= bounds[5]-bounds[4]+1;
        if (size<=0) {
            error = QObject::tr("Array of negative size", "Variant");
            return;
        }
    }
//    QVariantList data;
//    if (m_value.type()==QVariant::List)
//        data = m_value.toList();
//    while (data.size()<size) {
//        data.append(QVariant::Invalid);
//    }
//    m_value = data;
    l_restrictedBounds[6] = bounds.size();

    for (int i=0; i<bounds.size(); i+=2) {
        l_restrictedBounds[i] = l_bounds[6]
                ? qMax(l_bounds[i], bounds[i])
                : bounds[i];
    }
    for (int i=1; i<bounds.size(); i+=2) {
        l_restrictedBounds[i] = l_bounds[6]
                ? qMin(l_bounds[i], bounds[i])
                : bounds[i];
    }


//    if (m_reference) {
//        bool allEqual = true;
//        for (int i=0; i<l_bounds[6]; i++) {
//            allEqual = allEqual &&
//                    l_bounds[i]==m_reference->l_bounds[i] ;
//        }
//        if (!allEqual) {
//            error = QObject::tr("Array bounds mismatch");
//        }
//    }
}

Variant Variant::toReference()
{
    Variant result;
    if (m_reference) {
        result.m_reference = m_reference;
    }
    else {
        result.m_reference = this;
    }
    memcpy(result.l_bounds, l_restrictedBounds, 7*sizeof(int));
    memcpy(result.l_restrictedBounds, l_restrictedBounds, 7*sizeof(int));
    return result;
}

Variant Variant::toReference(const QList<int> & indeces)
{
    Variant result;
    if (m_reference) {
        result.m_reference = m_reference;
    }
    else {
        result.m_reference = this;
    }
    for (int i=0; i<indeces.size(); i++) {
        result.l_referenceIndeces[i] = indeces[i];
    }
    result.l_referenceIndeces[3] = indeces.size();

    return result;
}

} // namespace KumirCodeRun
