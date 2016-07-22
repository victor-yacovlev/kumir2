#ifndef KUMIRSTDLIB_H
#define KUMIRSTDLIB_H

#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <cstdio>
#ifndef APPLE
#ifndef USE_MINGW_TOOLCHAIN
#include <random>
#endif
#endif
#include "encodings.hpp"

#include <time.h>

#include <wchar.h>
#include <string>

#include <cmath>

#if defined(WIN32) || defined(_WIN32)
#   include <Windows.h>
#else
#   include <sys/time.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#   include <unistd.h>
#   include <string.h>
#   include <errno.h>
#endif


namespace VM { class Variable; }

namespace Kumir {

typedef wchar_t Char;
typedef std::wstring String;
inline String & operator+(String & s, const /*ascii only*/ char c) {
    Char uc = Char(c);
    s.push_back(uc);
    return s;
}
inline String & operator+(String &s1, const /*utf8*/ char * s2) {
    EncodingError encodingError;
    String us2 = Coder::decode(UTF8, std::string(s2), encodingError);
    s1.append(us2);
    return s1;
}

typedef double real;

inline String & operator+(String & s, const Char c) {
    s.push_back(c);
    return s;
}

struct FileType {
    enum OpenMode { NotOpen, Read, Write, Append };
    enum SpecialType { RegularFile, Console };
    inline static const char * _() { return "siibbl"; }
    inline FileType() { valid = false; mode = NotOpen; type = RegularFile; autoClose = false; handle = 0; }
    inline void setName(const String &name) {
        fullPath = name;
    }
    inline String getName() const {
        return fullPath;
    }
    inline void setMode(OpenMode m) {
        mode = int(m);
    }
    inline OpenMode getMode() const {
        return OpenMode(mode);
    }
    inline SpecialType getType() const {
        return SpecialType(type);
    }
    inline void setType(SpecialType t) {
        type = int(t);
    }
    inline bool isValid() const {
        return valid;
    }
    inline void invalidate() {
        valid = false;
        handle = 0;
    }
    inline bool operator==(const FileType & other) const {
        return other.fullPath==fullPath;
    }
    inline bool operator!=(const FileType & other) const {
        return other.fullPath!=fullPath;
    }

    String fullPath;
    int mode;
    int type;
    bool valid;
    bool autoClose;
    FILE* handle;
};

class AbstractInputBuffer {
public:
    virtual bool readRawChar(Char & ch) = 0;
    virtual void pushLastCharBack() = 0;
    virtual void clear() = 0;    
};

class AbstractOutputBuffer {
public:
    virtual void writeRawString(const String & ) = 0;
};

class StringList:
        public std::deque<String>
{
public:
    inline String join(Char separator) const {
        String result;
        size_t resultSize = 0;
        for (size_t i=0; i<size(); i++) {
            resultSize += at(i).length();
        }
        resultSize += size()-1;
        result.reserve(resultSize);
        for (size_t i=0; i<size(); i++) {
            result.append(at(i));
            if (i<size()-1)
                result.push_back(separator);
        }
        return result;
    }
};

class Core {
    friend class Math;
    friend class Random;
    friend class Files;
    friend class StringUtils;
    friend class System;
    friend class IO;
    friend class VM::Variable;
public:
    static void (*AbortHandler)();
    inline static void init() { error.clear(); }
    inline static void finalize() {}
    inline static const String & getError() { return error; }

    inline static String fromUtf8(const std::string & s) {
        String result;
        EncodingError encodingError;
        result = Coder::decode(UTF8, s, encodingError);
        return result;
    }

    inline static String fromAscii(const std::string & s) {
        String result;
        EncodingError encodingError;
        result = Coder::decode(ASCII, s, encodingError);
        return result;
    }

    inline static std::string toLowerCase(const std::string & s) {
        // !!!! Only for ASCII in onebyte case !!!!!
        std::string result;
        result.reserve(s.length());
        for (size_t i=0; i<s.length(); i++) {
            char ch = s[i];
            if (ch>='A' && ch<='Z')
                result.push_back(ch+32);
            else
                result.push_back(ch);
        }
        return result;
    }

    inline static std::wstring toLowerCaseW(const std::wstring & s) {
        std::wstring result;
        result.reserve(s.length());
        for (size_t i=0; i<s.length(); i++) {
            wchar_t ch = s[i];
            if (ch>=L'A' && ch<=L'Z') {
                result.push_back(ch+32);
            }
            else if (ch>=0x0400 && ch<=0x042F) {
                result.push_back(ch+0x20);
            }
            else
                result.push_back(ch);
        }
        return result;
    }

    inline static std::string toUpperCase(const std::string & s) {
        // !!!! Only for ASCII in onebyte case !!!!!
        std::string result;
        result.reserve(s.length());
        for (size_t i=0; i<s.length(); i++) {
            char ch = s[i];
            if (ch>='a' && ch<='z')
                result.push_back(ch-32);
            else
                result.push_back(ch);
        }
        return result;
    }

    inline static std::wstring toUpperCaseW(const std::wstring & s) {
        std::wstring result;
        result.reserve(s.length());
        for (size_t i=0; i<s.length(); i++) {
            wchar_t ch = s[i];
            if (ch>=L'a' && ch<=L'z') {
                result.push_back(ch-32);
            }
            else if (ch>=0x0430 && ch<=0x044F) {
                result.push_back(ch-0x20);
            }
            else
                result.push_back(ch);
        }
        return result;
    }

    inline static StringList splitString(const String & s, const Char separator, bool skipEmptyParts) {
        StringList result;
        size_t prev_index;
        prev_index = 0;
        while (true) {
            size_t cur_index = s.find(separator, prev_index);
            if (cur_index==s.npos)
                cur_index = s.length();
            size_t length = cur_index - prev_index;
            if (length==0 && !skipEmptyParts)
                result.push_back(String());
            else if (length>0) {
                result.push_back(s.substr(prev_index, length));
            }
            prev_index = cur_index+1;
            if (prev_index>=s.length())
                break;
        }
        return result;
    }
#if defined(WIN32) || defined(_WIN32)
    inline static Encoding getSystemEncoding() {
        return CP1251;
    }
#elif defined(DOS)
    inline static Encoding getSystemEncoding() {
        return CP866;
    }
#else
    inline static Encoding getSystemEncoding()
    {
        char * lc = 0;
        lc = getenv("LC_CTYPE");
        if (!lc)
            lc = getenv("LC_ALL");
        if (!lc)
            return UTF8;
        static const StringList parts = splitString(fromAscii(std::string(lc)), Char('.'), true);
        if (parts.size()==0)
            return UTF8;
        else {
            EncodingError encodingError;
            static const std::string last = Coder::encode(ASCII, parts.at(parts.size()-1), encodingError);
            if (last==std::string("KOI8-R"))
                return KOI8R;
            else if (last==std::string("CP866") ||
                     last==std::string("IBM866") ||
                     last==std::string("CP-866") ||
                     last==std::string("IBM-866"))
                return CP866;
            else if (last==std::string("CP1251") ||
                     last==std::string("WINDOWS1251") ||
                     last==std::string("CP-1251") ||
                     last==std::string("WINDOWS-1251"))
                return CP1251;
            else
                return UTF8;
        }
    }

#endif

    inline static void abort(const String & err) {
        error = err;
        if (AbortHandler) {
            AbortHandler();
        }
    }
protected:
    inline static void unsetError() {
        error.clear();
    }
    static String error;
};

class Math {
public:
    inline static void init() {}
    inline static void finalize() {}

    inline static bool checkSumm(int32_t lhs, int32_t rhs) {
        // Check for integer overflow
        // CLang completely removes the standard check technique due to optimization.
        // Use another method: cast to 64 bit
        const int64_t l = lhs;
        const int64_t r = rhs;
        const int64_t sum = l + r;
        static const int64_t Right = 2147483647LL;
        static const int64_t Left = -2147483648LL;
        bool result = sum >= Left && sum <= Right;
        return result;
    }

    inline static bool checkDiff(int32_t lhs, int32_t rhs) {
        const int64_t l = lhs;
        const int64_t r = rhs;
        const int64_t diff = l - r;
        static const int64_t Right = 2147483647LL;
        static const int64_t Left = -2147483648LL;
        bool result = diff >= Left && diff <= Right;
        return result;
    }

    inline static bool checkProd(int32_t lhs, int32_t rhs) {
        // Check for integer overflow
        int64_t prod = int64_t(lhs) * int64_t(rhs);
        return (prod >> 32)==(prod >> 31);
    }

    inline static bool isCorrectDouble(double val) {
        // !!!!!!!!!!! WARNING !!!!!!!!!!
        // this works ONLY for x86/x86_64 platform!
        double * pval = &val;
        uint64_t * pbits = reinterpret_cast<uint64_t*>(pval);
        uint64_t bits = *pbits;
        uint64_t expMask  = 0x7FF0000000000000;
        uint64_t fracMask = 0x000FFFFFFFFFFFFF;
        uint64_t exponent = (bits & expMask) >> 52;
        uint64_t fraction = bits & fracMask;
        bool Inf = (exponent==uint64_t(0x7FF)) && (fraction==uint64_t(0));
        bool NaN = (exponent==uint64_t(0x7FF)) && (fraction>uint64_t(0));
        return !Inf && !NaN;
    }


    inline static bool isCorrectReal(real val) {
        return isCorrectDouble(val);
    }

    inline static bool isCorrectIntegerConstant(const String & value) {
        size_t start = 0;
        if (value.length()<=start) return false;
        bool isNegative = value.at(0)==Char('-');
        if (isNegative) {
            start = 1;
        }
        if (value.length()<=start) return false;
        bool isHex = false;
        if (value.length()-start >= 1) {
            isHex = value.at(start)==Char('$');
            if (isHex) start += 1;
        }
        if (!isHex && value.length()-start >= 2) {
            isHex = value.at(start)==Char('0') && value.at(start+1)==Char('x');
            if (isHex) start += 2;
        }
        if (value.length()<=start) return false;
        while (start<value.length() && value.at(start)==Char('0'))
            start += 1;
        const String digits = value.substr(start);
        static const String maxHex = Core::fromAscii("80000000");
        static const String maxPositiveDecimal = Core::fromAscii("2147483647");
        static const String maxNegativeDecimal = Core::fromAscii("2147483648");
        if (isHex) {
            if (digits.length()==maxHex.length())
                return digits < maxHex;
            else
                return digits.length()<maxHex.length();
        }
        else {
            const String & maxDecimal =
                    isNegative ? maxNegativeDecimal : maxPositiveDecimal;
            if (digits.length()==maxDecimal.length())
                return digits <= maxDecimal;
            else
                return digits.length()<maxDecimal.length();
        }
    }

    inline static real abs(real x) { return ::fabs(x); }

    inline static int imax(int x, int y) { return x>y? x : y; }
    inline static int imin(int x, int y) { return x<y? x : y; }

    inline static real rmax(real x, real y) { return x>y? x : y; }
    inline static real rmin(real x, real y) { return x<y? x : y; }

    inline static int iabs(int x) { return x>0? x : -x; }
    inline static int intt(real x) {
        return static_cast<int>(::floor(x));
    }
    inline static real arccos(real x) {
        if (x>=-1.0 && x<=1.0) {
            return ::acos(x);
        }
        else {
            Core::abort(Core::fromUtf8("Неверный аргумент тригонометрической функции"));
            return 0.0;
        }
    }
    inline static real arcctg(real x) { return ::atan(1.0/x);}
    inline static real arcsin(real x) {
        if (x>=-1.0 && x<=1.0) {
            return ::asin(x);
        }
        else {
            Core::abort(Core::fromUtf8("Неверный аргумент тригонометрической функции"));
            return 0.0;
        }
    }
    inline static real arctg(real x) { return ::atan(x); }

    inline static real cos(real x) { return ::cos(x); }
    inline static real sin(real x) { return ::sin(x); }
    inline static real tg(real x) { return ::tan(x); }
    inline static real ctg(real x) {
        if (x==0.0) {
            Core::abort(Core::fromUtf8("Неверный аргумент тригонометрической функции"));
            return 0.0;
        }
        else {
            return 1.0/::tan(x);
        }
    }
    inline static real lg(real x) {
        if (x>0.0) {
            real num = ::log(x);
            real den = ::log((real)10.0);
            real result = num / den;
            return result;
        }
        else {
            Core::abort(Core::fromUtf8("Логарифм от не положительного числа"));
            return 0.0;
        }
    }
    inline static real ln(real x) {
        if (x>0.0) {
            return ::log(x);
        }
        else {
            Core::abort(Core::fromUtf8("Логарифм от не положительного числа"));
            return 0.0;
        }
    }
    inline static real exp(real x) { return ::exp(x); }

    inline static real sqrt(real x) {
        if (x>=0.0) {
            return ::sqrt(x);
        }
        else {
            Core::abort(Core::fromUtf8("Нельзя извлечь квадратный корень из отрицательного числа"));
            return 0.0;
        }
    }
    inline static real pow(real a, real b) {
        real result = ::pow(a, b);
        if (!Math::isCorrectReal(result)) {
            Core::abort(Core::fromUtf8("Ошибка возведения в степень: результат - слишком большое число"));
            return 0.0;
        }
        return result;
    }
    inline static int ipow(int a, int b) {
        real rresult = ::floor(pow(real(a), real(b)));
        if (Core::error.length()>0) return 0;
        real absval = fabs(rresult);
        real mxintval = fabs(real(maxint()));
        if (absval>mxintval)
        {
            Core::abort(Core::fromUtf8("Ошибка возведения в степень: результат - слишком большое число"));
            return 0;
        }
        int iresult = static_cast<int>(rresult);
        return iresult;
    }
    template <typename A, typename B> inline static real safediv(A a, B b) {
#ifdef NO_ZERODIV_CHECK
        return static_cast<real>(a) / static_cast<real>(b);
#else
        if (b!=0) {
            return static_cast<real>(a)/static_cast<real>(b);
        }
        else {
            Core::abort(Core::fromUtf8("Деление на 0"));
            return 0.0;
        }
#endif
    }
    inline static int maxint() {
        return int(0x7FFFFFFF);
    }
    inline static real maxreal() {
        return 1.797693e+308;
    }
    inline static int div(int a, int b) {
        if (b<=0) {
            Core::abort(Core::fromUtf8("Деление на не натуральное число"));
            return 0;
        } else {
            int aa = a;
            if (aa<0) {
                unsigned int absolunta = -1*(aa);
                unsigned int quoti = absolunta / b + 1;
                aa += quoti * b;
            }
            int m = aa % b;
            return (a-m)/b;
        }
    }
    inline static int mod(int a, int b) {
        if (b<=0) {
            Core::abort(Core::fromUtf8("Деление на не натуральное число"));
            return 0;
        } else {
            if (a<0) {
                unsigned int absolunta = -1*(a);
                unsigned int quoti = absolunta / b + 1;
                a += quoti * b;
            }
            return a % b;
        }
    }
    template <typename T> static int sign(T x) {
        if (x>0) return 1;
        else if (x<0) return -1;
        else return 0;
    }
};

#include <stdlib.h>
class Random {
public:
    inline static void init() {
#ifndef WIN32
        FILE * urandom = fopen("/dev/urandom", "rb");
        char buffer[sizeof(unsigned)];
        fread(buffer, 1u, sizeof(unsigned), urandom);
        fclose(urandom);
        unsigned seed;
        unsigned * seed_data_ptr = reinterpret_cast<unsigned*>(buffer);
        seed = *seed_data_ptr;
        srand(seed);
#endif
#if defined(WIN32) && defined(USE_MINGW_TOOLCHAIN)
        srand(time(0));
#endif
    }
    inline static void finalize() {}
#if !defined(WIN32) || defined(USE_MINGW_TOOLCHAIN)
    inline static int irand(int a, int b) {
        if (a>b) {
            Core::abort(Core::fromUtf8("Неверный диапазон чисел"));
            return 0;
        }
        else {

            unsigned int rndValue = rand(); // in range [0..2^32]
            unsigned int rd_max =RAND_MAX;
            real scale = static_cast<real>(b-a+1)/static_cast<real>(rd_max);
            return Kumir::Math::imin(b, a+static_cast<int>(scale*rndValue));
        }
    }
    inline static int irnd(int x) {

        unsigned int rndValue = rand(); // in range [0..2^32]
        unsigned int rd_max =RAND_MAX;
        real scale = static_cast<real>(x)/static_cast<real>(rd_max);
        return Kumir::Math::imin(x, 1+static_cast<int>(scale*rndValue));
    }

    inline static real rrand(real a, real b) {
        if (a>b) {
            Core::abort(Core::fromUtf8("Неверный диапазон чисел"));
            return 0.0;
        }
        else {

            unsigned int rndValue = rand(); // in range [0..2^32]
            unsigned int rd_max =RAND_MAX;
            real scale = static_cast<real>(b-a+1)/static_cast<real>(rd_max);
            return Kumir::Math::rmin(b, a+static_cast<real>(scale*rndValue));
        }
    }
    inline static real rrnd(real x) {
        unsigned int rndValue = rand(); // in range [0..2^32]
        unsigned int rd_max =RAND_MAX;
        real scale = static_cast<real>(x)/static_cast<real>(rd_max);
        return Kumir::Math::rmin(x, static_cast<real>(scale*rndValue));
    }
#else
    inline static int irand(int a, int b) {
        if (a>b) {
            Core::abort(Core::fromUtf8("Неверный диапазон чисел"));
            return 0;
        }
        else {
            std::random_device rd;
            unsigned int rndValue = rd(); // in range [0..2^32]
#undef max
            unsigned int rd_max = rd.max();
            real scale = static_cast<real>(b-a+1)/static_cast<real>(rd_max);
            return Kumir::Math::imin(b, a+static_cast<int>(scale*rndValue));
        }
    }
    inline static int irnd(int x) {
        std::random_device rd;
        unsigned int rndValue = rd();
#undef max
        unsigned int rd_max = rd.max();
        real scale = static_cast<real>(x)/static_cast<real>(rd_max);
        return Kumir::Math::imin(x, 1+static_cast<int>(scale*rndValue));
    }

    inline static real rrand(real a, real b) {
        if (a>b) {
            Core::abort(Core::fromUtf8("Неверный диапазон чисел"));
            return 0.0;
        }
        else {
            std::random_device rd;
            unsigned int rndValue = rd(); // in range [0..2^32]
#undef max
            unsigned int rd_max = rd.max();
            real scale = static_cast<real>(b-a)/static_cast<real>(rd_max);
            return Kumir::Math::rmin(b, a+static_cast<real>(scale*rndValue));
        }
    }
    inline static real rrnd(real x) {
        std::random_device rd;
        unsigned int rndValue = rd();
#undef max
        unsigned int rd_max = rd.max();
        real scale = static_cast<real>(x)/static_cast<real>(rd_max);
        return Kumir::Math::rmin(x, static_cast<real>(scale*rndValue));
    }
#endif
};


typedef std::basic_stringstream<Char> StringStream;

class Converter {
public:

    enum ParseError {
        NoError,
        EmptyWord,
        WrongHex,
        WrongReal,
        WrongExpForm,
        BadSymbol,
        Overflow
    };

    inline static void init() {}
    inline static void finalize() {}

    static bool validDecimal(const String & astring) {
        static const String validSymbols = Core::fromAscii("0123456789");
        for (size_t i=0; i<astring.length(); i++) {
            if (i==0 && (astring[i]=='-' || astring[i]=='+'))
                continue;
            if (validSymbols.find(astring[i])==String::npos)
                return false;
        }
        return true;
    }

    static real fromDecimal(const String & astring) {
        real result = 0;
        real power = 1;
        size_t index;
        real digit;
        static const String digits = Core::fromAscii("0123456789");
        for (int i=astring.length()-1; i>=0; i--) {
            if (i==0 && astring[i]=='-') {
                result = -1 * result;
                break;
            }
            if (i==0 && astring[i]=='+')
                break;
            index = digits.find(astring[i]);
            if (index==String::npos)
                return 0.0;
            digit = static_cast<real>(index);
            result += power * digit;
            power *= 10;
        }
        return result;
    }

    static int parseInt(String word, char base, ParseError & error) {
        error = NoError;        
        if (word.length()==0) {
            error = EmptyWord;
            return 0;
        }
        size_t pos = 0;
        bool negative = false;
        if (word.at(pos)==Char('-')) {
            negative = true;
            pos += 1;
        }
        else if (word.at(pos)==Char('+'))
            pos += 1;
        if (base==0) {
            // Autodetect format
            if (word.length()-pos>=1 && word.at(pos)==Char('$')) {
                base = 16;
                pos += 1;
            }
            else if (word.length()-pos>=2 && word.at(pos)==Char('0') && word.at(pos+1)==Char('x')) {
                base = 16;
                pos += 2;
            }
            else {
                base = 10;
            }
        }
        word = word.substr(pos, word.length()-pos);
        if (word.length()==0) {
            error = pos>0 ? EmptyWord : WrongHex;
            return 0;
        }
        int result = 0;
        for (size_t i=0; i<word.length(); i++) {
            int power = Math::ipow(base, word.length()-i-1);
            Char ch = word.at(i);
            char digit = 0;
            bool wrongChar = false;
            if (ch>='0' && ch<='9')
                digit = static_cast<char>(ch)-'0';
            else if (ch>='A' && ch<='Z')
                digit = 10 + static_cast<char>(ch)-'A';
            else if (ch>='a' && ch<='z')
                digit = 10 + static_cast<char>(ch)-'a';
            else
                wrongChar = true;
            if (digit > base)
                wrongChar = true;
            if (wrongChar) {
                error = BadSymbol;
                return 0;
            }
            result += power * digit;
        }
        if (negative)
            result *= -1;
        if (NoError == error && !Math::isCorrectIntegerConstant(word)) {
            error = Overflow;
            return 0;
        }
        return result;
    }

    static real parseReal(String word, Char dot, ParseError & error) {
        error = NoError;
        if (word.length()==0) {
            error = EmptyWord;
            return 0;
        }
        size_t pos = 0;
        bool negative = false;
        if (word.at(pos)==Char('-')) {
            negative = true;
            pos += 1;
        }
        else if (word.at(pos)==Char('+'))
            pos += 1;
        real mantissa = 0.0;
        real exponenta = 0.0;
        real fraction = 0.0;
        String sIntegral, sFractional, sExponenta;
        sIntegral.reserve(30);
        sFractional.reserve(30);
        sExponenta.reserve(30);
        bool dotFound = false;
        bool eFound = false;
        static const String E = Core::fromUtf8("eEеЕ"); // includes cyrillic 'e'
        bool hasE = false;
        for (size_t i=0; i<E.length(); i++) {
            if (!hasE) {
                hasE = word.find(E[i])!=String::npos;
            }
            if (hasE)
                break;
        }
        for (size_t i=pos; i<word.length(); i++) {
            Char ch = word.at(i);
            if (!dotFound) {
                // parse integral part of mantissa
                if (ch==dot || E.find_first_of(ch)!=String::npos) {
                    if (ch==dot)
                        dotFound = true;
                    else {
                        dotFound = true;
                        eFound = true;
                    }

                }
                else {
                    sIntegral.push_back(ch);
                }
            }
            else if (!eFound) {
                // parse fractional part of mantissa
                if (E.find_first_of(ch)!=String::npos) {
                    if (sFractional.length()>0) {
                        if (!validDecimal(sFractional)) {
                            error = hasE? WrongExpForm : WrongReal;
                            return 0.0;
                        }
                        fraction = fromDecimal(sFractional);
                    }
                    if (fraction<0) {
                        error = hasE? WrongExpForm : WrongReal;
                        return 0.0;
                    }
                    eFound = true;
                }
                else {
                    sFractional.push_back(ch);
                }
            }
            else {
                // parse exponenta
                sExponenta.push_back(ch);
            }
        }
        if (hasE && sExponenta.length()==0) {
            error = WrongExpForm;
            return 0.0;
        }
        if (sIntegral.length()+sFractional.length()==0) {
            error = sExponenta.length()>0? WrongExpForm : WrongReal;
            return 0.0;
        }
        int fractionalLength = sFractional.length();
        for (int i=sFractional.length()-1; i>=0; i--) {
            Char ch = sFractional.at(i);
            if (ch==Char('0'))
                fractionalLength -= 1;
            else
                break;
        }
        if (!validDecimal(sIntegral) || !validDecimal(sFractional) || !validDecimal(sExponenta)) {
            error = WrongReal;
            return 0.0;
        }

        if (sExponenta.length()>0 && sExponenta.find(Char('.'))!=String::npos) {
            error = WrongExpForm;
            return 0.0;
        }

        fraction = fromDecimal(sFractional.substr(0,fractionalLength));
        for (int i=0; i<fractionalLength; i++) {
            fraction /= 10.0;
        }
        mantissa = fromDecimal(sIntegral);
        if (mantissa<0) {
            // Extra '-' at start
            error = WrongReal;
            return 0.0;
        }
        mantissa += fraction;
        exponenta = fromDecimal(sExponenta);
        real result = mantissa * ::pow(10, exponenta);
        if (negative && result != 0)
            result *= -1;
        if (!Math::isCorrectReal(result))
            error = Overflow;
        return result;
    }

    static String sprintfInt(int value, char base, int width, char al) {
        String result;
        result.reserve(30);
        static const char * digits = "0123456789abcdefghijklmnopqrstuvwxyz";
        int q, r;
        bool negative = value < 0;
        if (int64_t(value) == -2147483648LL) {
            if (base == 10)
                result = Core::fromAscii(std::string("-2147483648"));
        }
        else {
            q = negative? -value : value;
            if (q>0) {
                while (q>0) {
                    r = q % base;
                    result.insert(0, 1, digits[r]);
                    q = q / base;
                }
            }
            else {
                result.insert(0, 1, '0');
            }
            if (base==16)
                result.insert(0, 1, '$');
            if (negative)
                result.insert(0, 1, '-');
        }
        if (width>0) {
            int leftSpaces = 0;
            int rightSpaces = 0;
            if (al=='l') {
                rightSpaces = width - result.length();
            } else if (al=='r') {
                leftSpaces = width - result.length();
            } else {
                leftSpaces = (width - result.length()) / 2;
                rightSpaces = width - result.length() - leftSpaces;
            }
            if (leftSpaces>0)
                result.insert(0, leftSpaces, ' ');
            if (rightSpaces>0)
                result.append(rightSpaces, ' ');
        }
        return result;
    }

    static String sprintfReal(real value, Char dot, bool expform, int width, int decimals, char al) {
        std::stringstream stream;
        if (0 > decimals && !expform) {
            double absVal = fabs(double(value));
            if (0.0 != value && (absVal < 0.0001 || absVal > 999999.))
                expform = true;
        }
        if (expform) {
            stream << std::scientific;
//            stream.precision(2);
            stream.precision(0>decimals ? 6 : decimals);
        }
        else {
            stream << std::fixed;
            stream.precision(0>decimals ? 6 : decimals);
        }

        stream << value;

        std::string rpart = stream.str();
        std::string expPart;

        if (expform) {
            size_t ePos = rpart.find('e');
            if (std::string::npos == ePos)
                ePos = rpart.find('E');
            expPart = rpart.substr(ePos);
            rpart.resize(ePos);
        }

        // Replace ',' with '.' (for some locales like Russian)
        const size_t dotPos = rpart.find(',');
        if (std::string::npos != dotPos) {
            rpart.replace(dotPos, 1, ".");
        }
        if ((expform || 0>decimals) && std::string::npos != rpart.find('.')) {
            while (rpart.size() > 1 && '0' == rpart.at(rpart.size()-1)) {
                rpart.resize(rpart.length()-1);
            }
            if ('.' == rpart.at(rpart.length()-1)) {
                if (expform)
                    rpart.resize(rpart.length()-1);
                else
                    rpart.push_back('0');
            }
        }
        std::string ascii = rpart + expPart;
        while (width > 0 && ascii.length() < static_cast<size_t>(width)) {
            if ('r' == al) {
                ascii = std::string(" ") + ascii;
            }
            else {
                ascii.push_back(' ');
            }
        }

        EncodingError encodingError;
        String result = Coder::decode(ASCII, ascii, encodingError);
        return result;
    }


    inline static int stringToInt(const String & str, bool & ok)
    {
        ParseError error = NoError;
        int result = parseInt(str, 0, error);
        ok = error == NoError;
        return result;
    }
    inline static real stringToReal(const String & str, bool & ok)
    {
        ParseError error = NoError;
        real result = parseReal(str, '.', error);
        ok = error == NoError;
        return result;
    }
    inline static int stringToIntDef(const String & str, int def)
    {
        bool ok;
        int val = stringToInt(str, ok);
        if (ok) return val; else return def;
    }
    inline static real stringToRealDef(const String & str, real def)
    {
        bool ok;
        real val = stringToReal(str, ok);
        if (ok) return val; else return def;
    }
    inline static bool stringToBoolDef(const String & str, bool def)
    {
        String v = Core::toLowerCaseW(str);
        static const String trues[5] = {
            Core::fromAscii("true"), Core::fromAscii("yes"),
            Core::fromAscii("1"), Core::fromUtf8("да"),
            Core::fromUtf8("истина")
        };
        static const String falses[5] = {
            Core::fromAscii("false"), Core::fromAscii("no"),
            Core::fromAscii("0"), Core::fromUtf8("нет"),
            Core::fromUtf8("ложь")
        };
        for (size_t i=0; i<5; i++) {
            const String & t = trues[i];
            const String & f = falses[i];
            if (v == t) return true;
            if (v == f) return false;
        }
        return def;
    }

    inline static String realToString(real value)
    {
        return sprintfReal(value, '.', false, 0, -1, 'l');
    }
    inline static String intToString(int value)
    {
        return sprintfInt(value, 10, 0, 'l');
    }


};

class StringUtils {
public:
    inline static void init() {}
    inline static void finalize() {}
    inline static int length(const String & str) { return str.length(); }

    inline static int code(Char ch) {
        unsigned char value = 0;
        EncodingError error;
        value = KOI8RCodingTable::enc(ch, error);
        if (error) {
            if (OutOfTable == error) {
                Core::abort(Core::fromUtf8("Символ вне кодировки КОИ-8"));
            }
            else {
                Core::abort(Core::fromUtf8("Ошибка кодирования символа"));
            }
        }
        return static_cast<int>(value);
    }
    inline static Char symbol(int code) {
        if (code<0 || code>255) {
            Core::abort(Core::fromUtf8("Код вне диапазона [0..255]"));
            return L'\0';
        }
        else {
            char buf[2] = { static_cast<char>(code), '\0' };
            charptr p = reinterpret_cast<charptr>(&buf);
            EncodingError encodingError;
            uint32_t val = KOI8RCodingTable::dec(p, encodingError);
            return static_cast<wchar_t>(val);
        }
    }
    inline static int unicode(Char ch) {
        return static_cast<int>(ch);
    }
    inline static Char unisymbol(int code) {
        if (code<0 || code>65535) {
            Core::abort(Core::fromUtf8("Код вне диапазона [0..65535]"));
            return L'\0';
        }
        else {
            return static_cast<wchar_t>(code);
        }
    }

    template <class S, typename C>
    inline static S & trim(S & s) {
        int nonSpacePos = 0;
        for (; nonSpacePos<s.length(); nonSpacePos++) {
            if ( s[nonSpacePos]!=C(' ') &&
                 s[nonSpacePos]!=C('\t') &&
                 s[nonSpacePos]!=C('\n')
                 )
                break;
        }
        if (nonSpacePos>0) {
            s.erase(0, nonSpacePos);
        }
        nonSpacePos = s.length()-1;
        for (; nonSpacePos>=0; nonSpacePos--) {
            if ( s[nonSpacePos]!=C(' ') &&
                 s[nonSpacePos]!=C('\t') &&
                 s[nonSpacePos]!=C('\n')
                 )
                break;
        }
        if (s.length()-(nonSpacePos+1) > 0) {
            s.erase(nonSpacePos+1);
        }
        return s;
    }

    inline static String toLowerCase(const String & s) {
        return Core::toLowerCaseW(s);
    }

    inline static String toUpperCase(const String & s) {
        return Core::toUpperCaseW(s);
    }

    inline static int find(int from, const String &substr, const String & s) {
        if (from<1) {
            Core::abort(Core::fromUtf8("Индекс меньше 1"));
            return 0;
        }
        size_t start = static_cast<size_t>(from-1);
        size_t pos = s.find(substr, start);
        if (pos==String::npos)
            return 0;
        else
            return static_cast<int>(pos+1);
    }

    inline static int find(const String & substr, const String &s) {
        return find(1,substr,s);
    }

    inline static void insert(const String & substr, String & s, int pos) {
        if (pos<1) {
            Core::abort(Core::fromUtf8("Индекс меньше 1"));
        }
        else if (pos-1 >= s.length()) {
            s.append(substr);
        }
        else {
            size_t spos = static_cast<size_t>(pos-1);
            s.insert(spos, substr);
        }
    }

    inline static void replace(String & s, const String & oldSubstr, const String & newSubstr, bool all) {
        size_t pos = 0;
        while (true) {
            pos = s.find(oldSubstr, pos);
            if (pos==String::npos) {
                break;
            }
            s.replace(pos, oldSubstr.length(), newSubstr);
            if (all) {
                pos += newSubstr.length();
            }
            else {
                break;
            }
        }
    }

    inline static void remove(String & s, int pos, int count) {
        if (pos<1) {
            Core::abort(Core::fromUtf8("Индекс меньше 1"));
            return;
        }
        if (count<0) {
            Core::abort(Core::fromUtf8("Количество удаляемый символов меньше 0"));
            return;
        }
        if (count==0) {
            return;
        }
        if (pos-1+count>s.length()) {
            s.resize(static_cast<size_t>(pos-1));
        }
        else {
            s.replace(static_cast<size_t>(pos-1), static_cast<size_t>(count), String());
        }
    }
};

class Files {
    friend class IO;
public:
    inline static void setConsoleInputBuffer(AbstractInputBuffer * b) {
        consoleInputBuffer = b;
    }

    inline static void setConsoleOutputBuffer(AbstractOutputBuffer * b) {
        consoleOutputBuffer = b;
    }

    inline static bool isOpenedFiles() {
        bool remainingOpenedFiles = false;
        for (std::deque<FileType>::iterator it=openedFiles.begin(); it != openedFiles.end(); ++it) {
            FileType & f = *it;
            if (!f.autoClose) {
                remainingOpenedFiles = true;
                break;
            }
        }
        return remainingOpenedFiles;
    }

    inline static void init() {
        fileEncoding = DefaultEncoding;
    }

    inline static void finalize() {
        if (isOpenedFiles() && Core::getError().length()==0)
            Core::abort(Core::fromUtf8("Остались не закрытые файлы"));
        for (size_t i=0; i<openedFiles.size(); i++) {
            FileType & f = openedFiles[i];
            if (f.handle)
                fclose(f.handle);
        }
        openedFiles.clear();
        if (assignedIN!=stdin)
            fclose(assignedIN);
        if (assignedOUT!=stdout)
            fclose(assignedOUT);

        assignedIN = stdin;
        assignedOUT = stdout;
    }

    inline static void setFileEncoding(const String & enc) {
        String encoding = Core::toLowerCaseW(enc);
        StringUtils::trim<String,Char>(encoding);
        if (encoding.length()==0) {
            fileEncoding = DefaultEncoding;
            return;
        }
        size_t minus = encoding.find_first_of(Char('-'));
        if (minus!=String::npos) {
            encoding.erase(minus, 1);
        }
        static const String ansi1 = Core::fromAscii("cp1251");
        static const String ansi2 = Core::fromAscii("windows1251");
        static const String ansi3 = Core::fromAscii("windows");
        static const String ansi4 = Core::fromAscii("ansi");
        static const String ansi5 = Core::fromAscii("1251");
        static const String oem1 = Core::fromAscii("cp866");
        static const String oem2 = Core::fromAscii("ibm866");
        static const String oem3 = Core::fromAscii("ibm");
        static const String oem4 = Core::fromAscii("oem");
        static const String oem5 = Core::fromAscii("oem866");
        static const String oem6 = Core::fromAscii("dos");
        static const String koi1 = Core::fromAscii("koi8");
        static const String koi2 = Core::fromAscii("koi8r");
        static const String koi3 = Core::fromUtf8("кои8");
        static const String koi4 = Core::fromUtf8("кои8р");
        static const String utf1 = Core::fromAscii("utf");
        static const String utf2 = Core::fromAscii("utf8");
        static const String utf3 = Core::fromAscii("linux");
        static const String intel1 = Core::fromAscii("unicode");
        static const String intel2 = Core::fromAscii("utf16");
        static const String intel3 = Core::fromAscii("utf16le");
        static const String intel4 = Core::fromUtf8("юникод");
        static const String motorola = Core::fromAscii("utf16be");
        if (encoding==ansi1 || encoding==ansi2 || encoding==ansi3 || encoding==ansi4 || encoding==ansi5) {
            fileEncoding = CP1251;
        }
        else if (encoding==oem1 || encoding==oem2 || encoding==oem3 || encoding==oem4 || encoding==oem5 || encoding==oem6) {
            fileEncoding = CP866;
        }
        else if (encoding==koi1 || encoding==koi2 || encoding==koi3 || encoding==koi4) {
            fileEncoding = KOI8R;
        }
        else if (encoding==utf1 || encoding==utf2 || encoding==utf3) {
            fileEncoding = UTF8;
        }
        else if (encoding==intel1 || encoding==intel2 || encoding==intel3 || encoding==intel4) {
            fileEncoding = UTF16INTEL;
        }
        else if (encoding==motorola) {
            fileEncoding = UTF16MOTOROLA;
        }
        else {
            Core::abort(Core::fromUtf8("Неизвестная кодировка"));
        }
    }

#if defined(WIN32) || defined(_WIN32)
    inline static String getAbsolutePath(const String & fileName) {
        wchar_t cwd[1024];
        GetCurrentDirectoryW(1024, cwd);
        String workDir;
        workDir = String(cwd);
        workDir.push_back(Char('\\'));
        String absPath;
        if (fileName.length()==0
                ||
                (fileName.length()>2 && fileName.at(1)==Char(':') && fileName.at(2)==Char('\\'))
            )
            absPath = fileName;
        else
            absPath = workDir + fileName;
        return getNormalizedPath(absPath, Char('\\'));
    }
    inline static String CurrentWorkingDirectory() {
        wchar_t cwd[1024];
        GetCurrentDirectoryW(1024, cwd);
        String workDir;
        workDir = String(cwd);
        return workDir;
    }
#else
    inline static String getAbsolutePath(const String & fileName) {
        char cwd[1024];
        getcwd(cwd, 1024*sizeof(char));
        String workDir;
        std::string sworkDir = std::string(cwd);
        workDir = Core::fromUtf8(sworkDir);
        workDir.push_back(Char('/'));
        String absPath;
        if (fileName.length()==0 || fileName.at(0)==Char('/'))
            absPath = fileName;
        else
            absPath = workDir + fileName;
        return getNormalizedPath(absPath, Char('/'));
    }

    inline static String CurrentWorkingDirectory() {
        char cwd[1024];
        getcwd(cwd, 1024*sizeof(char));
        String workDir;
        std::string sworkDir = std::string(cwd);
        workDir = Core::fromUtf8(sworkDir);
        return workDir;
    }

#endif

    inline static String getNormalizedPath(const String & path, const Char separator)
    {
        if (path.length()==0)
            return path;
        const StringList parts = Core::splitString(path, separator, true);
        StringList normParts;
        int skip = 0;
        String result;
        static const String CUR = Core::fromAscii(".");
        static const String UP  = Core::fromAscii("..");
        for (int i=parts.size()-1; i>=0; i--) {
            const String & apart = parts[i];
            if (apart==CUR) {
                // pass
            }
            else if (apart==UP) {
                skip += 1;
            }
            else {
                if (skip==0) {
                    normParts.push_front(apart);
                }
                else {
                    skip -= 1;
                }
            }
        }
        result = normParts.join(separator);
        if (path.at(0)==separator)
            result.insert(0, 1, separator);
        if (path.length()>1 && path.at(path.length()-1)==separator)
            result.push_back(separator);
        return result;
    }

#if defined(WIN32) || defined(_WIN32)
    inline static bool canOpenForRead(const String & path)
    {
        return exist(path); // TODO implement me
    }

    inline static bool canOpenForWrite(const String & path)
    {
        return true;
    }
#else
    inline static bool canOpenForRead(const String & fileName)
    {
        char * path;
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
        struct stat st;
        int res = stat(path, &st);
        bool exists = res==0;
        bool readAsOwner = false;
        bool readAsGroup = false;
        bool readAsOther = false;
        if (exists) {
            bool w = st.st_mode & S_IRUSR;
            bool g = st.st_mode & S_IRGRP;
            bool o = st.st_mode & S_IROTH;
            readAsOwner = w && getuid()==st.st_uid;
            readAsGroup = g && getgid()==st.st_gid;
            readAsOther = o;
        }
        free(path);
        bool result = readAsOwner || readAsGroup || readAsOther;
        return result;
    }

    inline static bool canOpenForWrite(const String & fileName)
    {
        char * path;
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
        struct stat st;
        int res = stat(path, &st);
        bool exists = res==0;
        bool writeAsOwner = false;
        bool writeAsGroup = false;
        bool writeAsOther = false;
        if (exists) {
            bool w = st.st_mode & S_IRUSR;
            bool g = st.st_mode & S_IRGRP;
            bool o = st.st_mode & S_IROTH;
            writeAsOwner = w && getuid()==st.st_uid;
            writeAsGroup = g && getgid()==st.st_gid;
            writeAsOther = o;
        }
        else if (errno==ENOENT) {
            while (!exists) {
                int i = 0;
                for (i=strlen(path)-1; i>=0; i++) {
                    if (path[i]=='/') {
                        path[i] = '\0';
                        break;
                    }
                }
                if (i==0)
                    break;
                res = stat(path, &st);
                exists = res==0;
                if (exists) {
                    bool w = st.st_mode & S_IRUSR;
                    bool g = st.st_mode & S_IRGRP;
                    bool o = st.st_mode & S_IROTH;
                    writeAsOwner = w && getuid()==st.st_uid;
                    writeAsGroup = g && getgid()==st.st_gid;
                    writeAsOther = o;
                    break;
                }
            }
        }
        free(path);
        bool result = writeAsOwner || writeAsGroup || writeAsOther;
        return result;
    }
#endif

#if defined(WIN32) || defined(_WIN32)
    inline static bool exist(const String & fileName) {
            DWORD dwAttrib = GetFileAttributesW(fileName.c_str());
            return dwAttrib!=INVALID_FILE_ATTRIBUTES;
    }
    inline static bool unlink(const String & fileName) {
        if (DeleteFileW(fileName.c_str())!=0)
            return true;
         else {
            if (GetLastError()==ERROR_FILE_NOT_FOUND)
                return false;
            else
                return false;
        }
    }
    inline static bool rmdir(const String & fileName) {
        if (RemoveDirectoryW(fileName.c_str())) {
            return true;
        }
        else {
            return false;
        }
    }

    inline static bool isDirectory(const String & fileName) {
        DWORD dwAttrib = GetFileAttributesW(fileName.c_str());
        return (dwAttrib!=INVALID_FILE_ATTRIBUTES &&
                 (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }
    inline static bool mkdir(const String & fileName) {
        BOOL result = CreateDirectoryW(fileName.c_str(), NULL);
        return result;
    }
#else
    inline static bool exist(const String & fileName) {
        EncodingError encodingError;
        std::string localName = Kumir::Coder::encode(Kumir::UTF8, fileName, encodingError);
        const char * path = localName.c_str();
        struct stat st;
        int res = stat(path, &st);
        bool result = res==0;
        return result;
    }

    inline static bool isDirectory(const String & fileName) {
        char * path;
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
        struct stat st;
        int res = stat(path, &st);
        bool result = res==0 && S_ISDIR(st.st_mode);
        free(path);
        return result;
    }

    inline static bool mkdir(const String & fileName) {
        char * path;
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
        int res = ::mkdir(path, 0666);
        bool result = res==0;
        free(path);
        return result;
    }

    inline static bool unlink(const String & fileName) {
        char * path;
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
        int res = ::unlink(path);
        bool result;
        if (res==0)
            result = true;
        else {
            result = false;
        }
        free(path);
        return result;
    }

    inline static bool rmdir(const String & fileName) {
        char * path;
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
        int res = ::rmdir(path);
        bool result;
        if (res==0)
            result = true;
        else {
            result = false;
        }
        free(path);
        return result;
    }
#endif

    inline static FileType getConsoleBuffer() {
        if (!consoleInputBuffer) {
            Core::abort(Core::fromUtf8("Консоль не доступна"));
            return FileType();
        }
        else {
            FileType ft;
            ft.valid = true;
            ft.setType(FileType::Console);
            return ft;
        }
    }

    inline static FileType open(const String & shortName, FileType::OpenMode mode, bool remember, FILE* *fh) {
        const String fileName = getAbsolutePath(shortName);
        for (std::deque<FileType>::const_iterator it = openedFiles.begin(); it!=openedFiles.end(); ++it) {
            const FileType & f = (*it);
            if (f.getName()==fileName) {
                Core::abort(Core::fromUtf8("Файл уже открыт: ")+fileName);
                return FileType();
            }
        }
        bool isCorrectName = true;
        std::string localName;
#if !defined(WIN32) && !defined(_WIN32)
        EncodingError encodingError;

        localName = Coder::encode(UTF8, fileName, encodingError);
        isCorrectName = NoEncodingError == encodingError;

#endif
        if (!isCorrectName) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Ошибка открытия файла: имя содержит недопустимый символ"));
            return FileType();
        }
        const char * path = localName.c_str();

#if defined(WIN32) || defined(_WIN32)
        const wchar_t * fmode;
        if (mode==FileType::Read)
            fmode = L"rb";
        else if (mode==FileType::Write)
            fmode = L"wb";
        else if (mode==FileType::Append)
            fmode = L"ab";
        FILE * res = _wfopen(fileName.c_str(), fmode);
//        _wfopen_s(&res, fileName.c_str(), fmode);
        const bool file_not_exists = ENOENT==errno;
#else
        const char * fmode;
        if (mode==FileType::Read)
            fmode = "rb";
        else if (mode==FileType::Write)
            fmode = "wb";
        else if (mode==FileType::Append)
            fmode = "ab";
        FILE* res = fopen(path, fmode);
        const bool file_not_exists = ENOENT==errno;
#endif
        FileType f;
        if (res==0) {
            if (file_not_exists) {
                Core::abort(Core::fromUtf8("Файл не существует: ")+fileName);
            }
            else {
                Core::abort(Core::fromUtf8("Невозможно открыть файл: ")+fileName);
            }
        }
        else {
            if (mode==FileType::Append)
                mode = FileType::Write;
            f.setName(fileName);
            f.setMode(mode);
            f.handle = res;
            f.autoClose = !remember;
            openedFiles.push_back(f);
            if (fh) {
                *fh = res;
            }
        }
        return f;
    }
    inline static void close(const FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();        
        for (; it!=openedFiles.end(); ++it) {
            FileType f = (*it);
            if (f==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return;
        }
        FileType f = (*it);
        FILE * fh = f.handle;
        f.invalidate();
        if (fh)
            fclose(fh);
        openedFiles.erase(it);        
    }

    inline static void reset(FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();
        for (; it!=openedFiles.end(); ++it) {
            const FileType & f = (*it);
            if (f==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return;
        }
        const FileType & f = (*it);
        FILE * fh = f.handle;
        fseek(fh, 0, 0);
    }
    inline static bool eof(const FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();
        for (; it!=openedFiles.end(); ++it) {
            const FileType & f = (*it);
            if (f==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return false;
        }
        const FileType & f = (*it);
        FILE * fh = f.handle;
        if (feof(fh))
            return true;
        unsigned char ch = 0x00;
        if (fh!=stdin) {
            ch = fgetc(fh);
            ungetc(ch, fh);
        }
        else {
            long pos = ftell(fh);
            ch = fgetc(fh);
            fseek(fh, pos, SEEK_SET);
        }
        return ch==0xFF;
    }
    inline static bool hasData(const FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();
        for (; it!=openedFiles.end(); ++it) {
            const FileType & f = (*it);
            if (f==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return false;
        }
        FILE * fh = (*it).handle;
        long backPos = -1;
        if (fh!=stdin)
            backPos = ftell(fh);
        std::vector<char> buffer(1024);
        size_t readAmount = 0;
        bool result = false;
        while (1) {
            if (feof(fh))
                break;
            char ch = fgetc(fh);
            if ((unsigned char)(ch)==0xFF)
                break;
            if (fh==stdin) {
                if (readAmount>=buffer.size())
                    buffer.resize(buffer.size()*2);
                buffer[readAmount] = ch;
                readAmount ++;
            }
            if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
                // found a whitespace
            }
            else {
                result = true;
                break;
            }
        }
        if (fh==stdin) {
            for (int i=readAmount-1; i>=0; i--) {
                ungetc(buffer[i], fh);
            }
        }
        else {
            fseek(fh, backPos, SEEK_SET);
        }
        return result;
    }

    inline static bool overloadedStdIn() {
        return assignedIN!=stdin;
    }

    inline static bool overloadedStdOut() {
        return assignedOUT!=stdout;
    }

    inline static FILE* getAssignedIn() {
        return assignedIN;
    }

    inline static FILE* getAssignedOut() {
        return assignedOUT;
    }

    inline static void assignInStream(String fileName) {
        StringUtils::trim<String,Char>(fileName);
        if (assignedIN!=stdin)
            fclose(assignedIN);
        if (fileName.length()>0)
            open(fileName, FileType::Read, false, &assignedIN);
        else
            assignedIN = stdin;
    }

    inline static void assignOutStream(String fileName) {
        StringUtils::trim<String,Char>(fileName);
        if (assignedIN!=stdout)
            fclose(assignedOUT);
        if (fileName.length()>0)
            open(fileName, FileType::Write, false, &assignedOUT);
        else
            assignedOUT = stdout;
    }

private:

    static FILE * assignedIN;
    static FILE * assignedOUT;

    static std::deque<FileType> openedFiles;

    static AbstractInputBuffer* consoleInputBuffer;
    static AbstractOutputBuffer* consoleOutputBuffer;
    static AbstractOutputBuffer* consoleErrorBuffer;

    struct IntegerFormat {
        int base;
        int width;
        enum Alignment { Left, Center, Right } alignment;
    };
    struct RealFormat {
        Char delimiter;
        int width;
        enum Alignment { Left, Center, Right } Alignment;
    };
    struct BoolFormat {
        std::set<String> yes;
        std::set<String> no;
    };
    struct CharFormat {
        bool raw;
    };
    struct StringFormat {
        enum LexemFormat { Word, Literal, Line } literal;
    };
    static Encoding fileEncoding;
};


class IO {
public:

    enum StreamType { File, InternalBuffer, ExternalBuffer };

    inline static void init() {}
    inline static void finalize() {}

    static String inputDelimeters;

    class OutputStream {
    public:
        OutputStream()
        {
            file = 0;
            encoding = UTF8;
            buffer.reserve(100);
            streamType_ = InternalBuffer;
            externalBuffer_ = 0;
        }
        OutputStream(FILE * f, Encoding enc) {
            streamType_ = File;
            file = f;
            if (enc==DefaultEncoding)
                encoding = UTF8;
            else
                encoding = enc;
            externalBuffer_ = 0;
        }
        OutputStream(AbstractOutputBuffer * buffer) {
            streamType_ = ExternalBuffer;
            file = 0;
            encoding = UTF8;
            externalBuffer_ = buffer;
        }

        inline const String & getBuffer() const { return buffer; }
        inline const StreamType type() const { return streamType_; }

        void writeRawString(const String & s) {
            if (type() == File) {
                if (encoding==UTF8 && ftell(file)==0) {
                    static const char * BOM = "\xEF\xBB\xBF";
                    fwrite(BOM, sizeof(char), 3, file);
                }
                std::string bytes;
                EncodingError encodingError;
                bytes = Coder::encode(encoding, s, encodingError);
                if (encodingError) {
                    Core::abort(Core::fromUtf8("Ошибка кодирования строки вывода: недопустимый символ"));
                }
                fwrite(bytes.c_str(), sizeof(char), bytes.length(), file);
            }
            else if (type() == ExternalBuffer) {
                if (!externalBuffer_) {
                    Core::abort(Core::fromUtf8("Ошибка вывода: консоль не доступна"));
                }
                else {
                    externalBuffer_->writeRawString(s);
                }
            }
            else {
                buffer.append(s);
            }
        }

    private:
        StreamType streamType_;
        FILE * file;
        Encoding encoding;
        String buffer;
        AbstractOutputBuffer * externalBuffer_;

    };


    class InputStream {
    public:        

        inline InputStream() {
            streamType_ = InternalBuffer;
            file_=0;
            encoding_=UTF8;
            errStart_=0;
            errLength_=0;
            currentPosition_=0;
            externalBuffer_ = 0;
        }

        inline InputStream(const String & b) {
            streamType_ = InternalBuffer;
            file_=0;
            encoding_=UTF8;
            errStart_=0;
            errLength_=0;
            buffer_=b;
            currentPosition_=0;
            externalBuffer_ = 0;
        }

        inline InputStream(AbstractInputBuffer * buffer) {
            streamType_ = ExternalBuffer;
            file_=0;
            encoding_=UTF8;
            errStart_=0;
            errLength_=0;
            currentPosition_=0;
            externalBuffer_ = buffer;
        }

        inline InputStream(FILE * f, Encoding enc) {
            streamType_ = File;
            file_ = f;
            externalBuffer_ = 0;
            if (enc==DefaultEncoding) {
                bool forceUtf8 = false;
                if (f!=stdin) {
                    long curpos = ftell(f);
                    fseek(f, 0, SEEK_SET);
                    unsigned char B[3];
                    if (fread(B, 1, 3, f)==3) {
                        forceUtf8 = B[0]==0xEF && B[1]==0xBB && B[2]==0xBF;
                    }
                    fseek(f, curpos, SEEK_SET);
                }
                if (forceUtf8)
                    encoding_ = UTF8;
                else
                    encoding_ = Core::getSystemEncoding();
            }
            else {
                encoding_ = enc;
            }
            errStart_ = 0;
            errLength_ = 0;
            currentPosition_=0;
            if (f == stdin) {
                fileSize_ = -1;
            }
            else {
                long curpos = ftell(f);
                fseek(f, 0L, SEEK_END);
                fileSize_ = ftell(f);
                fseek(f, curpos, SEEK_SET);
            }
        }
        inline int currentPosition() const {
            return currentPosition_;
        }
        inline void getError(String & text, int & start, int & len) const {
            text = error_;
            start = errStart_;
            len = errLength_;
        }

        inline StreamType type() const { return streamType_; }
        inline void setError(const String & err) {
            if (type() != InternalBuffer)
                Core::abort(err);
            else
                error_ = err;
        }
        inline bool hasError() const {
            if (type() != InternalBuffer) {
                return Core::getError().length()>0;
            }
            else {
                return error_.length()>0;
            }
        }
        inline void markPossibleErrorStart() {
            errStart_ = currentPosition_; errLength_ = 0; error_.clear();
        }
        inline bool readRawChar(Char & x) {
            lastCharBuffer_[0] = lastCharBuffer_[1] = lastCharBuffer_[2] = '\0';
            if ( type() == InternalBuffer ) {
                if (currentPosition_==buffer_.length())
                    return false;
                x = buffer_.at(currentPosition_);
                currentPosition_ += 1;
                errLength_ += 1;
                return true;
            }
            else if ( type() == ExternalBuffer ) {
                return externalBuffer_->readRawChar(x);
            }
            else {
                if (feof(file_))
                    return false;
                long pos = ftell(file_);
                if (fileSize_ != -1 && pos >= fileSize_) {
                    return false;
                }
                charptr buffer = reinterpret_cast<charptr>(&lastCharBuffer_);
                if (encoding_!=UTF8) {
                    // Read only one byte
                    lastCharBuffer_[0] = fgetc(file_);
                    uint8_t firstByte = lastCharBuffer_[0];
                    if (firstByte == 255 && fileSize_ == -1) {
                        return false;
                    }
                }
                else {
                    // More complex...
                    long cpos = ftell(file_);
                    if (cpos==0) {
                        // Try to read BOM
                        static const char * BOM = "\xEF\xBB\xBF";
                        char firstThree[3];
                        bool seekBack = true;
                        if (fread(firstThree,sizeof(char),3,file_)==3
                                && strncmp(BOM, firstThree, 3)==0)
                            seekBack = false;
                        if (seekBack)
                            fseek(file_, 0, SEEK_SET);
                    }
                    lastCharBuffer_[0] = fgetc(file_);
                    uint8_t firstByte = lastCharBuffer_[0];
                    uint8_t oneSymbMark = firstByte >> 5;
                    uint8_t twoSymbMark = firstByte >> 4;
                    if (firstByte==255 && file_==Files::getAssignedIn()) {
//                        Core::abort(Core::fromUtf8("Ошибка чтения данных: входной поток закончился"));
                        return false;
                    }
                    else if (firstByte==255) {
                        return false;
                    }
                    int extraBytes = 0;
                    if (firstByte>127) {
                        if ( oneSymbMark & 0x06 )
                            extraBytes = 1;
                        else if ( twoSymbMark & 0x0E ) {
                            extraBytes = 2;
                        }
                    }
                    for (int i=0; i<extraBytes; i++) {
                        if (feof(file_)) {
                            Core::abort(Core::fromUtf8("Ошибка чтения данных из файла: UTF-8 файл поврежден"));
                            return false;
                        }
                        lastCharBuffer_[i+1] = fgetc(file_);
                    }
                }
                const std::string sb(buffer);
                std::wstring res;
                EncodingError encodingError;
                res = Coder::decode(encoding_, sb, encodingError);
                if (encodingError) {
                    Core::abort(Core::fromUtf8("Ошибка перекодирования при чтении данных из текстового файла"));
                    return false;
                }
                if (res.length()==0) {
                    Core::abort(Core::fromUtf8("Ошибка перекодирования при чтении данных из текстового файла"));
                    return false;
                }
                x = res.at(0);
                return true;
            }
        }
        inline void pushLastCharBack() {
            if ( type() == InternalBuffer ) {
                currentPosition_ -= 1;
                errLength_ -= 1;
            }
            else if ( type() == ExternalBuffer ) {
                externalBuffer_->pushLastCharBack();
            }
            else /* File */ {
                if (file_==stdin) {
                    if (lastCharBuffer_[2]!='\0')
                        ungetc(lastCharBuffer_[2], file_);
                    if (lastCharBuffer_[1]!='\0')
                        ungetc(lastCharBuffer_[1], file_);
                    ungetc(lastCharBuffer_[0], file_);
                }
                else {
                    fseek(file_, -1*strlen(lastCharBuffer_), SEEK_CUR);
                }
            }
        }
        inline String readUntil(const String & delimeters) {
            String result;
            result.reserve(100);
            Char current;
            while (readRawChar(current)) {
                if (delimeters.find_first_of(current)!=String::npos
                        && current!=Char('\r')
                        ) {
                    pushLastCharBack();
                    break;
                }
                else {
                    if (current!=Char('\r'))
                        result.push_back(current);
                }
            }
            return result;
        }
        inline void skipDelimiters(const String & delim) {
            // Skip delimiters until lexem
            Char skip(32);
            while (readRawChar(skip)) {
                if (delim.find_first_of(skip)==String::npos
                        && skip!=Char('\r')
                        ) {
                    pushLastCharBack();
                    break;
                }
            }
        }

    private:        
        StreamType streamType_;
        FILE * file_;
        long fileSize_;
        Encoding encoding_;
        String buffer_;
        String error_;
        int errStart_;
        int errLength_;
        int currentPosition_;
        char lastCharBuffer_[3];
        AbstractInputBuffer * externalBuffer_;
    }; // end inner class InputStream


protected:
    // IO helper functions
    static String readWord(InputStream & is) {
        String delim = inputDelimeters;
        is.skipDelimiters(delim);
        // Mark as lexem begin position
        is.markPossibleErrorStart();
        return is.readUntil(delim);
    }

    static String readLiteralOrWord(InputStream & is) {
        String delim = inputDelimeters;
        is.skipDelimiters(delim);
        // Mark as lexem begin position
        is.markPossibleErrorStart();
        Char bracket = Char('\0');
        if (!is.readRawChar(bracket)) {
            is.setError(Core::fromUtf8("Не могу прочитать литерал: текст закончился"));
            return String();
        }
        if (bracket!=Char('\'') && bracket!=Char('"')) {
            is.pushLastCharBack();
            return is.readUntil(delim);
        }
        Char current;
        String result;
        result.reserve(100);
        while (is.readRawChar(current)) {
            if (current!=bracket)
                result.push_back(current);
            else
                break;
        }
        if (current!=bracket) {
//            is.setError(Core::fromUtf8("Ошибка чтения литерала: текст закончился раньше, чем появилась закрывающая кавычка"));
        }
        else {
            // Skip closing bracket
            is.readRawChar(bracket);
        }
        return result;
    }



public:
    // Generic functions to be in use while input from GUI
    inline static String readLine(InputStream & is) {
        String result;
        result.reserve(100);
        Char current;
        while (is.readRawChar(current)) {
            if (current!=10 && current!=13)
                result.push_back(current);
            if (current==10)
                break;
        }
        return result;
    }
    inline static String readString(InputStream & is) {
        return readLiteralOrWord(is);
    }

    inline static Char readChar(InputStream & is) {
        Char result(0);
        if (is.hasError()) return result;

        bool ok;
        ok = is.readRawChar(result);
        if (!ok)
            is.setError(Core::fromUtf8("Ошибка ввода символа: текст закончился"));

        return result;
    }
    inline static int readInteger(InputStream & is) {
        String word = readWord(is);
        if (is.hasError()) return 0;
        Converter::ParseError error = Converter::NoError;
        int result = Converter::parseInt(word, 10, error);
        if (error==Converter::EmptyWord) {
            is.setError(Core::fromUtf8("Ошибка ввода целого числа: текст закончился"));
        }
        else if (error==Converter::BadSymbol) {
            is.setError(Core::fromUtf8("Ошибка ввода целого числа: число содержит неверный символ"));
        }
        else if (error==Converter::Overflow) {
            is.setError(Core::fromUtf8("Ошибка ввода: слишком большое целое число"));
        }
        return result;
    }
    inline static real readReal(InputStream & is) {
        String word = readWord(is);
        if (is.hasError()) return 0;
        Converter::ParseError error = Converter::NoError;
        real result = Converter::parseReal(word, '.', error);
        if (error==Converter::EmptyWord) {
            is.setError(Core::fromUtf8("Ошибка ввода вещественного числа: текст закончился"));
        }
        else if (error==Converter::BadSymbol) {
            is.setError(Core::fromUtf8("Ошибка ввода вещественного числа: число содержит неверный символ"));
        }
        else if (error==Converter::WrongExpForm) {
            is.setError(Core::fromUtf8("Ошибка ввода вещественного числа: неверная запись экспоненциальной формы"));
        }
        else if (error==Converter::WrongReal) {
            is.setError(Core::fromUtf8("Ошибка ввода вещественного числа: неверная запись"));
        }
        else if (error==Converter::Overflow) {
            is.setError(Core::fromUtf8("Ошибка ввода: слишком большое вещественное число"));
        }
        return result;
    }
    inline static bool readBool(InputStream & is) {
        String word = Core::toLowerCaseW(readWord(is));
        if (is.hasError()) return 0;
        if (word.length()==0) {
            is.setError(Core::fromUtf8("Ошибка ввода логического значения: ничего не введено"));
        }
        bool yes = false;
        bool no = false;
        static std::set<String> YES, NO;
        YES.insert(Core::fromAscii("true"));
        YES.insert(Core::fromAscii("yes"));
        YES.insert(Core::fromAscii("1"));
        YES.insert(Core::fromUtf8("да"));
        YES.insert(Core::fromUtf8("истина"));
        NO.insert(Core::fromAscii("false"));
        NO.insert(Core::fromAscii("no"));
        NO.insert(Core::fromAscii("0"));
        NO.insert(Core::fromUtf8("нет"));
        NO.insert(Core::fromUtf8("ложь"));

        if (YES.count(word)) {
            yes = true;
        }


        if (NO.count(word)) {
            no = true;
        }

        if (!yes && !no)
        {
            is.setError(Core::fromUtf8("Ошибка ввода логического значения: неизвестное значение"));
        }
        return yes;
    }

    inline static void writeString(OutputStream & os, const String & str, int width) {
        String data = str;
        if (width) {
            // TODO implement me
        }
        os.writeRawString(data);
    }

    inline static void writeChar(OutputStream & os, const Char & chr, int width) {
        String data;
        data.push_back(chr);
        if (width) {
            // TODO implement me
        }
        os.writeRawString(data);
    }

    inline static void writeInteger(OutputStream & os, int value, int width) {
        const String strval = Converter::sprintfInt(value, 10, width, 'r');
        os.writeRawString(strval);
    }

    inline static void writeReal(OutputStream & os, real value, int width, int decimals) {
        const String strval = Converter::sprintfReal(value, '.', false, width, decimals, 'r');
        os.writeRawString(strval);
    }

    inline static void writeBool(OutputStream & os, bool value, int width) {
        static const String YES = Core::fromUtf8("да");
        static const String NO = Core::fromUtf8("нет");
        const String & sval = value? YES : NO;
        if (width) {
            // TODO implement me
        }
        os.writeRawString(sval);
    }

    // Format parsing functions
    inline static StringList splitIntoLexemsByDelimeter(const String &s, Char delim = Char(':')) {
        StringList result;
        String current;
        current.reserve(10);
        for (size_t i=0; i<s.length(); i++) {
            if (s[i]==delim) {
                result.push_back(current);
                current.clear();
                current.reserve(10);
            }
            else if (s[i]!=' ') {
                current.push_back(s[i]);
            }
        }
        if (current.length()>0)
            result.push_back(current);
        return result;
    }


    static Encoding LOCALE_ENCODING;

    // Actual functions to be in use while input from stream

    static InputStream makeInputStream(FileType fileNo, bool fromStdIn) {
        if (fromStdIn && fileNo.getType()!=FileType::Console) {
            return InputStream(Files::getAssignedIn(), LOCALE_ENCODING);
        }
        else if (fileNo.getType() == FileType::Console) {
            return InputStream(Files::consoleInputBuffer);
        }
        else {
            std::deque<FileType>::iterator it = Files::openedFiles.begin();            
            for ( ; it!=Files::openedFiles.end(); ++it) {
                if (*it==fileNo) {
                    break;
                }
            }
            if (it==Files::openedFiles.end()) {
                Core::abort(Core::fromUtf8("Файл с таким ключем не открыт"));
                return InputStream();
            }
            const FileType file = (*it);
            if (file.getMode()!=FileType::Read) {
                Core::abort(Core::fromUtf8("Файл с таким ключем открыт на запись"));
                return InputStream();
            }
            return InputStream((*it).handle, Files::fileEncoding);
        }
    }

    inline static OutputStream makeOutputStream(FileType fileNo, bool toStdOut) {
      //  std::cout<<fileNo.fullPath;
        if (toStdOut) {
            return OutputStream(Files::getAssignedOut(), LOCALE_ENCODING);
        }
        else if (fileNo.getType() == FileType::Console) {
            return OutputStream(Files::consoleOutputBuffer);
        }
        else {
            std::deque<FileType>::iterator it = Files::openedFiles.begin();            
            for ( ; it!=Files::openedFiles.end(); ++it) {
                if (*it==fileNo) {
                    break;
                }
            }
            if (it==Files::openedFiles.end()) {
                Core::abort(Core::fromUtf8("Файл с таким ключем не открыт"));
                return OutputStream();
            }
            const FileType file = (*it);
            if (file.getMode()==FileType::Read) {
                Core::abort(Core::fromUtf8("Файл с таким ключем открыт на чтение"));
                return OutputStream();
            }
            return OutputStream((*it).handle, Files::fileEncoding);
        }
    }

    inline static int readInteger(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readInteger(stream);
    }

    inline static real readReal(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0.0;
        return readReal(stream);
    }

    inline static bool readBool(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return false;
        return readBool(stream);
    }

    inline static Char readChar(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return Char(' ');
        return readChar(stream);
    }
    inline static String readString(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return String();
        return readString(stream);
    }
    inline static String readLine(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return String();
        return readLine(stream);
    }

    inline static void writeInteger(int width, int value, FileType fileNo = FileType(), bool toStdOut = true) {
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeInteger(stream, value, width);
    }

    inline static void writeReal(int width, int decimals, real value, FileType fileNo = FileType(), bool toStdOut = true) {
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeReal(stream, value, width, decimals);
    }
    inline static void writeBool(int width, bool value, FileType fileNo = FileType(), bool toStdOut = true) {
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeBool(stream, value, width);
    }
    inline static void writeString(int width, const String & value, FileType fileNo = FileType(), bool toStdOut = true) {
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeString(stream, value, width);
    }
    inline static void writeChar(int width, Char value, FileType fileNo = FileType(), bool toStdOut = true) {
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeChar(stream, value, width);
    }

}; // end class IO

class System {
public:
    inline static void init() {}
    inline static void finalize() {}

    inline static int time() {
        int hours = 0;
        int mins = 0;
        int secs = 0;
        int msecs = 0;
#if defined(WIN32) || defined(_WIN32)
        SYSTEMTIME st;
        memset(&st, 0, sizeof(SYSTEMTIME));
        GetSystemTime(&st);
        hours = st.wHour;
        mins = st.wMinute;
        secs = st.wSecond;
        msecs = st.wMilliseconds;
#else
        struct timeval tv;
        gettimeofday(&tv, 0);
        tzset();
        time_t epoch;
        struct tm * loc;
        struct tm dummy;
        epoch = ::time(NULL);
        loc = localtime_r(&epoch, &dummy);
        hours = loc->tm_hour;
        mins = loc->tm_min;
        secs = loc->tm_sec;
        msecs = tv.tv_usec / 1000;
#endif
        int result =
                hours * 3600 * 1000 +
                mins * 60 * 1000 +
                secs * 1000 +
                msecs;
        return result;
    }
};

inline void initStandardLibrary() {
    Core::init();
    Math::init();
    Random::init();
    Converter::init();
    StringUtils::init();
    Files::init();
    System::init();
}

inline void finalizeStandardLibrary() {
    Core::finalize();
    Math::finalize();
    Random::finalize();
    Converter::finalize();
    StringUtils::finalize();
    Files::finalize();
    System::finalize();
}

#ifndef DO_NOT_DECLARE_STATIC
String Core::error = String();
void (*Core::AbortHandler)() = 0;
std::deque<FileType> Files::openedFiles;
AbstractInputBuffer* Files::consoleInputBuffer = 0;
AbstractOutputBuffer* Files::consoleOutputBuffer = 0;
AbstractOutputBuffer* Files::consoleErrorBuffer = 0;
FILE * Files::assignedIN = stdin;
FILE * Files::assignedOUT = stdout;
#if defined(WIN32) || defined(_WIN32)
Encoding IO::LOCALE_ENCODING = CP866;
#else
Encoding IO::LOCALE_ENCODING = UTF8;
#endif
Encoding Files::fileEncoding;
String Kumir::IO::inputDelimeters = Kumir::Core::fromAscii(" \n\t");
#endif

}

#endif // KUMIRSTDLIB_H
