#ifndef KUMIRCODERUN_VARIANT_H
#define KUMIRCODERUN_VARIANT_H

#include <QtCore>
#include "dataformats/bc_tableelem.h"
#include <wchar.h>
#include <string>

using namespace Bytecode;

namespace KumirCodeRun {


class Variant
{
public:
    inline explicit Variant() { create(); }

    inline void create()
    {
        l_referenceIndeces[3] = 0;
        l_bounds[6] = 0;
        l_restrictedBounds[6] = 0;
        m_value = QVariant::Invalid;
        i_dimension = 0;
        e_baseType = VT_void;
        m_reference = 0;
    }

    static bool ignoreUndefinedError;

    inline explicit Variant(int v) { create() ; e_baseType = VT_int; m_value = v; }
    inline explicit Variant(double v) { create(); e_baseType = VT_float; m_value = v; }
    inline explicit Variant(wchar_t v) { create(); e_baseType = VT_char; m_value = QChar(v); }
    inline explicit Variant(const QChar & v) { create(); e_baseType = VT_char; m_value = v; }
    inline explicit Variant(const QString & v) { create(); e_baseType = VT_string; m_value = v; }
    inline explicit Variant(const std::wstring & v) { create(); e_baseType = VT_string; m_value = QString::fromStdWString(v); }
    inline explicit Variant(bool v) { create(); e_baseType = VT_bool; m_value = v; }
    inline explicit Variant(Variant * ref) { create(); m_reference = ref; }

    void init();
    inline quint8 dimension() const { return i_dimension; }
    inline void setDimension(quint8 v) { i_dimension = v; }
    inline void setName(const QString & n) {
        s_name = n.toStdWString();
    }
    inline QString name() const {
        if(m_reference)
            return m_reference->name();
        else
            return QString::fromStdWString(s_name);
    }
    inline QString myName() const {
        return QString::fromStdWString(s_name);
    }
    inline void setAlgorhitmName(const QString & n) {
        s_algorhitmName = n.toStdWString();
    }
    inline QString algorhitmName() const {
        if(m_reference)
            return m_reference->algorhitmName();
        else
            return QString::fromStdWString(s_algorhitmName);
    }

    void setBounds(const QList<int> & bounds);
    void updateBounds(const QList<int> & bounds);
    QList<int> bounds() const;
    QList<int> effectiveBounds() const;

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
    inline void setReference(Variant * r, const QList<int> & effectiveBounds) {
        m_reference = r;
        l_bounds[6] = l_restrictedBounds[6] = effectiveBounds.size();
        for (int i=0; i<effectiveBounds.size(); i++) {
            l_bounds[i] = l_restrictedBounds[i] = effectiveBounds[i];
        }
    }
    inline Variant * reference() { return m_reference; }

    inline void setReferenceIndeces(const QList<int> &v) {
        for (int i=0; i<v.size(); i++) {
            l_referenceIndeces[i] = v[i];
        }
        l_referenceIndeces[3] = v.size();
    }
    inline QList<int> referenceIndeces() const {
        QList<int> result;
        for (int i=0; i<l_referenceIndeces[3]; i++) {
            result << l_referenceIndeces[i];
        }
        return result;
    }

    inline int toInt() const { return value().toInt(); }
    inline double toReal() const { return value().toDouble(); }
    inline bool toBool() const { return value().toBool(); }
    inline QChar toQChar() const { return value().toChar(); }
    inline wchar_t toChar() const { return static_cast<wchar_t>(toQChar().unicode()); }
    inline std::wstring toString() const { return toQString().toStdWString(); }
    QString toQString() const;
    QString toQString(const QList<int> & indeces) const;
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
    int l_bounds[7];
    int l_restrictedBounds[7];
    ValueType e_baseType;
    Variant * m_reference;
    int l_referenceIndeces[4];
    std::wstring s_name;
    std::wstring s_algorhitmName;
};

typedef QList<Variant> VariantList;

} // namespace KumirCodeRun

Q_DECLARE_METATYPE(KumirCodeRun::Variant)
Q_DECLARE_METATYPE(KumirCodeRun::VariantList)

#endif // KUMIRCODERUN_VARIANT_H
