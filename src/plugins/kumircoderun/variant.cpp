#include "variant.h"

namespace KumirCodeRun {

QString Variant::error = "";

QVariant Variant::value() const
{
    if (m_reference && l_referenceIndeces.isEmpty())
        return m_reference->value();
    else if (m_reference && l_referenceIndeces.size()>0)
        return m_reference->value(l_referenceIndeces);
    else {
        return m_value;
    }
}

void Variant::setValue(const QVariant &v)
{
    if (m_reference && l_referenceIndeces.isEmpty())
        m_reference->setValue(v);
    else if (m_reference && l_referenceIndeces.size()>0)
        m_reference->setValue(l_referenceIndeces, v);
    else {
        m_value = v;
    }
}

QVariant Variant::value(int index0) const
{
    if (m_reference)
        return m_reference->value(index0);
    if (m_value.type()==QVariant::Invalid || l_bounds.size()<1) {
        error = QObject::tr("Array not initialized", "Variant");
        return QVariant::Invalid;
    }
    if (index0<l_bounds[0].first || index0>l_bounds[0].second) {
        error = QObject::tr("Index out of range", "Variant");
        return QVariant::Invalid;
    }
    int index = linearIndex(index0);
    if (m_value.toList()[index].type()==QVariant::Invalid) {
        error = QObject::tr("Array element not defined");
        return QVariant::Invalid;
    }
    return m_value.toList()[index];
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
    default:
        result = value().toString();
        break;
    }
    return result;
}

void Variant::setValue(int index0, const QVariant &value)
{
    if (m_reference)
        m_reference->setValue(index0, value);
    if (m_value.type()==QVariant::Invalid || l_bounds.size()<1) {
        error = QObject::tr("Array not initialized", "Variant");
        return ;
    }
    if (index0<l_bounds[0].first || index0>l_bounds[0].second) {
        error = QObject::tr("Index out of range", "Variant");
        return ;
    }
    int index = linearIndex(index0);
    QVariantList list = m_value.toList();
    list[index] = value;
    m_value = list;
}

QVariant Variant::value(int index0, int index1) const
{
    if (m_reference)
        return m_reference->value(index0, index1);
    if (m_value.type()==QVariant::Invalid || l_bounds.size()<2) {
        error = QObject::tr("Array not initialized", "Variant");
        return QVariant::Invalid;
    }
    if (index0<l_bounds[0].first || index0>l_bounds[0].second || index1<l_bounds[1].first || index1>l_bounds[1].second) {
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
    if (m_reference)
        m_reference->setValue(index0, index1, value);
    if (m_value.type()==QVariant::Invalid || l_bounds.size()<2) {
        error = QObject::tr("Array not initialized", "Variant");
        return ;
    }
    if (index0<l_bounds[0].first || index0>l_bounds[0].second || index1<l_bounds[1].first || index1>l_bounds[1].second) {
        error = QObject::tr("Index out of range", "Variant");
        return ;
    }
    int index = linearIndex(index0, index1);
    m_value.toList()[index] = value;
}

QVariant Variant::value(int index0, int index1, int index2) const
{
    if (m_reference)
        return m_reference->value(index0, index1, index2);
    if (m_value.type()==QVariant::Invalid || l_bounds.size()<3) {
        error = QObject::tr("Array not initialized", "Variant");
        return QVariant::Invalid;
    }
    if (index0<l_bounds[0].first || index0>l_bounds[0].second || index1<l_bounds[1].first || index1>l_bounds[1].second|| index2<l_bounds[2].first || index2>l_bounds[2].second) {
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
    if (m_reference)
        m_reference->setValue(index0, index1, index2, value);
    if (m_value.type()==QVariant::Invalid || l_bounds.size()<3) {
        error = QObject::tr("Array not initialized", "Variant");
        return ;
    }
    if (index0<l_bounds[0].first || index0>l_bounds[0].second || index1<l_bounds[1].first || index1>l_bounds[1].second|| index2<l_bounds[2].first || index2>l_bounds[2].second) {
        error = QObject::tr("Index out of range", "Variant");
        return ;
    }
    int index = linearIndex(index0, index1, index2);
    if (m_value.toList()[index].type()==QVariant::Invalid) {
        error = QObject::tr("Array element not defined");
        return ;
    }
    m_value.toList()[index] = value;
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
    for (int i=0; i<l_bounds.size(); i++) {
        result << l_bounds[i].first;
        result << l_bounds[i].second;
    }
    return result;
}

int Variant::linearIndex(int a) const
{
    return a-l_bounds[0].first;
}

int Variant::linearIndex(int a, int b) const
{
    int size0 = l_bounds[0].second-l_bounds[0].first+1;
    return (a-l_bounds[0].first)*size0 + b-l_bounds[1].first;
}

int Variant::linearIndex(int a, int b, int c) const
{
    int size0 = l_bounds[0].second-l_bounds[0].first+1;
    int size1 = l_bounds[1].second-l_bounds[1].first+1;
    return (a-l_bounds[0].first)*size0 + (b-l_bounds[1].first)*size1 + c-l_bounds[2].first;
}

void Variant::init()
{
    if (i_dimension==0) {
        m_value = QVariant::Invalid;
    }
    else if (m_value.type()==QVariant::List) {
        for (int i=0; i<m_value.toList().size(); i++) {
            m_value.toList()[i] = QVariant::Invalid;
        }
    }
}

void Variant::setBounds(const QList<int> &bounds)
{
    if (m_reference)
        return;
    int size = 0;
    i_dimension = bounds.size()/2;
    if (i_dimension>=1) {
        size = bounds[1]-bounds[0]+1;
    }
    if (i_dimension>=2) {
        size *= bounds[3]-bounds[2]+1;
    }
    if (i_dimension>=3) {
        size *= bounds[5]-bounds[4]+1;
    }
    QVariantList data;
    if (m_value.type()==QVariant::List)
        data = m_value.toList();
    while (data.size()<size) {
        data.append(QVariant::Invalid);
    }
    m_value = data;
    l_bounds.clear();
    for (int i=0; i<i_dimension; i++) {
        QPair<int,int> p(bounds[2*i], bounds[2*i+1]);
        l_bounds << p;
    }
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
    result.l_referenceIndeces = indeces;
    return result;
}

} // namespace KumirCodeRun
