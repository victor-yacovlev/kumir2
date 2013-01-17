#ifndef KUMIRSTDLIB_H
#define KUMIRSTDLIB_H

#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <cstdio>
#include <random>
#include "encodings.hpp"

#ifndef NO_SYSTEM
#   include <time.h>
#endif

#ifdef NO_UNICODE
#include <string>
#   if not defined(ONEBYTE_LOCALE)
#       define ONEBYTE_LOCALE CP866
#   endif
#else
#include <wchar.h>
#include <string>
#endif

#include <cmath>

#ifndef NO_FILES
#   if defined(WIN32) || defined(_WIN32)
#       include <Windows.h>
#   else
#       include <sys/stat.h>
#       include <fcntl.h>
#       include <unistd.h>
#       include <string.h>
#       include <errno.h>
#   endif
#endif

namespace VM { class Variable; }


namespace Kumir {

#ifdef NO_UNICODE
typedef char Char;
typedef std::string String;
#else
typedef wchar_t Char;
typedef std::wstring String;
#endif

#ifdef NO_FPU
typedef float real;
#else
typedef double real;
#endif

struct FileType {
    enum OpenMode { NotOpen, Read, Write, Append };
    inline static const char * _() { return "sib"; }
    inline FileType() { valid = false; mode = NotOpen; }
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
    inline bool isValid() const {
        return valid;
    }
    inline void invalidate() {
        valid = false;
    }
    inline bool operator==(const FileType & other) const {
        return other.fullPath==fullPath;
    }
    inline bool operator!=(const FileType & other) const {
        return other.fullPath!=fullPath;
    }

private:
    String fullPath;
    int mode;
    bool valid;
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
    inline static void init() { error.clear(); }
    inline static void finalize() {}
    inline static const String & getError() { return error; }

    inline static String fromUtf8(const std::string & s) {
        String result;
        try {
            result = Coder::decode(UTF8, s);
        }
        catch (...) {}
        return result;
    }

    inline static String fromAscii(const std::string & s) {
        String result;
        try {
            result = Coder::decode(ASCII, s);
        }
        catch (...) {}
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
            static const std::string last = Coder::encode(ASCII, parts.at(parts.size()-1));
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

protected:
    inline static void abort(const String & err) {
        error = err;
    }
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
        int32_t sum = lhs+rhs;
        if (lhs>=0) {
            return sum >= rhs;
        }
        else {
            return sum < rhs;
        }
    }

    inline static bool checkDiff(int32_t lhs, int32_t rhs) {
        // Check for integer overflow
        int32_t diff = lhs-rhs;
        if (rhs>=0) {
            return diff <= lhs;
        }
        else {
            return diff > lhs;
        }
    }

    inline static bool checkProd(int32_t lhs, int32_t rhs) {
        // Check for integer overflow
        int64_t prod = lhs * rhs;
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
//        uint64_t signMask = 0x8000000000000000;
        uint64_t exponent = (bits & expMask) >> 52;
        uint64_t fraction = bits & fracMask;
//        uint64_t sign = (bits & signMask) >> 63;
        bool Inf = (exponent==uint64_t(0x7FF)) && (fraction==uint64_t(0));
        bool NaN = (exponent==uint64_t(0x7FF)) && (fraction>uint64_t(0));
        return !Inf && !NaN;
    }

    inline static bool isCorrectReal(real val) {
        return isCorrectDouble(val);
        // TODO: implement for ARM/AVR platform!!!!
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
            return ::log(x)/::log(10.0);
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
        return static_cast<int>(::floor(::pow(real(a), b)));
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
        return 0x80000000;
    }
    inline static real maxreal() {
#ifdef NO_FPU
        return 3.4028234e+38;
#else
        return 1.797693e+308;
#endif
    }
    inline static int div(int a, int b) {
        if (b<=0) {
            Core::abort(Core::fromUtf8("Деление на не натуральное число"));
            return 0;
        } else {
            int aa = a;
            while (aa<0)
                aa += b;
            int m = aa % b;
            return (a-m)/b;
        }
    }
    inline static int mod(int a, int b) {
        if (b<=0) {
            Core::abort(Core::fromUtf8("Деление на не натуральное число"));
            return 0;
        } else {
            while (a<0)
                a += b;
            return a % b;
        }
    }
    template <typename T> static int sign(T x) {
        if (x>0) return 1;
        else if (x<0) return -1;
        else return 0;
    }
};

#ifdef NO_RANDOM
// Dummy "random generator"
class Random {
public:
    inline static void init() {}
    inline static void finalize() {}
    template <typename T> inline static T rand(T, T) { return 0; }
    template <typename T> inline static T rnd(T) { return 0; }
};
#else
#include <stdlib.h>
class Random {
public:
    inline static void init() {
    }
    inline static void finalize() {}

    inline static int irand(int a, int b) {
        if (a>b) {
            Core::abort(Core::fromUtf8("Неверный диапазон чисел"));
            return 0;
        }
        else {
            std::random_device rd;
            unsigned int rndValue = rd(); // in range [0..2^32]
#undef max
            unsigned int rd_max = std::random_device::max();
            real scale = static_cast<real>(b-a+1)/static_cast<real>(rd_max);
            return Kumir::Math::imin(b, a+static_cast<int>(scale*rndValue));
        }
    }
    inline static int irnd(int x) {
        std::random_device rd;
        unsigned int rndValue = rd();
#undef max
        unsigned int rd_max = std::random_device::max();
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
            unsigned int rd_max = std::random_device::max();
            real scale = static_cast<real>(b-a+1)/static_cast<real>(rd_max);
            return Kumir::Math::rmin(b, a+static_cast<real>(scale*rndValue));
        }
    }
    inline static real rrnd(real x) {
        std::random_device rd;
        unsigned int rndValue = rd();
#undef max
        unsigned int rd_max = std::random_device::max();
        real scale = static_cast<real>(x)/static_cast<real>(rd_max);
        return Kumir::Math::rmin(x, static_cast<real>(scale*rndValue));
    }
};
#endif

#ifdef NO_UNICODE
typedef std::stringstream StringStream;
#else
typedef std::basic_stringstream<Char> StringStream;
#endif

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
            error = WrongHex;
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

        fraction = fromDecimal(sFractional);
        for (int i=0; i<fractionalLength; i++) {
            fraction /= 10.0;
        }
        mantissa = fromDecimal(sIntegral);
        mantissa += fraction;
        exponenta = fromDecimal(sExponenta);
        real result = mantissa * ::pow(10, exponenta);
        if (negative)
            result *= -1;
        if (!Math::isCorrectDouble(result))
            error = Overflow;
        return result;
    }

    static String sprintfInt(int value, char base, int width, char al) {
        String result;
        result.reserve(30);
        static const char * digits = "0123456789abcdefghijklmnopqrstuvwxyz";
        int q, r;
        bool negative = value < 0;
        q = value>=0? value : -value;
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
        char buffer[500];
        String sprintfFormat;
        sprintfFormat.reserve(10);
        sprintfFormat.push_back('%');
        if (decimals>0) {
            sprintfFormat.push_back('.');
            sprintfFormat.append(sprintfInt(decimals, 10, 0, 'l'));
        }
        if (expform)
            sprintfFormat.push_back('e');
        else
            sprintfFormat.push_back('f');
        sprintf(buffer, Coder::encode(ASCII, sprintfFormat).c_str(), value);
        std::string result(reinterpret_cast<char*>(&buffer));

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
        String uniresult = Coder::decode(ASCII, result);
        size_t dotPos = uniresult.find_first_of('.');
        if (dotPos!=String::npos) {
            uniresult[dotPos] = dot;
        }
        return uniresult;
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
    inline static String realToString(real value)
    {
        return sprintfReal(value, '.', false, 0, 0, 'l');
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
#ifdef NO_UNICODE
#   if ONEBYTE_LOCALE==CP866
#       define CODINGTABLE CP866CodingTable
#   elif ONEBYTE_LOCALE==CP1251
#       define CODINGTABLE CP1251CodingTable
#   elif ONEBYTE_LOCALE==KOI8R
#       define CODINGTABLE KOI8RCodingTable
#   elif ONEBYTE_LOCALE==UTF8
#       define CODINGTABLE UTF8CodingTable
#   endif

    inline static int code(char ch) {
        int value = 0;
        uint32_t univalue = 0;
        char buf[2] = { ch, '\0' };
        charptr p = reinterpret_cast<charptr>(&buf);
        try {
            univalue = CODINGTABLE::dec(p);
            value = static_cast<int>(KOI8RCodingTable::enc(univalue));
        }
        catch (EncodingError e) {
            if (e==OutOfTable)
                Core::abort(Core::fromUtf8("Символ вне кодировки КОИ-8"));
            else
                Core::abort(Core::fromUtf8("Ошибка кодирования символа"));
        }
        return value;
    }
    inline static char symbol(int code) {
        if (code<0 || code>255) {
            Core::abort(Core::fromUtf8("Код вне диапазона [0..255]"));
            return L'\0';
        }
        else {
            char value = '\0';
            uint32_t univalue = 0;
            char buf[2] = { static_cast<char>(code), '\0' };
            charptr p = reinterpret_cast<charptr>(&buf);
            try {
                univalue = KOI8RCodingTable::dec(p);
                value = static_cast<char>(CODINGTABLE::enc(univalue));
            }
            catch (EncodingError e) {
                if (e==OutOfTable)
                    Core::abort(Core::fromUtf8("Символ вне кодировки КОИ-8"));
                else
                    Core::abort(Core::fromUtf8("Ошибка кодирования символа"));
            }
            return value;
        }
    }
    inline static int unicode(char ch) {
        int value = 0;
        char buf[2] = { ch, '\0' };
        charptr p = reinterpret_cast<charptr>(&buf);
        try {
            value = CODINGTABLE::dec(p);
        }
        catch (EncodingError e) {
            if (e==OutOfTable)
                Core::abort(Core::fromUtf8("Символ вне кодировки Базового Юникода"));
            else
                Core::abort(Core::fromUtf8("Ошибка кодирования символа"));
        }
        return value;
    }
    inline static char unisymbol(int code) {
        if (code<0 || code>65535) {
            Core::abort(Core::fromUtf8("Код вне диапазона [0..65535]"));
            return '\0';
        }
        else {
            char value = '\0';
            try {
                value = static_cast<char>(CODINGTABLE::enc(static_cast<uint32_t>(code)));
            }
            catch (EncodingError e) {
                if (e==OutOfTable)
                    Core::abort(Core::fromUtf8("Символ вне кодировки Базового Юникода"));
                else
                    Core::abort(Core::fromUtf8("Ошибка кодирования символа"));
            }
            return value;
        }
    }
#else
    inline static int code(wchar_t ch) {
        unsigned char value = 0;
        try {
            value = KOI8RCodingTable::enc(ch);
        }
        catch (EncodingError e) {
            if (e==OutOfTable)
                Core::abort(Core::fromUtf8("Символ вне кодировки КОИ-8"));
            else
                Core::abort(Core::fromUtf8("Ошибка кодирования символа"));
        }
        return static_cast<int>(value);
    }
    inline static wchar_t symbol(int code) {
        if (code<0 || code>255) {
            Core::abort(Core::fromUtf8("Код вне диапазона [0..255]"));
            return L'\0';
        }
        else {
            char buf[2] = { static_cast<char>(code), '\0' };
            charptr p = reinterpret_cast<charptr>(&buf);
            uint32_t val = KOI8RCodingTable::dec(p);
            return static_cast<wchar_t>(val);
        }
    }
    inline static int unicode(wchar_t ch) {
        return static_cast<int>(ch);
    }
    inline static wchar_t unisymbol(int code) {
        if (code<0 || code>65535) {
            Core::abort(Core::fromUtf8("Код вне диапазона [0..65535]"));
            return L'\0';
        }
        else {
            return static_cast<wchar_t>(code);
        }
    }
#endif
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
#ifdef NO_UNICODE
        return Core::toLowerCase(s);
#else
        return Core::toLowerCaseW(s);
#endif
    }
    inline static String toUpperCase(const String & s) {
#ifdef NO_UNICODE
        return Core::toUpperCase(s);
#else
        return Core::toUpperCaseW(s);
#endif
    }
    inline static int find(int from, const String &substr, const String & s) {
        if (from<1) {
            Core::abort(Core::fromUtf8("Индекс меньше 1"));
            return 0;
        }
        size_t start = static_cast<size_t>(from-1);
        size_t pos = s.find(substr, start);
        if (pos==String::npos)
            return -1;
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
    inline static bool isOpenedFiles() { return openedFiles.size()> 0; }
    inline static void init() {
        fileEncoding = DefaultEncoding;
    }
    inline static void finalize() {
        if (isOpenedFiles() && Core::getError().length()==0)
            Core::abort(Core::fromUtf8("Остались не закрытые файлы"));
        for (size_t i=0; i<openedFiles.size(); i++) {
            fclose(openedFileHandles[i]);
        }
        openedFiles.clear();
        openedFileHandles.clear();
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
        if (fileName.length()==0 || fileName.at(0)==Char('\\'))
            absPath = fileName;
        else
            absPath = workDir + fileName;
        return getNormalizedPath(absPath, Char('\\'));
    }
#else
    inline static String getAbsolutePath(const String & fileName) {
        char cwd[1024];
        getcwd(cwd, 1024*sizeof(char));
        String workDir;
#   ifdef NO_UNICODE
        workDir = String(cwd);
#   else
        wchar_t wcwd[1024];
        size_t pl = mbstowcs(wcwd, cwd, 1024);
        wcwd[pl] = L'\0';
        workDir = String(wcwd);
#   endif
        workDir.push_back(Char('/'));
        String absPath;
        if (fileName.length()==0 || fileName.at(0)==Char('/'))
            absPath = fileName;
        else
            absPath = workDir + fileName;
        return getNormalizedPath(absPath, Char('/'));
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
#   ifdef NO_UNICODE
        path = const_cast<char*>(fileName.c_str());
#   else
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
#   endif
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
#   ifndef NO_UNICODE
        free(path);
#   endif
        bool result = readAsOwner || readAsGroup || readAsOther;
        return result;
    }

    inline static bool canOpenForWrite(const String & fileName)
    {
        char * path;
#   ifdef NO_UNICODE
        path = const_cast<char*>(fileName.c_str());
#   else
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
#   endif
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
#   ifndef NO_UNICODE
        free(path);
#   endif
        bool result = writeAsOwner || writeAsGroup || writeAsOther;
        return result;
    }
#endif

#if defined(WIN32) || defined(_WIN32)
    inline static bool exist(const String & fileName) {
            DWORD dwAttrib = GetFileAttributesW(fileName.c_str());
            return dwAttrib!=INVALID_FILE_ATTRIBUTES;
    }
    inline static int unlinkFile(const String & fileName) {
        if (DeleteFileW(fileName.c_str())!=0)
            return 0;
         else {
            if (GetLastError()==ERROR_FILE_NOT_FOUND)
                return 1;
            else
                return 2;
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
#   ifdef NO_UNICODE
        const char * path = const_cast<char*>(fileName.c_str());
#   else
        std::string localName = Kumir::Coder::encode(Kumir::UTF8, fileName);
        const char * path = localName.c_str();
#   endif
        struct stat st;
        int res = stat(path, &st);
        bool result = res==0;
        return result;
    }

    inline static bool isDirectory(const String & fileName) {
        char * path;
#   ifdef NO_UNICODE
        path = const_cast<char*>(fileName.c_str());
#   else
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
#   endif
        struct stat st;
        int res = stat(path, &st);
        bool result = res==0 && S_ISDIR(st.st_mode);
#   ifndef NO_UNICODE
        free(path);
#   endif
        return result;
    }

    inline static bool mkdir(const String & fileName) {
        char * path;
#   ifdef NO_UNICODE
        path = const_cast<char*>(fileName.c_str());
#   else
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
#   endif
        int res = ::mkdir(path, 0666);
        bool result = res==0;
#   ifndef NO_UNICODE
        free(path);
#   endif
        return result;
    }

    inline static int unlinkFile(const String & fileName) {
        char * path;
#   ifdef NO_UNICODE
        path = const_cast<char*>(fileName.c_str());
#   else
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
#   endif
        int res = ::unlink(path);
        int result;
        if (res==0)
            result = 0;
        else {
            result = errno==ENOENT? 1 : 2;
        }
#   ifndef NO_UNICODE
        free(path);
#   endif
        return result;
    }
#endif
    inline static FileType open(const String & shortName, FileType::OpenMode mode, bool remember=true, FILE* *fh = 0) {
        const String fileName = getAbsolutePath(shortName);
//        std::wcout<<fileName;
        for (std::deque<FileType>::const_iterator it = openedFiles.begin(); it!=openedFiles.end(); ++it) {
            const FileType & f = (*it);
            if (f.getName()==fileName) {
                Core::abort(Core::fromUtf8("Файл уже открыт: ")+fileName);
                return FileType();
            }
        }
#   ifdef NO_UNICODE
        const char * path = fileName.c_str();
#   else
#       if defined(WIN32) || defined(_WIN32)
        Encoding codec = CP866;
#       else
        Encoding codec = UTF8;
#       endif
        const std::string localName = Coder::encode(codec, fileName);
        const char * path = localName.c_str();
#   endif

        const char * fmode;
        if (mode==FileType::Read)
            fmode = "r";
        else if (mode==FileType::Write)
            fmode = "w";
        else if (mode==FileType::Append)
            fmode = "a";

        FILE* res = fopen(path, fmode);
        FileType f;
        if (res==0) {
            Core::abort(Core::fromUtf8("Невозможно открыть файл: ")+fileName);
        }
        else {
            if (mode==FileType::Append)
                mode = FileType::Write;
            f.setName(fileName);
            f.setMode(mode);
            if (remember) {
                openedFiles.push_back(f);
                openedFileHandles.push_back(res);
            }
            else if (fh) {
                *fh = res;
            }
        }
        return f;
    }
    inline static void close(const FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();
        std::deque<FILE*>::iterator it2 = openedFileHandles.begin();
        for (; it!=openedFiles.end(); ++it, ++it2) {
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
        FILE * fh = (*it2);
        f.invalidate();
        fclose(fh);
        openedFiles.erase(it);
        openedFileHandles.erase(it2);
    }

    inline static void reset(FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();
        std::deque<FILE*>::iterator it2 = openedFileHandles.begin();
        for (; it!=openedFiles.end(); ++it, ++it2) {
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
        FILE * fh = (*it2);
        fseek(fh, 0, 0);
    }
    inline static bool eof(const FileType & key) {
        std::deque<FileType>::iterator it = openedFiles.begin();
        std::deque<FILE*>::iterator it2 = openedFileHandles.begin();
        for (; it!=openedFiles.end(); ++it, ++it2) {
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
        FILE * fh = (*it2);
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
        std::deque<FILE*>::iterator it2 = openedFileHandles.begin();
        for (; it!=openedFiles.end(); ++it, ++it2) {
            const FileType & f = (*it);
            if (f==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return false;
        }
        FILE * fh = (*it2);
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
    static std::deque<FILE*> openedFileHandles;

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

    inline static void init() {}
    inline static void finalize() {}

    static String inputDelimeters;

    class OutputStream {
    public:
        OutputStream() { stream = false; file = 0; encoding = UTF8; buffer.reserve(100); }
        OutputStream(FILE * f, Encoding enc) {
            stream = true;
            file = f;
            if (enc==DefaultEncoding)
                encoding = UTF8;
            else
                encoding = enc;
        }
        inline const String & getBuffer() const { return buffer; }
        inline bool isStream() const { return stream; }
        void writeRawString(const String & s) {
            if (isStream()) {
                if (encoding==UTF8 && ftell(file)==0) {
                    static const char * BOM = "\xEF\xBB\xBF";
                    fwrite(BOM, sizeof(char), 3, file);
                }
                std::string bytes;
                try {
                    bytes = Coder::encode(encoding, s);
                }
                catch (...) {
                    Core::abort(Core::fromUtf8("Ошибка кодирования строки вывода: недопустимый символ"));
                }
                fwrite(bytes.c_str(), sizeof(char), bytes.length(), file);
            }
            else {
                buffer.append(s);
            }
        }

    private:
        bool stream;
        FILE * file;
        Encoding encoding;
        String buffer;

    };

    class InputStream {
    public:
        InputStream() {
            stream=false;
            file=0;
            encoding=UTF8;
            errStart=0;
            errLength=0;
            currentPosition=0;
        }
        InputStream(const String & b) {
            stream=false;
            file=0;
            encoding=UTF8;
            errStart=0;
            errLength=0;
            buffer=b;
            currentPosition=0;
        }
        InputStream(FILE * f, Encoding enc) {
            stream = true;
            file = f;
            if (enc==DefaultEncoding) {
                bool forceUtf8 = false;
                if (f!=stdin) {
                    long curpos = ftell(f);
                    fseek(f, 0, SEEK_SET);
                    char B[3];
                    if (fread(B, 1, 3, f)==3) {
                        forceUtf8 = B[0]==0xEF && B[1]==0xBB && B[2]==0xBF;
                    }
                    fseek(f, curpos, SEEK_SET);
                }
                if (forceUtf8)
                    encoding = UTF8;
                else
                    encoding = Core::getSystemEncoding();
            }
            else {
                encoding = enc;
            }
            errStart = 0;
            errLength = 0;
            currentPosition=0;
        }
        inline void getError(String & text, int & start, int & len) {
            text = error;
            start = errStart;
            len = errLength;
        }

        inline bool isStream() const { return stream; }
        inline void setError(const String & err) {
            if (isStream())
                Core::abort(err);
            else
                error = err;
        }
        inline bool hasError() const {
            if (isStream()) return Core::getError().length()>0;
            else return error.length()>0;
        }
        inline void markPossibleErrorStart() { errStart = currentPosition; errLength = 0; error.clear();}
        bool readRawChar(Char & x) {
            lastCharBuffer[0] = lastCharBuffer[1] = lastCharBuffer[2] = '\0';
            if (!stream) {
                if (currentPosition==buffer.length())
                    return false;
                x = buffer.at(currentPosition);
                currentPosition += 1;
                errLength += 1;
                return true;
            }
            else {
                if (feof(file))
                    return false;
                charptr buffer = reinterpret_cast<charptr>(&lastCharBuffer);
                if (encoding!=UTF8) {
                    // Read only one byte
                    lastCharBuffer[0] = fgetc(file);
                }
                else {
                    // More complex...
                    long cpos = ftell(file);
                    if (cpos==0) {
                        // Try to read BOM
                        static const char * BOM = "\xEF\xBB\xBF";
                        char firstThree[3];
                        bool seekBack = true;
                        if (fread(firstThree,sizeof(char),3,file)==3
                                && strncmp(BOM, firstThree, 3)==0)
                            seekBack = false;
                        if (seekBack)
                            fseek(file, 0, SEEK_SET);
                    }
                    lastCharBuffer[0] = fgetc(file);
                    uint8_t firstByte = lastCharBuffer[0];
                    uint8_t oneSymbMark = firstByte >> 5;
                    uint8_t twoSymbMark = firstByte >> 4;
                    if (firstByte==255 && file==Files::getAssignedIn()) {
                        Core::abort(Core::fromUtf8("Ошибка чтения данных: входной поток закончился"));
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
                        if (feof(file)) {
                            Core::abort(Core::fromUtf8("Ошибка чтения данных из файла: UTF-8 файл поврежден"));
                            return false;
                        }
                        lastCharBuffer[i+1] = fgetc(file);
                    }
                }
                const std::string sb(buffer);
                std::wstring res;
                try {
                    res = Coder::decode(encoding, sb);
                }
                catch (...) {
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
        void pushLastCharBack() {
            if (!stream) {
                currentPosition -= 1;
                errLength -= 1;
            }
            else {
                if (file==stdin) {
                    if (lastCharBuffer[2]!='\0')
                        ungetc(lastCharBuffer[2], file);
                    if (lastCharBuffer[1]!='\0')
                        ungetc(lastCharBuffer[1], file);
                    ungetc(lastCharBuffer[0], file);
                }
                else {
                    fseek(file, -1*strlen(lastCharBuffer), SEEK_CUR);
                }
            }
        }
        inline String readUntil(const String & delimeters) {
            String result;
            result.reserve(100);
            Char current;
            while (readRawChar(current)) {
                if (delimeters.find_first_of(current)!=String::npos) {
                    pushLastCharBack();
                    break;
                }
                else {
                    result.push_back(current);
                }
            }
            return result;
        }
        inline void skipDelimiters(const String & delim) {
            // Skip delimiters until lexem
            Char skip(32);
            while (readRawChar(skip)) {
                if (delim.find_first_of(skip)==String::npos) {
                    pushLastCharBack();
                    break;
                }
            }
        }

    private:
        bool stream;
        FILE * file;
        Encoding encoding;
        String buffer;
        String error;
        int errStart;
        int errLength;
        int currentPosition;
        char lastCharBuffer[3];
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


    // Actual functions to be in use while input from stream

#if !defined(LOCALE_ENCODING)
#   if defined(WIN32) || defined(_WIN32)
#       define LOCALE_ENCODING CP866
#   else
#       define LOCALE_ENCODING UTF8
#   endif
#endif

    static InputStream makeInputStream(FileType fileNo, bool fromStdIn) {
        if (fromStdIn) {
            return InputStream(Files::getAssignedIn(), LOCALE_ENCODING);
        }
        else {
            std::deque<FileType>::iterator it = Files::openedFiles.begin();
            std::deque<FILE*>::iterator it2 = Files::openedFileHandles.begin();
            for ( ; it!=Files::openedFiles.end(); ++it, ++it2) {
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
            return InputStream(*it2, Files::fileEncoding);
        }
    }

    inline static OutputStream makeOutputStream(FileType fileNo, bool toStdOut) {
      //  std::cout<<fileNo.fullPath;
        if (toStdOut) {
            return OutputStream(Files::getAssignedOut(), LOCALE_ENCODING);
        }
        else {
            std::deque<FileType>::iterator it = Files::openedFiles.begin();
            std::deque<FILE*>::iterator it2 = Files::openedFileHandles.begin();
            for ( ; it!=Files::openedFiles.end(); ++it, ++it2) {
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
            return OutputStream(*it2, Files::fileEncoding);
        }
    }

    inline static int readInteger(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readInteger(stream);
    }

    inline static real readReal(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readReal(stream);
    }

    inline static bool readBool(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readBool(stream);
    }

    inline static Char readChar(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readChar(stream);
    }
    inline static String readString(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readString(stream);
    }
    inline static String readLine(FileType fileNo = FileType(), bool fromStdIn = true) {
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
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

#ifdef NO_SYSTEM
// Dummy class implementation
class System {
public:
    inline static void init() {}
    inline static void finalize() {}

    inline static int time() { return 0; }
};
#else
#include <time.h>
#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif
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
#endif

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
std::deque<FileType> Files::openedFiles;
std::deque<FILE*> Files::openedFileHandles;
FILE * Files::assignedIN = stdin;
FILE * Files::assignedOUT = stdout;
Encoding Files::fileEncoding;
String Kumir::IO::inputDelimeters = Kumir::Core::fromAscii(" \n\t");
#endif

}

#endif // KUMIRSTDLIB_H
