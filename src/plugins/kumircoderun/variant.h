#ifndef KUMIRCODERUN_VARIANT_H
#define KUMIRCODERUN_VARIANT_H

#include <QtCore>
#include "bytecode/tableelem.h"

using namespace Bytecode;

namespace KumirCodeRun {

class Variant
{
public:
    inline explicit Variant()
        { m_value = QVariant::Invalid; i_dimension = 0; e_baseType = VT_void; m_reference = 0; }
    inline explicit Variant(int v)
        { m_value = v; i_dimension = 0; e_baseType = VT_int; m_reference = 0; }
    inline explicit Variant(double v)
        { m_value = v; i_dimension = 0; e_baseType = VT_float; m_reference = 0; }
    inline explicit Variant(const QChar & v)
        { m_value = v; i_dimension = 0; e_baseType = VT_char; m_reference = 0; }
    inline explicit Variant(const QString & v)
        { m_value = v; i_dimension = 0; e_baseType = VT_string; m_reference = 0; }
    inline explicit Variant(bool v)
        { m_value = v; i_dimension = 0; e_baseType = VT_bool; m_reference = 0; }
    inline explicit Variant(Variant * ref)
        { m_value = QVariant::Invalid; i_dimension = 0; e_baseType = VT_void; m_reference = ref; }

    void init();
    inline quint8 dimension() const { return i_dimension; }
    inline void setDimension(quint8 v) { i_dimension = v; }
    inline void setName(const QString & n) { s_name = n; }
    inline QString name() const { if(m_reference) return m_reference->name(); else return s_name; }

    void setBounds(const QList<int> & bounds);
    QList<int> bounds() const;

    bool hasValue() const;
    bool hasValue(const QList<int> & indeces) const;
    bool hasValue(int index0) const;
    bool hasValue(int index0, int index1) const;
    bool hasValue(int index0, int index1, int index2) const;

    QVariant value() const;
    QVariant value(int index0) const;
    QVariant value(int index0, int index1) const;
    QVariant value(int index0, int index1, int index2) const;

    QVariant value(const QList<int> & indeces) const;

    inline bool isReference() const { return m_reference!=0; }
    inline void setReference(Variant * r) { m_reference = r; }
    inline Variant * reference() { return m_reference; }

    inline void setReferenceIndeces(const QList<int> &v) { l_referenceIndeces = v; }
    inline QList<int> referenceIndeces() const { return l_referenceIndeces; }

    inline int toInt() const { return value().toInt(); }
    inline double toReal() const { return value().toDouble(); }
    inline bool toBool() const { return value().toBool(); }
    inline QChar toChar() const { return value().toChar(); }
    QString toString() const;
    QString toString(const QList<int> & indeces) const;
    Variant toReference();
    Variant toReference(const QList<int> & indeces);
    void setValue(const QVariant & value);
    void setValue(int index0, const QVariant & value);
    void setValue(int index0, int index1, const QVariant & value);
    void setValue(int index0, int index1, int index2, const QVariant & value);
    void setValue(const QList<int> & indeces, const QVariant & value);
    static QString error;

    inline ValueType baseType() const { return e_baseType; }
    inline void setBaseType(ValueType vt) { e_baseType = vt; }

private:
    int linearIndex(int a) const;
    int linearIndex(int a, int b) const;
    int linearIndex(int a, int b, int c) const;
    QVariant m_value;
    quint8 i_dimension;
    QList< QPair<int,int> > l_bounds;
    ValueType e_baseType;
    Variant * m_reference;
    QList<int> l_referenceIndeces;
    QString s_name;

};

typedef QList<Variant> VariantList;

} // namespace KumirCodeRun

Q_DECLARE_METATYPE(KumirCodeRun::Variant)
Q_DECLARE_METATYPE(KumirCodeRun::VariantList)

#endif // KUMIRCODERUN_VARIANT_H
