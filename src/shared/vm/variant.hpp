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

struct Record {
    std::vector<class AnyValue> fields;
};

class AnyValue
{
    friend class Variable;
public:
    inline explicit AnyValue(): svalue_(0), avalue_(0), uvalue_(0) { __init__(); }
    inline AnyValue(const AnyValue & other): svalue_(0), avalue_(0), uvalue_(0) {
        __init__();
        type_ = other.type_;
        if (other.svalue_) {
            svalue_ = new String(*(other.svalue_));
        }
        if (other.uvalue_) {
            uvalue_ = new Record(*(other.uvalue_));
        }
        if (other.avalue_) {
            avalue_ = new std::vector<class AnyValue>(*(other.avalue_));
        }
        if (type_==VT_int)
            ivalue_ = other.ivalue_;
        if (type_==VT_real)
            rvalue_ = other.rvalue_;
        if (type_==VT_bool)
            bvalue_ = other.bvalue_;
        if (type_==VT_char)
            cvalue_ = other.cvalue_;
    }

    inline explicit AnyValue(ValueType t): svalue_(0), avalue_(0), uvalue_(0) { __init__(); type_ = t;  svalue_ = t==VT_string? new String() : 0; ivalue_ = 0; }
    inline explicit AnyValue(int v): svalue_(0), avalue_(0), uvalue_(0) {
        __init__();
        type_ = VT_int;
        ivalue_ = v;
    }
    inline explicit AnyValue(real v): svalue_(0), avalue_(0), uvalue_(0) { __init__(); type_ = VT_real;  rvalue_ = v; }
    inline explicit AnyValue(bool v): svalue_(0), avalue_(0), uvalue_(0) { __init__(); type_ = VT_bool; bvalue_ = v; }
    inline explicit AnyValue(Char v): svalue_(0), avalue_(0), uvalue_(0) { __init__(); type_ = VT_char; cvalue_ = v; }
    inline explicit AnyValue(const String & v): svalue_(0), avalue_(0), uvalue_(0) { __init__(); type_ = VT_string; svalue_ = new String(v); }
    inline explicit AnyValue(const Record & value): svalue_(0), avalue_(0), uvalue_(0) {
        __init__();
        type_ = VT_record;
        uvalue_ = new Record(value);
    }

    inline void operator=(ValueType t) { __init__(); type_ = t;  svalue_ = t==VT_string? new String() : 0; }
    inline void operator=(int v) { __init__(); type_ = VT_int;  ivalue_ = v; }
    inline void operator=(real v) { __init__(); type_ = VT_real; rvalue_ = v; }
    inline void operator=(bool v) { __init__(); type_ = VT_bool; bvalue_ = v; }
    inline void operator=(Char v) { __init__(); type_ = VT_char; cvalue_ = v; }
    inline void operator=(const String & v) { __init__(); type_ = VT_string; svalue_ = new String(v); }
    inline void operator=(const Record & value) {
        __init__();
        type_ = VT_record;
        uvalue_ = new Record(value);
    }
    inline void operator=(const AnyValue &other) {
        __init__();
        type_ = other.type_;
        if (other.svalue_) {
            svalue_ = new String(*(other.svalue_));
        }
        if (other.uvalue_) {
            uvalue_ = new Record(*(other.uvalue_));
        }
        if (other.avalue_) {
            avalue_ = new std::vector<class AnyValue>(*(other.avalue_));
        }
        if (type_==VT_int)
            ivalue_ = other.ivalue_;
        if (type_==VT_real)
            rvalue_ = other.rvalue_;
        if (type_==VT_bool)
            bvalue_ = other.bvalue_;
        if (type_==VT_char)
            cvalue_ = other.cvalue_;
    }

    inline int toInt() const {
        if (type_==VT_bool) return bvalue_? 1 : 0;
        else if (type_==VT_char) return static_cast<int>(cvalue_);
        else return ivalue_;
    }
    inline real toReal() const {
        if (type_==VT_bool || type_==VT_int) return static_cast<real>(toInt());
        else return rvalue_;
    }
    inline bool toBool() const {
        if (type_==VT_int) return ivalue_ > 0;
        else if (type_==VT_real) return rvalue_ > 0.0;
        else if (type_==VT_char) return cvalue_ != '\0';
        else if (type_==VT_string) return svalue_ && svalue_->length() > 0;
        else return bvalue_;
    }
    inline Char toChar() const {
        if (type_==VT_int) return static_cast<Char>(ivalue_);
        else if (type_==VT_string && svalue_ && svalue_->length()==1) return svalue_->at(0);
        else return cvalue_;
    }
    inline String toString() const {
        if (type_==VT_int) return Kumir::Converter::sprintfInt(ivalue_, 10, 0, 0);
        else if (type_==VT_real) return Kumir::Converter::sprintfReal(rvalue_, '.', false, 0, -1, 0);
        else if (type_==VT_bool) return bvalue_? Kumir::Core::fromUtf8("да") : Kumir::Core::fromUtf8("нет");
        else if (type_==VT_char) {
            String sval;
            sval.push_back(cvalue_);
            return sval;
        }
        else if (type_==VT_void) return String();
        else if (svalue_)
            return String(*svalue_);
        else
            return String();
    }
    inline const Record & toRecord() const {
        return *uvalue_;
    }
    inline Record & toRecord() {
        return *uvalue_;
    }


    inline bool isValid() const { return type_!=VT_void || ( avalue_ && avalue_->size()>0 ); }

    inline ValueType type() const { return type_; }
    inline const AnyValue & at(size_t index) const { return avalue_->at(index); }
    inline const AnyValue & operator[](size_t index) const { return at(index); }
    inline AnyValue & at(size_t index) { return avalue_->at(index); }
    inline AnyValue & operator[](size_t index) { return at(index); }

    inline size_t rawSize() const { return avalue_? avalue_->size() : 0; }
    inline ~AnyValue() {
        if (svalue_)
            delete svalue_;
        if (avalue_) {
            avalue_->clear();
            delete avalue_;
        }
        if (uvalue_) {
            delete uvalue_;
        }
    }


protected:

    inline void resize(size_t size) {
        if (!avalue_)
            avalue_ = new std::vector<class AnyValue>(size);
        if (size==0) {
            if (avalue_->size())
                avalue_->clear();
        }
        else {
            if (size != avalue_->size()) {
                size_t asize = avalue_->size();
                avalue_->resize(size);
            }
        }
    }

private:
    inline void __init__() {
        if (avalue_) {
            avalue_->clear();
            delete avalue_;
        }
        if (svalue_) {
            delete svalue_;
        }
        if (uvalue_) {
            delete uvalue_;
        }
        type_ = VT_void;
        svalue_ = 0;
        ivalue_ = 0;
        uvalue_ = 0;
        avalue_ = 0;
    }

    ValueType type_;
    union {
        int ivalue_;
        real rvalue_;
        Char cvalue_;
        bool bvalue_;
    };
    String * svalue_;
    std::vector<class AnyValue> * avalue_;
    Record * uvalue_;
};



class Variable
{
public:

    inline explicit Variable() { create(); }

    inline void create()
    {
        referenceStackContextNo_ = -2;
        referenceIndeces_[0] = referenceIndeces_[1] = referenceIndeces_[2] =         referenceIndeces_[3] = 0;
        bounds_[0] = bounds_[1] = bounds_[2] = bounds_[3] = bounds_[4] = bounds_[5] = bounds_[6] = 0;
        restrictedBounds_[0] = restrictedBounds_[1] = restrictedBounds_[2] = restrictedBounds_[3] = restrictedBounds_[4] = restrictedBounds_[5] = restrictedBounds_[6] = 0;
        value_ = VT_void;
        dimension_ = 0;
        baseType_ = VT_void;
        reference_ = 0;
        constant_ = false;
    }

    static bool ignoreUndefinedError;

    inline explicit Variable(int v) { create() ; baseType_ = VT_int; value_ = v; }
    inline explicit Variable(double v) { create(); baseType_ = VT_real; value_ = v; }
    inline explicit Variable(Char & v) { create(); baseType_ = VT_char; value_ = v; }
    inline explicit Variable(const String & v) { create(); baseType_ = VT_string; value_ = v; }
    inline explicit Variable(bool v) { create(); baseType_ = VT_bool; value_ = v; }
    inline explicit Variable(const Record & value, const String & clazz, const std::string & asciiClazz) {
        create();
        baseType_ = VT_record;
        value_ = value;
        setRecordClassLocalizedName(clazz);
        setRecordClassAsciiName(asciiClazz);
    }
    inline explicit Variable(Variable * ref) { create(); reference_ = ref; }
    inline explicit Variable(const AnyValue & v) {
        create();
        baseType_ = v.type();
        value_ = v;
    }

    inline bool isValid() const {
        return reference_
                ? reference_->isValid()
                : value_.type()!=VT_void || dimension_>0;
    }

    inline bool isConstant() const { return reference_? reference_->isConstant() : constant_; }
    inline void setConstantFlag(bool value) { constant_ = value; }

    inline void init();
    inline uint8_t dimension() const { return reference_? reference_->dimension() : dimension_; }
    inline void setDimension(uint8_t v) { dimension_ = v; }
    inline void setName(const String & n) {
        name_ = n;
    }
    inline String fullReferenceName() const;
    inline const String & name() const {
        if(reference_)
            return reference_->name();
        else
            return name_;
    }
    inline const String & myName() const {
        return name_;
    }
    inline const String & recordModuleLocalizedName() const {
        if (reference_)
            return reference_->recordModuleLocalizedName();
        else
            return recordModuleLocalizedName_;
    }
    inline const std::string & recordModuleAsciiName() const {
        if (reference_)
            return reference_->recordModuleAsciiName();
        else
            return recordModuleAsciiName_;
    }
    inline const std::string & recordClassAsciiName() const {
        if (reference_)
            return reference_->recordClassAsciiName();
        else
            return recordClassAsciiName_;
    }
    inline const String & recordClassLocalizedName() const {
        if (reference_)
            return reference_->recordClassLocalizedName();
        else
            return recordClassLocalizedName_;
    }
    inline void setRecordModuleLocalizedName(const String & n) {
        if (reference_)
            reference_->setRecordModuleLocalizedName(n);
        else
            recordModuleLocalizedName_ = n;
    }
    inline void setRecordModuleAsciiName(const std::string & n) {
        if (reference_)
            reference_->setRecordModuleAsciiName(n);
        else
            recordModuleAsciiName_ = n;
    }
    inline void setRecordClassAsciiName(const std::string & n) {
        if (reference_)
            reference_->setRecordClassAsciiName(n);
        else
            recordClassAsciiName_ = n;
    }
    inline void setRecordClassLocalizedName(const String & n) {
        if (reference_)
            reference_->setRecordClassLocalizedName(n);
        else
            recordClassLocalizedName_ = n;
    }

    inline void setAlgorhitmName(const String & n) {
        algorhitmName_ = n;
    }
    inline void setModuleName(const String & n) {
        moduleName_ = n;
    }
    inline const String & moduleName() const {
        return moduleName_;
    }

    inline const String & algorhitmName() const {
        if(reference_)
            return reference_->algorhitmName();
        else
            return algorhitmName_;
    }

    inline void setBounds(int bounds[7]);
    inline void updateBounds(int bounds[7]);
    inline void getBounds(/*out*/ int bounds[7]) const { memcpy(bounds, bounds_, 7*sizeof(int)); }
    inline void getEffectiveBounds(/*out*/ int bounds[7]) const { memcpy(bounds, restrictedBounds_, 7*sizeof(int)); }

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

    inline size_t rawSize() const { return value_.rawSize(); }
    inline const AnyValue & at(size_t index) const { return value_.avalue_->at(index); }
    inline const AnyValue & operator[](size_t index) const { return at(index); }
    inline AnyValue & at(size_t index) { return value_.avalue_->at(index); }
    inline AnyValue & operator[](size_t index) { return at(index); }

    inline bool isReference() const { return reference_!=0; }
    inline void setReference(Variable * r, int effectiveBounds[7]) {
        reference_ = r;
        memcpy(bounds_, effectiveBounds, 7*sizeof(int));
        memcpy(restrictedBounds_, effectiveBounds, 7*sizeof(int));
    }
    inline Variable * reference() { return reference_; }

    inline void setReferenceIndeces(int v[4]) {
        memcpy(referenceIndeces_, v, 4*sizeof(int));
    }
    inline void getReferenceIndeces(int result[4]) const {
        memcpy(result, referenceIndeces_, 4*sizeof(int));
    }

    inline int toInt() const { return value().toInt(); }
    inline real toReal() const { return value().toReal(); }
    inline double toDouble() const { return toReal(); }
    inline bool toBool() const { return value().toBool(); }
    inline Char toChar() const { return value().toChar(); }
    inline String toString() const;
    inline String toString(int indeces[4]) const;
    inline const Record toRecord() const {
        if (reference_) {
            const Record result = reference_->toRecord();
            return result;
        }
        else {
            return value_.toRecord();
        }
    }
    inline Record & toRecord() {
        if (reference_) {
            Record & result = reference_->toRecord();
            return result;
        }
        else {
            Record & result = value_.toRecord();
            return result;
        }
    }

    inline Variable toReference();
    inline static Variable toConstReference(const AnyValue & value);

    inline int referenceStackContextNo() const { return referenceStackContextNo_; }
    inline void setReferenceStackContextNo(int v) { referenceStackContextNo_ = v; }

    inline Variable toReference(int indeces[4]);
    inline void setValue(const AnyValue & value);
    inline void setValue(int index0, const AnyValue & value);
    inline void setValue(int index0, int index1, const AnyValue & value);
    inline void setValue(int index0, int index1, int index2, const AnyValue & value);
    inline void setValue(int indeces[4], const AnyValue & value);

    inline void setConstValue(const Variable & ctab);

    inline ValueType baseType() const { return reference_? reference_->baseType() : baseType_; }
    inline void setBaseType(ValueType vt) { baseType_ = vt; }

    inline static void unsetError() { Kumir::Core::unsetError(); }
private:
    inline size_t linearIndex(int a) const;
    inline size_t linearIndex(int a, int b) const;
    inline size_t linearIndex(int a, int b, int c) const;
    AnyValue value_;
    uint8_t dimension_;
    int bounds_[7];
    int restrictedBounds_[7];
    ValueType baseType_;
    Variable * reference_;
    int referenceIndeces_[4];
    String name_;
    String algorhitmName_;
    String moduleName_;
    std::string recordModuleAsciiName_;
    String recordModuleLocalizedName_;
    std::string recordClassAsciiName_;
    String recordClassLocalizedName_;
    bool constant_;
    int referenceStackContextNo_;
};

/* ----------------------- IMPLEMENTATION ----------------------*/

bool Variable::hasValue() const
{
    if (reference_) {
        if (referenceIndeces_[3]==0)
            return reference_->hasValue();
        else if (referenceIndeces_[3]==1)
            return reference_->hasValue(referenceIndeces_[0]);
        else if (referenceIndeces_[3]==2)
            return reference_->hasValue(referenceIndeces_[0], referenceIndeces_[1]);
        else
            return reference_->hasValue(referenceIndeces_[0], referenceIndeces_[1], referenceIndeces_[2]);
    }
    else
        return value_.isValid();
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
    if (reference_) {
        if (referenceIndeces_[3]==0) {
            return reference_->value();
        }
        else if (referenceIndeces_[3]==1) {
            return reference_->value(referenceIndeces_[0]);
        }
        else if (referenceIndeces_[3]==2) {
            return reference_->value(referenceIndeces_[0], referenceIndeces_[1]);
        }
        else if (referenceIndeces_[3]==3) {
            return reference_->value(referenceIndeces_[0], referenceIndeces_[1], referenceIndeces_[2]);
        }
    }
    else {
        if (!value_.isValid() && !ignoreUndefinedError)
            Kumir::Core::abort(Kumir::Core::fromUtf8("Нет значения у величины"));
        return value_;
    }
    return value_;
}

void Variable::setValue(const AnyValue &v)
{
    if (reference_) {
        if (referenceIndeces_[3]==0) {
            reference_->setValue(v);
        }
        else if (referenceIndeces_[3]==1) {
            reference_->setValue(referenceIndeces_[0], v);
        }
        else if (referenceIndeces_[3]==2) {
            reference_->setValue(referenceIndeces_[0],referenceIndeces_[1], v);
        }
        else if (referenceIndeces_[3]==3) {
            reference_->setValue(referenceIndeces_[0],referenceIndeces_[1],referenceIndeces_[2], v);
        }

    }
    else {
        value_ = v;
    }

}




String Variable::toString() const
{
    String result;
    switch (baseType_)
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
    switch (baseType_)
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
    return a-bounds_[0];
}

size_t Variable::linearIndex(int a, int b) const
{
    int size0 = bounds_[3]-bounds_[2]+1;
    int offset0 = (a - bounds_[0]) * size0;
    int result = offset0 + b-bounds_[2];
    return result;
}

size_t Variable::linearIndex(int a, int b, int c) const
{
    int size0 = bounds_[3]-bounds_[2]+1;
    int size1 = bounds_[5]-bounds_[4]+1;
    return (a-bounds_[0])*size0*size1 + (b-bounds_[2])*size1 + c-bounds_[4];
}


void Variable::setConstValue(const Variable & ctab)
{
    if (isReference()) {
        reference_->setConstValue(ctab);
        return;
    }
    const int dim = ctab.dimension();
    int cbounds[7];
    if (dim>0) {
        ctab.getBounds(cbounds);
        for (int i=0; i<dim; i++) {
            const int mysize = bounds_[2*i+1] - bounds_[2*i];
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
        const int mx = bounds_[0];
        const int sx = cbounds [1] - cbounds [0];
        for (int x=0; x<=sx; x++) {
            setValue(mx+x, ctab.value(cx+x));
        }
        break;
    }
    case 2: {
        const int cy = cbounds [0];
        const int my = bounds_[0];
        const int cx = cbounds [2];
        const int mx = bounds_[2];
        const int sy = cbounds [1] - cbounds [0];
        const int sx = cbounds [3] - cbounds [2];
        for (int y=0; y<=sy; y++) {
            for (int x=0; x<=sx; x++) {
                setValue(my+y, mx+x, ctab.value(cy+y, cx+x));
                unsetError();
            }
        }
        break;
    }
    case 3: {
        const int cz = cbounds [0];
        const int mz = bounds_[0];
        const int cy = cbounds [2];
        const int my = bounds_[2];
        const int cx = cbounds [4];
        const int mx = bounds_[4];
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
    if (reference_)
        return reference_->hasValue(index0);
    if (value_.rawSize()==0 || restrictedBounds_[6]<1) {
        return false;
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1]) {
        return false;
    }
    int index = linearIndex(index0);
    return value_.isValid() && value_[index].isValid();
}

AnyValue Variable::value(int index0) const
{
    if (reference_)
        return reference_->value(index0);
    if (value_.rawSize()==0 || restrictedBounds_[6]<1) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return AnyValue(VT_void);
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return AnyValue(VT_void);
    }
    int index = linearIndex(index0);
    if (value_[index].type()==VT_void) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение элемента таблицы не определено"));
        return AnyValue(VT_void);
    }
    return value_[index];
}

void Variable::setValue(int index0, const AnyValue &value)
{
    if (!reference_ && (value_.rawSize()==0 || restrictedBounds_[6]<1)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return ;
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return ;
    }
    if (reference_) {
        reference_->setValue(index0, value);
        return;
    }
    size_t index = linearIndex(index0);
    value_[index] = value;
}



// DIM = 2

bool Variable::hasValue(int index0, int index1) const
{
    if (reference_)
        return reference_->hasValue(index0, index1);
    if (value_.rawSize()==0 || restrictedBounds_[6]<2) {
        return false;
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1] || index1<restrictedBounds_[2]|| index1>restrictedBounds_[3]) {
        return false;
    }
    size_t index = linearIndex(index0, index1);
    return value_.isValid() && value_[index].isValid();
}

AnyValue Variable::value(int index0, int index1) const
{
    if (reference_)
        return reference_->value(index0, index1);
    if (value_.rawSize()==0 || restrictedBounds_[6]<2) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return AnyValue(VT_void);
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1] || index1<restrictedBounds_[2] || index1>restrictedBounds_[3]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return AnyValue(VT_void);
    }
    size_t index = linearIndex(index0, index1);
    if (value_[index].type()==VT_void) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение элемента таблицы не определено"));
        return AnyValue(VT_void);
    }
    return value_[index];
}

void Variable::setValue(int index0, int index1, const AnyValue &value)
{
    if (!reference_ && (value_.rawSize()==0 || restrictedBounds_[6]<2)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return ;
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1] || index1<restrictedBounds_[2] || index1>restrictedBounds_[3]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return ;
    }
    if (reference_) {
        reference_->setValue(index0, index1, value);
        return;
    }
    size_t index = linearIndex(index0, index1);
    value_[index] = value;
}


// DIM = 3

bool Variable::hasValue(int index0, int index1, int index2) const
{
    if (reference_)
        return reference_->hasValue(index0, index1, index2);
    if (value_.rawSize()==0 || restrictedBounds_[6]<3) {
        return false;
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1] || index1<restrictedBounds_[2]|| index1>restrictedBounds_[3]|| index2<restrictedBounds_[4] || index2>restrictedBounds_[5]) {
        return false;
    }
    size_t index = linearIndex(index0, index1, index2);
    return value_.isValid() && value_[index].isValid();
}

AnyValue Variable::value(int index0, int index1, int index2) const
{
    if (reference_)
        return reference_->value(index0, index1, index2);
    if (value_.rawSize()==0 || restrictedBounds_[6]<3) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return AnyValue(VT_void);
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1] || index1<restrictedBounds_[2] || index1>restrictedBounds_[3]|| index2<restrictedBounds_[4] || index2>restrictedBounds_[5]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return AnyValue(VT_void);
    }
    size_t index = linearIndex(index0, index1, index2);
    if (value_[index].type()==VT_void) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение элемента таблицы не определено"));
        return AnyValue(VT_void);
    }
    return value_[index];
}

void Variable::setValue(int index0, int index1, int index2, const AnyValue &value)
{
    if (!reference_ && (value_.rawSize()==0 || restrictedBounds_[6]<3)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Таблица не инициализирована"));
        return ;
    }
    if (index0<restrictedBounds_[0] || index0>restrictedBounds_[1] || index1<restrictedBounds_[2] || index1>restrictedBounds_[3]|| index2<restrictedBounds_[4] || index2>restrictedBounds_[5]) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        return ;
    }
    if (reference_) {
        reference_->setValue(index0, index1, index2, value);
        return;
    }
    size_t index = linearIndex(index0, index1, index2);
    value_[index] = value;
}

void Variable::init()
{
    if (reference_) {
        reference_->init();
        return;
    }
    if (dimension_==0) {
        value_ = VT_void;
    }
    else {
        if (dimension_==1) {
            for (int x=restrictedBounds_[0]; x<=restrictedBounds_[1]; x++) {
                size_t index = linearIndex(x);
                value_[index] = VT_void;
            }
        }
        else if (dimension_==2) {
            for (int y=restrictedBounds_[0]; y<=restrictedBounds_[1]; y++) {
                for (int x=restrictedBounds_[2]; x<=restrictedBounds_[3]; x++) {
                    size_t index = linearIndex(y, x);
                    value_[index] = VT_void;
                }
            }
        }
        else if (dimension_==3) {
            for (int z=restrictedBounds_[0]; z<=restrictedBounds_[1]; z++) {
                for (int y=restrictedBounds_[2]; y<=restrictedBounds_[3]; y++) {
                    for (int x=restrictedBounds_[4]; x<=restrictedBounds_[5]; x++) {
                        size_t index = linearIndex(z, y, x);
                        value_[index] = VT_void;
                    }
                }
            }
        }

    }
}

void Variable::setBounds(int bounds[7])
{
    size_t size = 0;
    dimension_ = bounds[6]/2;
    if (dimension_>=1) {
        size = bounds[1]-bounds[0]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (dimension_>=2) {
        size *= bounds[3]-bounds[2]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (dimension_>=3) {
        size *= bounds[5]-bounds[4]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }

    value_.resize(size);

    memcpy(bounds_, bounds, 7*sizeof(int));
    memcpy(restrictedBounds_, bounds_, 7*sizeof(int));
}

void Variable::updateBounds(int bounds[7])
{
    size_t size = 0;
    dimension_ = bounds[6]/2;
    if (dimension_>=1) {
        size = bounds[1]-bounds[0]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (dimension_>=2) {
        size *= bounds[3]-bounds[2]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }
    if (dimension_>=3) {
        size *= bounds[5]-bounds[4]+1;
        if (size<=0) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
            return;
        }
    }

    restrictedBounds_[6] = bounds[6];

    for (int i=0; i<bounds[6]; i+=2) {
        restrictedBounds_[i] = bounds_[6]
                ? Kumir::Math::imax(bounds_[i], bounds[i])
                : bounds[i];
    }
    for (int i=1; i<bounds[6]; i+=2) {
        restrictedBounds_[i] = bounds_[6]
                ? Kumir::Math::imin(bounds_[i], bounds[i])
                : bounds[i];
    }

}

Variable Variable::toReference()
{
    Variable result;
    if (reference_) {
        result.reference_ = reference_;
    }
    else {
        result.reference_ = this;
    }
    memcpy(result.bounds_, restrictedBounds_, 7*sizeof(int));
    memcpy(result.restrictedBounds_, restrictedBounds_, 7*sizeof(int));
    return result;
}



Variable Variable::toReference(int indeces[4])
{
    Variable result;
    if (reference_) {
        result.reference_ = reference_;
    }
    else {
        result.reference_ = this;
    }
    memcpy(result.referenceIndeces_, indeces, 4*sizeof(int));
    return result;
}

String Variable::fullReferenceName() const
{
    if (reference_) {
        String result = name();
        if (referenceIndeces_[3]) {
            result.push_back('[');
            for (size_t i=0; i<referenceIndeces_[3]; i++) {
                result += Kumir::Converter::sprintfInt(referenceIndeces_[i],10,0,0);
                if (i<referenceIndeces_[3]-1)
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
