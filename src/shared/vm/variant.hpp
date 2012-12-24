#ifndef VARIANT_HPP
#define VARIANT_HPP

#ifdef DO_NOT_DECLARE_STATIC
#define DO_NOT_DECLARE_STATIC_VARIANT
#endif

#define DO_NOT_DECLARE_STATIC
#include "stdlib/kumirstdlib.hpp"
#include "vm_enums.h"





namespace VM {

using Kumir::Char;
using Kumir::String;
using Kumir::real;
using namespace Bytecode;

typedef std::deque< class AnyValue > Record;

class AnyValue
{
    friend class Variable;
public:
    inline explicit AnyValue() { __init__(); }
    inline explicit AnyValue(ValueType t) { __init__(); tp = t;  svalue = t==VT_string? new String() : 0; ivalue = 0; }
    inline explicit AnyValue(int v) { __init__(); tp = VT_int;  ivalue = v; }
    inline explicit AnyValue(real v) { __init__(); tp = VT_real;  rvalue = v; }
    inline explicit AnyValue(bool v) { __init__(); tp = VT_bool; bvalue = v; }
    inline explicit AnyValue(Char v) { __init__(); tp = VT_char; cvalue = v; }
    inline explicit AnyValue(const String & v) { tp = VT_string; svalue = new String(v); }
    inline explicit AnyValue(const Record & value) { __init__(); tp = VT_record; uvalue = value; }

    inline void operator=(ValueType t) { __init__(); tp = t;  svalue = t==VT_string? new String() : 0; }
    inline void operator=(int v) { __init__(); tp = VT_int;  ivalue = v; }
    inline void operator=(real v) { __init__(); tp = VT_real; rvalue = v; }
    inline void operator=(bool v) { __init__(); tp = VT_bool; bvalue = v; }
    inline void operator=(Char v) { __init__(); tp = VT_char; cvalue = v; }
    inline void operator=(const String & v) { __init__(); tp = VT_string; svalue = new String(v); }
    inline void operator=(const Record & value) {
        __init__();
        tp = VT_record;
        uvalue = value;
    }

    inline int toInt() const {
        if (tp==VT_bool) return bvalue? 1 : 0;
        else if (tp==VT_char) return static_cast<int>(cvalue);
        else return ivalue;
    }
    inline real toReal() const {
        if (tp==VT_bool || tp==VT_int) return static_cast<real>(toInt());
        else return rvalue;
    }
    inline bool toBool() const {
        if (tp==VT_int) return ivalue!=0;
        else if (tp==VT_real) return rvalue!=0.0;
        else return bvalue;
    }
    inline Char toChar() const {
        if (tp==VT_int) return static_cast<Char>(ivalue);
        else if (tp==VT_string && svalue && svalue->length()==1) return svalue->at(0);
        else return cvalue;
    }
    inline String toString() const {
        if (tp==VT_int) return Kumir::Converter::sprintfInt(ivalue, 10, 0, 0);
        else if (tp==VT_real) return Kumir::Converter::sprintfReal(rvalue, '.', false, 0, 0, 0);
        else if (tp==VT_bool) return bvalue? Kumir::Core::fromUtf8("да") : Kumir::Core::fromUtf8("нет");
        else if (tp==VT_char) {
            String sval;
            sval.push_back(cvalue);
            return sval;
        }
        else if (tp==VT_void) return String();
        else return *svalue;
    }
    inline const Record & toRecord() const {
        return uvalue;
    }
    inline Record & toRecord() {
        return uvalue;
    }

    template <typename T>
    inline T toUserValue() const {
        size_t fields = strlen(T::_());
        T value;
        void * ptr = &value;
        for (size_t i=0; i<fields; i++) {
            void * fieldptr = 0;
            size_t copySize = 0;
            char tp = T::_()[i];
            switch (tp) {
            case 'i': {
                int val = uvalue.at(i).toInt();
                fieldptr = &val;
                copySize = sizeof(int);
                ptr += copySize;
                memcpy(ptr, fieldptr, copySize);
                break;
            }
            case 'd': {
                double val = uvalue.at(i).toReal();
                fieldptr = &val;
                ptr += copySize;
                memcpy(ptr, fieldptr, copySize);
                break;
            }
            case 'b': {
                bool val = uvalue.at(i).toBool();
                fieldptr = &val;
                ptr += copySize;
                memcpy(ptr, fieldptr, copySize);
                break;
            }
            case 'c': {
                Char val = uvalue.at(i).toChar();
                fieldptr = &val;
                copySize = sizeof(Char);
                ptr += sizeof(Char);
                break;
            }
            case 's': {
                String val = uvalue.at(i).toString();
                fieldptr = &val;
                ptr += copySize;
                String * stringRef = reinterpret_cast<String*>(fieldptr);
                stringRef->assign(val); // copy container data
                break;
            }
            default:
                break;
            }
        }
        return value;
    }

    inline bool isValid() const { return tp!=VT_void || asize>0; }

    inline ValueType type() const { return tp; }
    inline const AnyValue & at(size_t index) const { return avalue[index]; }
    inline const AnyValue & operator[](size_t index) const { return at(index); }
    inline AnyValue & at(size_t index) { return avalue[index]; }
    inline AnyValue & operator[](size_t index) { return at(index); }

    inline size_t rawSize() const { return asize; }


protected:

    inline void resize(size_t size) {
        if (size==0) {
            if (avalue)
                free(avalue);
            avalue = 0;
            asize = 0;
        }
        else {
            if (size >= asize) {
                avalue = reinterpret_cast<AnyValue*>(realloc(avalue, size * sizeof(AnyValue)));
                for (size_t i=asize; i<size; i++)
                    avalue[i] = AnyValue();
            }
            else {
                for (size_t i=size; i<asize; i++) {
                    delete &(avalue[i]);
                }
                avalue = reinterpret_cast<AnyValue*>(realloc(avalue, size * sizeof(AnyValue)));
            }
            asize = size;
        }
    }

private:
    inline void __init__() {
        tp = VT_void;
        svalue = 0;
        ivalue = 0;
        avalue = 0;
        asize = 0;
    }

    ValueType tp;
    union {
        int ivalue;
        real rvalue;
        Char cvalue;
        bool bvalue;
    };
    Record uvalue;
    String * svalue;
    AnyValue * avalue;
    size_t asize;
};



class Variable
{
public:

    inline explicit Variable() { create(); }

    inline void create()
    {
        l_referenceIndeces[0] = l_referenceIndeces[1] = l_referenceIndeces[2] =         l_referenceIndeces[3] = 0;
        l_bounds[0] = l_bounds[1] = l_bounds[2] = l_bounds[3] = l_bounds[4] = l_bounds[5] = l_bounds[6] = 0;
        l_restrictedBounds[0] = l_restrictedBounds[1] = l_restrictedBounds[2] = l_restrictedBounds[3] = l_restrictedBounds[4] = l_restrictedBounds[5] = l_restrictedBounds[6] = 0;
        m_value = VT_void;
        i_dimension = 0;
        e_baseType = VT_void;
        m_reference = 0;
        b_constant = false;
    }

    static bool ignoreUndefinedError;

    inline explicit Variable(int v) { create() ; e_baseType = VT_int; m_value = v; }
    inline explicit Variable(double v) { create(); e_baseType = VT_real; m_value = v; }
    inline explicit Variable(Char & v) { create(); e_baseType = VT_char; m_value = v; }
    inline explicit Variable(const String & v) { create(); e_baseType = VT_string; m_value = v; }
    inline explicit Variable(bool v) { create(); e_baseType = VT_bool; m_value = v; }
    inline explicit Variable(const Record & value) {
        create();
        e_baseType = VT_record;
        m_value = value;
    }
    inline explicit Variable(Variable * ref) { create(); m_reference = ref; }
    inline explicit Variable(const AnyValue & v) { create(); e_baseType = v.type(); m_value = v; }

    inline bool isValid() const { return e_baseType!=VT_void; }

    inline bool isConstant() const { return m_reference? m_reference->isConstant() : b_constant; }
    inline void setConstantFlag(bool value) { b_constant = value; }

    inline void init();
    inline uint8_t dimension() const { return i_dimension; }
    inline void setDimension(uint8_t v) { i_dimension = v; }
    inline void setName(const String & n) {
        s_name = n;
    }
    inline String fullReferenceName() const;
    inline const String & name() const {
        if(m_reference)
            return m_reference->name();
        else
            return s_name;
    }
    inline const String & myName() const {
        return s_name;
    }
    inline void setAlgorhitmName(const String & n) {
        s_algorhitmName = n;
    }
    inline const String & algorhitmName() const {
        if(m_reference)
            return m_reference->algorhitmName();
        else
            return s_algorhitmName;
    }

    inline void setBounds(int bounds[7]);
    inline void updateBounds(int bounds[7]);
    inline void getBounds(/*out*/ int bounds[7]) const { memcpy(bounds, l_bounds, 7*sizeof(int)); }
    inline void getEffectiveBounds(/*out*/ int bounds[7]) const { memcpy(bounds, l_restrictedBounds, 7*sizeof(int)); }

    inline bool hasValue() const;
    inline bool hasValue(int indeces[4]) const;
    inline bool hasValue(int index0) const;
    inline bool hasValue(int index0, int index1) const;
    inline bool hasValue(int index0, int index1, int index2) const;

    inline AnyValue value() const;
    inline AnyValue value(int index0) const;
    inline AnyValue value(int index0, int index1) const;
    inline AnyValue value(int index0, int index1, int index2) const;

    inline AnyValue value(int indeces[4]) const;

    inline size_t rawSize() const { return m_value.rawSize(); }
    inline const AnyValue & at(size_t index) const { return m_value.avalue[index]; }
    inline const AnyValue & operator[](size_t index) const { return at(index); }
    inline AnyValue & at(size_t index) { return m_value.avalue[index]; }
    inline AnyValue & operator[](size_t index) { return at(index); }

    inline bool isReference() const { return m_reference!=0; }
    inline void setReference(Variable * r, int effectiveBounds[7]) {
        m_reference = r;
        memcpy(l_bounds, effectiveBounds, 7*sizeof(int));
        memcpy(l_restrictedBounds, effectiveBounds, 7*sizeof(int));
    }
    inline Variable * reference() { return m_reference; }

    inline void setReferenceIndeces(int v[4]) {
        memcpy(l_referenceIndeces, v, 4*sizeof(int));
    }
    inline void getReferenceIndeces(int result[4]) const {
        memcpy(result, l_referenceIndeces, 4*sizeof(int));
    }

    inline int toInt() const { return value().toInt(); }
    inline real toReal() const { return value().toReal(); }
    inline double toDouble() const { return toReal(); }
    inline bool toBool() const { return value().toBool(); }
    inline Char toChar() const { return value().toChar(); }
    inline String toString() const;
    inline String toString(int indeces[4]) const;
    inline const Record & toRecord() const { return m_value.toRecord(); }
    inline Record & toRecord() { return m_value.toRecord(); }

    inline Variable toReference();
    inline static Variable toConstReference(const AnyValue & value);
    inline Variable toReference(int indeces[4]);
    inline void setValue(const AnyValue & value);
    inline void setValue(int index0, const AnyValue & value);
    inline void setValue(int index0, int index1, const AnyValue & value);
    inline void setValue(int index0, int index1, int index2, const AnyValue & value);
    inline void setValue(int indeces[4], const AnyValue & value);

    inline void setConstValue(const Variable & ctab);

    inline ValueType baseType() const { return m_reference? m_reference->baseType() : e_baseType; }
    inline void setBaseType(ValueType vt) { e_baseType = vt; }

    inline static void unsetError() { Kumir::Core::unsetError(); }
private:
    inline size_t linearIndex(int a) const;
    inline size_t linearIndex(int a, int b) const;
    inline size_t linearIndex(int a, int b, int c) const;
    AnyValue m_value;
    uint8_t i_dimension;
    int l_bounds[7];
    int l_restrictedBounds[7];
    ValueType e_baseType;
    Variable * m_reference;
    int l_referenceIndeces[4];
    String s_name;
    String s_algorhitmName;
    bool b_constant;
};

/* ----------------------- IMPLEMENTATION ----------------------*/

bool Variable::hasValue() const
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


bool Variable::hasValue(int indeces[4]) const
{
    if (indeces[3]==1)
        return hasValue(indeces[0]);
    else if (indeces[3]==2)
        return hasValue(indeces[0], indeces[1]);
    else if (indeces[3]==3)
        return hasValue(indeces[0], indeces[1], indeces[2]);
    else
        return hasValue();
}

AnyValue Variable::value() const
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
        if (!m_value.isValid() && !ignoreUndefinedError)
            Kumir::Core::abort(Kumir::Core::fromUtf8("Нет значения у величины"));
        return m_value;
    }
    return m_value;
}

void Variable::setValue(const AnyValue &v)
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




String Variable::toString() const
{
    String result;
    switch (e_baseType)
    {
    case VT_bool:
        if (value().toBool())
            result = Kumir::Core::fromUtf8("да");
        else
            result = Kumir::Core::fromUtf8("нет");
        break;
    case VT_real:
        result = Kumir::Converter::sprintfReal(value().toReal(), '.', false, 0,0,0);
        break;
    case VT_int:
        result = Kumir::Converter::sprintfInt(value().toInt(), 10, 0, 0);
        break;
    case VT_char:
        result.push_back(value().toChar());
        break;
    case VT_string:
        result = value().toString();
        break;
    default:
        break;
    }
    return result;
}

String Variable::toString(int indeces[4]) const
{
    String result;
    switch (e_baseType)
    {
    case VT_bool:
        if (value(indeces).toBool())
            result = Kumir::Core::fromUtf8("да");
        else
            result = Kumir::Core::fromUtf8("нет");
        break;
    case VT_real:
        result = Kumir::Converter::sprintfReal(value(indeces).toReal(), '.', false, 0,0,0);
        break;
    case VT_int:
        result = Kumir::Converter::sprintfInt(value(indeces).toInt(), 10, 0, 0);
        break;
    case VT_char:
        result.push_back(value(indeces).toChar());
        break;
    case VT_string:
        result = value(indeces).toString();
        break;
    default:
        break;
    }
    return result;
}

AnyValue Variable::value(int indeces[4]) const
{
    if (indeces[3]==1)
        return value(indeces[0]);
    else if (indeces[3]==2)
        return value(indeces[0], indeces[1]);
    else if (indeces[3]==3)
        return value(indeces[0], indeces[1], indeces[2]);
    else
        return value();
}

void Variable::setValue(int indeces[4], const AnyValue &value)
{
    if (indeces[3]==1)
        setValue(indeces[0], value);
    else if (indeces[3]==2)
        setValue(indeces[0], indeces[1], value);
    else if (indeces[3]==3)
        setValue(indeces[0], indeces[1], indeces[2], value);
    else
        setValue(value);
}


size_t Variable::linearIndex(int a) const
{
    return a-l_bounds[0];
}

size_t Variable::linearIndex(int a, int b) const
{
    int size0 = l_bounds[3]-l_bounds[2]+1;
    int offset0 = (a - l_bounds[0]) * size0;
    int result = offset0 + b-l_bounds[2];
    return result;
}

size_t Variable::linearIndex(int a, int b, int c) const
{
    int size0 = l_bounds[3]-l_bounds[2]+1;
    int size1 = l_bounds[5]-l_bounds[4]+1;
    return (a-l_bounds[0])*size0*size1 + (b-l_bounds[2])*size1 + c-l_bounds[4];
}


void Variable::setConstValue(const Variable & ctab)
{
    if (isReference()) {
        m_reference->setConstValue(ctab);
        return;
    }
    const int dim = ctab.dimension();
    int cbounds[7];
    if (dim>0) {
        ctab.getBounds(cbounds);
        for (int i=0; i<dim; i++) {
            const int mysize = l_bounds[2*i+1] - l_bounds[2*i];
            const int csize  = cbounds [2*i+1] - cbounds [2*i];
            if (mysize < csize) {
                Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
                return;
            }
        }
    }
    switch (dim)
    {
    case 0: {
        setValue(ctab.value());
        break;
    }
    case 1: {
        const int cx = cbounds [0];
        const int mx = l_bounds[0];
        const int sx = cbounds [1] - cbounds [0];
        for (int x=0; x<=sx; x++) {
            setValue(mx+x, ctab.value(cx+x));
        }
        break;
    }
    case 2: {
        const int cy = cbounds [0];
        const int my = l_bounds[0];
        const int cx = cbounds [2];
        const int mx = l_bounds[2];
        const int sy = cbounds [1] - cbounds [0];
        const int sx = cbounds [3] - cbounds [2];
        for (int y=0; y<=sy; y++) {
            for (int x=0; x<=sx; x++) {
                setValue(my+y, mx+x, ctab.value(cy+y, cx+x));
            }
        }
        break;
    }
    case 3: {
        const int cz = cbounds [0];
        const int mz = l_bounds[0];
        const int cy = cbounds [2];
        const int my = l_bounds[2];
        const int cx = cbounds [4];
        const int mx = l_bounds[4];
        const int sz = cbounds [1] - cbounds [0];
        const int sy = cbounds [3] - cbounds [2];
        const int sx = cbounds [5] - cbounds [4];
        for (int z=0; z<sz; z++) {
            for (int y=0; y<=sy; y++) {
                for (int x=0; x<=sx; x++) {
                    setValue(mz+z, my+y, mx+x, ctab.value(cz+z, cy+y, cx+x));
                }
            }
        }
        break;
        }
        default: {
        break;
    }
    }
}

// DIM = 1

bool Variable::hasValue(int index0) const
{
    if (m_reference)
        return m_reference->hasValue(index0);
    if (m_value.rawSize()==0 || l_restrictedBounds[6]<1) {
        return false;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1]) {
        return false;
    }
    int index = linearIndex(index0);
    return m_value.isValid() && m_value[index].isValid();
}

AnyValue Variable::value(int index0) const
{
    if (m_reference)
        return m_reference->value(index0);
    if (m_value.rawSize()==0 || l_restrictedBounds[6]<1) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return AnyValue(VT_void);
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return AnyValue(VT_void);
    }
    int index = linearIndex(index0);
    if (m_value[index].type()==VT_void) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение элемента таблицы не определено"));
        return AnyValue(VT_void);
    }
    return m_value[index];
}

void Variable::setValue(int index0, const AnyValue &value)
{
    if (!m_reference && (m_value.rawSize()==0 || l_restrictedBounds[6]<1)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return ;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return ;
    }
    if (m_reference) {
        m_reference->setValue(index0, value);
        return;
    }
    size_t index = linearIndex(index0);
    m_value[index] = value;
}



// DIM = 2

bool Variable::hasValue(int index0, int index1) const
{
    if (m_reference)
        return m_reference->hasValue(index0, index1);
    if (m_value.rawSize()==0 || l_restrictedBounds[6]<2) {
        return false;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2]|| index1>l_restrictedBounds[3]) {
        return false;
    }
    size_t index = linearIndex(index0, index1);
    return m_value.isValid() && m_value[index].isValid();
}

AnyValue Variable::value(int index0, int index1) const
{
    if (m_reference)
        return m_reference->value(index0, index1);
    if (m_value.rawSize()==0 || l_restrictedBounds[6]<2) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return AnyValue(VT_void);
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return AnyValue(VT_void);
    }
    size_t index = linearIndex(index0, index1);
    if (m_value[index].type()==VT_void) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение элемента таблицы не определено"));
        return AnyValue(VT_void);
    }
    return m_value[index];
}

void Variable::setValue(int index0, int index1, const AnyValue &value)
{
    if (!m_reference && (m_value.rawSize()==0 || l_restrictedBounds[6]<2)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return ;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return ;
    }
    if (m_reference) {
        m_reference->setValue(index0, index1, value);
        return;
    }
    size_t index = linearIndex(index0, index1);
    m_value[index] = value;
}


// DIM = 3

bool Variable::hasValue(int index0, int index1, int index2) const
{
    if (m_reference)
        return m_reference->hasValue(index0, index1, index2);
    if (m_value.rawSize()==0 || l_restrictedBounds[6]<3) {
        return false;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2]|| index1>l_restrictedBounds[3]|| index2<l_restrictedBounds[4] || index2>l_restrictedBounds[5]) {
        return false;
    }
    size_t index = linearIndex(index0, index1, index2);
    return m_value.isValid() && m_value[index].isValid();
}

AnyValue Variable::value(int index0, int index1, int index2) const
{
    if (m_reference)
        return m_reference->value(index0, index1, index2);
    if (m_value.rawSize()==0 || l_restrictedBounds[6]<3) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return AnyValue(VT_void);
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]|| index2<l_restrictedBounds[4] || index2>l_restrictedBounds[5]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return AnyValue(VT_void);
    }
    size_t index = linearIndex(index0, index1, index2);
    if (m_value[index].type()==VT_void) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение элемента таблицы не определено"));
        return AnyValue(VT_void);
    }
    return m_value[index];
}

void Variable::setValue(int index0, int index1, int index2, const AnyValue &value)
{
    if (!m_reference && (m_value.rawSize()==0 || l_restrictedBounds[6]<3)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return ;
    }
    if (index0<l_restrictedBounds[0] || index0>l_restrictedBounds[1] || index1<l_restrictedBounds[2] || index1>l_restrictedBounds[3]|| index2<l_restrictedBounds[4] || index2>l_restrictedBounds[5]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return ;
    }
    if (m_reference) {
        m_reference->setValue(index0, index1, index2, value);
        return;
    }
    size_t index = linearIndex(index0, index1, index2);
    m_value[index] = value;
}

void Variable::init()
{
    if (m_reference) {
        m_reference->init();
        return;
    }
    if (i_dimension==0) {
        m_value = VT_void;
    }
    else {
        if (i_dimension==1) {
            for (int x=l_restrictedBounds[0]; x<=l_restrictedBounds[1]; x++) {
                size_t index = linearIndex(x);
                m_value[index] = VT_void;
            }
        }
        else if (i_dimension==2) {
            for (int y=l_restrictedBounds[0]; y<=l_restrictedBounds[1]; y++) {
                for (int x=l_restrictedBounds[2]; x<=l_restrictedBounds[3]; x++) {
                    size_t index = linearIndex(y, x);
                    m_value[index] = VT_void;
                }
            }
        }
        else if (i_dimension==3) {
            for (int z=l_restrictedBounds[0]; z<=l_restrictedBounds[1]; z++) {
                for (int y=l_restrictedBounds[2]; y<=l_restrictedBounds[3]; y++) {
                    for (int x=l_restrictedBounds[4]; x<=l_restrictedBounds[5]; x++) {
                        size_t index = linearIndex(z, y, x);
                        m_value[index] = VT_void;
                    }
                }
            }
        }

    }
}

void Variable::setBounds(int bounds[7])
{
    size_t size = 0;
    i_dimension = bounds[6]/2;
    if (i_dimension>=1) {
        size = bounds[1]-bounds[0]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (i_dimension>=2) {
        size *= bounds[3]-bounds[2]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (i_dimension>=3) {
        size *= bounds[5]-bounds[4]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }

    m_value.resize(size);

    memcpy(l_bounds, bounds, 7*sizeof(int));
    memcpy(l_restrictedBounds, l_bounds, 7*sizeof(int));
}

void Variable::updateBounds(int bounds[7])
{
    size_t size = 0;
    i_dimension = bounds[6]/2;
    if (i_dimension>=1) {
        size = bounds[1]-bounds[0]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (i_dimension>=2) {
        size *= bounds[3]-bounds[2]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (i_dimension>=3) {
        size *= bounds[5]-bounds[4]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }

    l_restrictedBounds[6] = bounds[6];

    for (int i=0; i<bounds[6]; i+=2) {
        l_restrictedBounds[i] = l_bounds[6]
                ? Kumir::Math::imax(l_bounds[i], bounds[i])
                : bounds[i];
    }
    for (int i=1; i<bounds[6]; i+=2) {
        l_restrictedBounds[i] = l_bounds[6]
                ? Kumir::Math::imin(l_bounds[i], bounds[i])
                : bounds[i];
    }

}

Variable Variable::toReference()
{
    Variable result;
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



Variable Variable::toReference(int indeces[4])
{
    Variable result;
    if (m_reference) {
        result.m_reference = m_reference;
    }
    else {
        result.m_reference = this;
    }
    memcpy(result.l_referenceIndeces, indeces, 4*sizeof(int));
    return result;
}

String Variable::fullReferenceName() const
{
    if (m_reference) {
        String result = name();
        if (l_referenceIndeces[3]) {
            result.push_back('[');
            for (size_t i=0; i<l_referenceIndeces[3]; i++) {
                result += Kumir::Converter::sprintfInt(l_referenceIndeces[i],10,0,0);
                if (i<l_referenceIndeces[3]-1)
                    result.push_back(',');
            }
            result.push_back(']');
        }
        return result;
    }
    else {
        return name();
    }
}

#ifndef DO_NOT_DECLARE_STATIC_VARIANT
bool Variable::ignoreUndefinedError = false;
#endif

}


#endif
