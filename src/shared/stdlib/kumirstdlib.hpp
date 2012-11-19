#ifndef KUMIRSTDLIB_H
#define KUMIRSTDLIB_H

#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <deque>
#include <map>
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
#       error Not implemented
#   else
#       include <sys/stat.h>
#       include <fcntl.h>
#       include <unistd.h>
#       include <string.h>
#       include <errno.h>
#   endif
#endif

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
                result.push_back(ch+0x30);
            }
            else
                result.push_back(ch);
        }
        return result;
    }

protected:
    inline static void abort(const String & err) {
        error = err;
    }
    static String error;
};

class Math {
public:
    inline static void init() {}
    inline static void finalize() {}

    inline static real abs(real x) { return ::fabs(x); }

    template <typename T> inline static T max(T x, T y) { return x>y? x : y; }
    template <typename T> inline static T min(T x, T y) { return x<y? x : y; }

    inline static int iabs(int x) { return x>0? x : -x; }
    inline static int intt(real x) { return static_cast<int>(x); }
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
        if (b>=0) {
            return ::pow(a, b);
        }
        else {
            Core::abort(Core::fromUtf8("Нельзя возводить в отрицательную степень"));
            return 0.0;
        }
    }
    inline static int ipow(int a, int b) {
        if (b>=0) {
            return static_cast<int>(::floor(::pow(a, b)));
        }
        else {
            Core::abort(Core::fromUtf8("Нельзя возводить в отрицательную степень"));
            return 0.0;
        }
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
    inline static void init() { srand(time(NULL)); }
    inline static void finalize() {}

    template <typename T> inline static T rand(T a, T b) {
        if (a>b) {
            Core::abort(Core::fromUtf8("Неверный диапазон чисел"));
            return 0;
        }
        else {
            int rndValue = ::rand(); // in range [0..2^32]
            real scale = static_cast<real>(b-a+1)/static_cast<real>(RAND_MAX);
            return Kumir::Math::min(b, a+static_cast<T>(scale*rndValue));
        }
    }
    template <typename T> inline static T rnd(T x) {
        int rndValue = ::rand();
        real scale = static_cast<real>(x)/static_cast<real>(RAND_MAX);
        return Kumir::Math::min(x, 1+static_cast<T>(scale*rndValue));
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
        BadSymbol,
        Overflow
    };

    inline static void init() {}
    inline static void finalize() {}

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

    static int parseReal(String word, Char dot, ParseError & error) {
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
        int integral = 0;
        int fractional = 0;
        int iexponenta = 0;
        real fraction = 0.0;
        String sIntegral, sFractional, sExponenta;
        sIntegral.reserve(30);
        sFractional.reserve(30);
        sExponenta.reserve(30);
        bool dotFound = false;
        bool eFound = false;
        static const String E = Core::fromUtf8("eEеЕ"); // includes cyrillic 'e'
        for (size_t i=pos; i<word.length(); i++) {
            Char ch = word.at(i);
            if (!dotFound) {
                // parse integral part of mantissa
                if (ch==dot || E.find_first_of(ch)!=String::npos) {
                    if (sIntegral.length()>0) {
                        integral = parseInt(sIntegral, 10, error);
                        if (error!=NoError) return 0.0;
                    }
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
                        fractional = parseInt(sFractional, 10, error);
                        if (error!=NoError) return 0.0;
                    }
                    if (fractional<0) {
                        error = BadSymbol;
                        return 0.0;
                    }
                    eFound = true;
                }
            }
            else {
                // parse exponenta
                sExponenta.push_back(ch);
            }
        }
        if (sExponenta.length()>0) {
            iexponenta = parseInt(sExponenta, 10, error);
            if (error!=NoError) return 0.0;
        }
        real fractionalLength = static_cast<real>(sFractional.length());
        for (size_t i=sFractional.length()-1; i>=0; i--) {
            Char ch = sFractional.at(i);
            if (ch==Char('0'))
                fractionalLength -= 1.0;
        }
        fraction = static_cast<real>(fractional)/Math::pow(10, fractionalLength);
        mantissa = static_cast<real>(integral) + fraction;
        exponenta = static_cast<real>(iexponenta);
        real result = mantissa * Math::pow(10, exponenta);
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
        char buffer[50];
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
        int result = parseReal(str, '.', error);
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
};

#ifdef NO_FILES
// Dummy files implementation
class Files {
    friend class IO;
public:
    inline static void init() {}
    inline static void finalize() {}

    inline static bool exist(const String & ) { return false; }
    inline static bool unlinkFile(const String & ) { return false; }

    inline static int open(const String & , const char * ) { return 0; }
    inline static void close(int ) {}
    inline static void reset(int ) {}
    inline static bool eof(int ) { return true; }

    inline static bool readBool(int, const String &) { return false; }
    inline static int readInt(int, const String &) { return 0; }
    inline static real readReal(int, const String &) { return 0; }
    inline static Char readChar(int, const String &) { return Char(0); }
    inline static String readString(int, const String &) { return String(); }

    inline static void writeBool(int,bool, const String &) {}
    inline static void writeInt(int,int, const String &) {}
    inline static void writeReal(int,real, const String &) {}
    inline static void writeChar(int,Char, const String &) {}
    inline static void writeString(int,const String &, const String &) {}
};
#else
class Files {
    friend class IO;
public:
    enum FileMode { FM_Read, FM_Write, FM_Append };
    inline static bool isOpenedFiles() { return openedFiles.size()> 0; }
    inline static void init() {
        fileEncoding = UTF8;
    }
    inline static void finalize() {
        if (isOpenedFiles() && Core::getError().length()==0)
            Core::abort(Core::fromUtf8("Остались не закрытые файлы"));
        for (std::list<File>::const_iterator it = openedFiles.begin();
             it != openedFiles.end(); ++it)
        {
            const File & f = (*it);
            fclose(f.d);
        }
        openedFiles.clear();
    }

    inline static void setFileEncoding(const String & enc) {
        String encoding = Core::toLowerCaseW(enc);
        size_t minus = encoding.find_first_of(Char('-'));
        if (minus!=String::npos) {
            encoding.erase(minus, 1);
        }
        static const String ansi1 = Core::fromAscii("cp1251");
        static const String ansi2 = Core::fromAscii("windows1251");
        static const String ansi3 = Core::fromAscii("windows");
        static const String ansi4 = Core::fromAscii("ansi");
        static const String oem1 = Core::fromAscii("cp866");
        static const String oem2 = Core::fromAscii("ibm866");
        static const String oem3 = Core::fromAscii("ibm");
        static const String oem4 = Core::fromAscii("oem");
        static const String koi1 = Core::fromAscii("koi8");
        static const String koi2 = Core::fromAscii("koi8r");
        static const String koi3 = Core::fromUtf8("кои8");
        static const String koi4 = Core::fromUtf8("кои8р");
        static const String utf1 = Core::fromAscii("utf");
        static const String utf2 = Core::fromAscii("utf8");
        static const String utf3 = Core::fromAscii("unicode");
        static const String utf4 = Core::fromAscii("linux");
        if (encoding==ansi1 || encoding==ansi2 || encoding==ansi3 || encoding==ansi4) {
            fileEncoding = CP1251;
        }
        else if (encoding==oem1 || encoding==oem2 || encoding==oem3 || encoding==oem4) {
            fileEncoding = CP866;
        }
        else if (encoding==koi1 || encoding==koi2 || encoding==koi3 || encoding==koi4) {
            fileEncoding = KOI8R;
        }
        else if (encoding==utf1 || encoding==utf2 || encoding==utf3 || encoding==utf4) {
            fileEncoding = UTF8;
        }
        else {
            Core::abort(Core::fromUtf8("Неизвестная кодировка"));
        }
    }

#if defined(WIN32) || defined(_WIN32)
    inline static bool exist(const String & ) {
# error not implemented
    }
    inline static int unlinkFile(const String & ) {
# error not implemented
    }
    inline static int open(const String & , const char * ) {
# error not implemented
    }
#else
    inline static bool exist(const String & fileName) {
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

    inline static int open(const String & fileName, FileMode mode) {
        for (std::list<File>::iterator it = openedFiles.begin(); it!=openedFiles.end(); ++it) {
            const File & f = (*it);
            if (f.name==fileName) {
                Core::abort(Core::fromUtf8("Файл уже открыт: ")+fileName);
                return -1;
            }
        }
        char * path;
#   ifdef NO_UNICODE
        path = const_cast<char*>(fileName.c_str());
#   else
        path = reinterpret_cast<char*>( calloc(fileName.length()*2+1, sizeof(char)) );
        size_t pl = wcstombs(path, fileName.c_str(), fileName.length()*2+1);
        path[pl] = '\0';
#   endif

        const char * fmode;
        if (mode==FM_Read)
            fmode = "r";
        else if (mode==FM_Write)
            fmode = "w";
        else if (mode==FM_Append)
            fmode = "a";

        FILE * res = fopen(path, fmode);
        File f;
        f.key = -1;
        if (res==0) {
            Core::abort(Core::fromUtf8("Невозможно открыть файл: ")+fileName);
        }
        else {
            if (mode==FM_Append)
                mode = FM_Write;
            f.name = fileName;
            f.key = openedFiles.size();
            f.mode = mode;
            f.d = res;
            openedFiles.push_back(f);
        }
#   ifndef NO_UNICODE
        free(path);
#   endif
        return f.key;
    }
#endif
    inline static void close(int key) {
        std::list<File>::iterator it = openedFiles.begin();
        for (; it!=openedFiles.end(); ++it) {
            const File & f = (*it);
            if (f.key==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return;
        }
        fclose(it->d);
        openedFiles.erase(it);
    }

    inline static void reset(int key) {
        std::list<File>::iterator it = openedFiles.begin();
        for (; it!=openedFiles.end(); ++it) {
            const File & f = (*it);
            if (f.key==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return;
        }
        const File & f = (*it);
        fseek(f.d, 0, 0);
    }
    inline static bool eof(int key) {
        std::list<File>::iterator it = openedFiles.begin();
        for (; it!=openedFiles.end(); ++it) {
            const File & f = (*it);
            if (f.key==key) {
                break;
            }
        }
        if (it==openedFiles.end()) {
            Core::abort(Core::fromUtf8("Неверный ключ"));
            return false;
        }
        const File & f = (*it);
        return feof(f.d)>0? true : false;
    }


private:
    struct File {
        int key;
        String name;
        FileMode mode;
        FILE * d;
    };

    static std::list<File> openedFiles;

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

#endif

class IO {
public:

    inline static void init() {}
    inline static void finalize() {}

    static String outputDelimiter;
    static String inputDelimeters;
    static bool spaceIsDelimeterToo;
    enum TextAlignment { Left, Right, Center };

    struct IntFormat {
        char base;
        int width;
        TextAlignment align;
        IntFormat() { base = 10; width = 0; align = Left; }
    };

    struct RealFormat {
        Char delimeter;
        bool exp;
        int width;
        int decs;
        TextAlignment align;
        RealFormat() { delimeter = Core::fromAscii(".").at(0); exp=false; width=0; decs=0; align=Left; }
    };

    struct BoolFormat {
        std::deque<String> yes;
        std::deque<String> no;
        BoolFormat() {
            yes.push_back(Core::fromUtf8("да"));
            yes.push_back(Core::fromUtf8("Да"));
            yes.push_back(Core::fromUtf8("ДА"));
            yes.push_back(Core::fromUtf8("1"));
            no.push_back(Core::fromUtf8("нет"));
            no.push_back(Core::fromUtf8("Нет"));
            no.push_back(Core::fromUtf8("НЕТ"));
            no.push_back(Core::fromUtf8("0"));
        }
    };

    struct CharFormat {
        bool raw;
        CharFormat() { raw = false; }
    };

    struct StringFormat {
        enum Lexem {
            Word, Literal, Line
        } lexem;
        StringFormat() { lexem = Word; }
    };

    class OutputStream {
    public:
        OutputStream() { stream = false; file = 0; encoding = UTF8; buffer.reserve(100); }
        OutputStream(FILE * f, Encoding enc) { stream = true; file = f; encoding = enc; }
        inline const String & getBuffer() const { return buffer; }
        inline bool isStream() const { return stream; }
        void writeRawString(const String & s) {
            if (isStream()) {
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
            encoding = enc;
            errStart = 0;
            errLength = 0;
            currentPosition=0;
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
                    lastCharBuffer[0] = fgetc(file);
                    int extraBytes = 0;
                    if ( (lastCharBuffer[0] >> 5) & 0x06 )
                        extraBytes = 1;
                    else if ( (lastCharBuffer[0] >> 4) & 0x0E ) {
                        extraBytes = 2;
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
                if (lastCharBuffer[2]!='\0')
                    ungetc(lastCharBuffer[2], file);
                if (lastCharBuffer[1]!='\0')
                    ungetc(lastCharBuffer[1], file);
                ungetc(lastCharBuffer[0], file);
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
        inline String skipDelimiters(const String & delim) {
            // Skip delimiters until lexem
            Char skip(32);
            while (readRawChar(skip)) {
                if (delim.find_first_of(skip)!=String::npos) {
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
        if (spaceIsDelimeterToo)
            delim += Core::fromAscii(" \t\n\r");
        is.skipDelimiters(delim);
        // Mark as lexem begin position
        is.markPossibleErrorStart();
        return is.readUntil(delim);
    }

    static String readLiteral(InputStream & is) {
        String delim = inputDelimeters;
        if (spaceIsDelimeterToo)
            delim += Core::fromAscii(" \t\n\r");
        is.skipDelimiters(delim);
        // Mark as lexem begin position
        is.markPossibleErrorStart();
        Char bracket;
        if (!is.readRawChar(bracket)) {
            is.setError(Core::fromUtf8("Не могу прочитать литерал: текст закончился"));
            return String();
        }
        if (bracket!=Char('\'') && bracket!=Char('"')) {
            is.setError(Core::fromUtf8("Ошибка чтения литерала: литерал начинается не с кавычки"));
            return String();
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
            is.setError(Core::fromUtf8("Ошибка чтения литерала: текст закончился раньше, чем появилась закрывающая кавычка"));
        }
        // Skip closing bracket
        is.readRawChar(bracket);
        return result;
    }

    static String readLine(InputStream & is) {
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

public:
    // Generic functions to be in use while input from GUI
    inline static String readString(InputStream & is, const StringFormat & format) {
        if (format.lexem==StringFormat::Word)
            return readLine(is);
        else if (format.lexem==StringFormat::Literal)
            return readLiteral(is);
        else
            return readLine(is);
    }
    static Char readChar(InputStream & is, const CharFormat & format) {
        Char result(0);
        if (is.hasError()) return result;
        if (format.raw) {
            bool ok;
            ok = is.readRawChar(result);
            if (!ok)
                is.setError(Core::fromUtf8("Ошибка ввода символа: текст закончился"));
        }
        else {
            String word = readWord(is);
            if (is.hasError()) return result;
            if (word.length()==0)
                is.setError(Core::fromUtf8("Ошибка ввода символа: текст закончился"));
            else if (word.length()>1)
                is.setError(Core::fromUtf8("Ошибка ввода: введено больше одного символа"));
            else
                result = word.at(0);
        }
        return result;
    }
    inline static int readInteger(InputStream & is, const IntFormat & format) {
        String word = readWord(is);
        if (is.hasError()) return 0;
        Converter::ParseError error = Converter::NoError;
        int result = Converter::parseInt(word, format.base, error);
        if (error==Converter::EmptyWord) {
            is.setError(Core::fromUtf8("Ошибка ввода целого числа: текст закончился"));
        }
        else if (error==Converter::WrongHex) {
            is.setError(Core::fromUtf8("Ошибка ввода целого числа: неверное 16-ричное число"));
        }
        else if (error==Converter::BadSymbol) {
            is.setError(Core::fromUtf8("Ошибка ввода целого числа: число содержит неверный символ"));
        }
        else if (error==Converter::Overflow) {
            is.setError(Core::fromUtf8("Ошибка ввода: слишком большое целое число"));
        }
        return result;
    }
    inline static real readReal(InputStream & is, const RealFormat & format) {
        String word = readWord(is);
        if (is.hasError()) return 0;
        Converter::ParseError error = Converter::NoError;
        real result = Converter::parseReal(word, format.delimeter, error);
        if (error==Converter::EmptyWord) {
            is.setError(Core::fromUtf8("Ошибка ввода вещественного числа: текст закончился"));
        }
        else if (error==Converter::BadSymbol) {
            is.setError(Core::fromUtf8("Ошибка ввода вещественного числа: число содержит неверный символ"));
        }
        else if (error==Converter::Overflow) {
            is.setError(Core::fromUtf8("Ошибка ввода: слишком большое вещественное число"));
        }
        return result;
    }
    inline static bool readBool(InputStream & is, const BoolFormat & format) {
        String word = readWord(is);
        if (is.hasError()) return 0;
        if (word.length()==0) {
            is.setError(Core::fromUtf8("Ошибка ввода логического значения: ничего не введено"));
        }
        bool yes = false;
        bool no = false;
        for (size_t i=0; i<format.yes.size(); i++) {
            if (format.yes[i]==word) {
                yes = true; break;
            }
        }
        for (size_t i=0; i<format.no.size(); i++) {
            if (format.no[i]==word) {
                no = true; break;
            }
        }
        if (!yes && !no)
        {
            is.setError(Core::fromUtf8("Ошибка ввода логического значения: неизвестное значение"));
        }
        return yes;
    }

    inline static void writeString(OutputStream & os, const String & str, const StringFormat & format) {
        String data;
        if (format.lexem==StringFormat::Literal) {
            data.reserve(str.length()+2+outputDelimiter.length());
            data.push_back(Char('"'));
            data.append(str);
            data.push_back(Char('"'));
            data.append(outputDelimiter);
        }
        else if (format.lexem==StringFormat::Word) {
            data.reserve(str.length()+outputDelimiter.length());
            data.append(str);
            data.append(outputDelimiter);
        }
        else {
            data = str;
        }
        os.writeRawString(data);
    }

    inline static void writeChar(OutputStream & os, const Char & chr, const CharFormat & format) {
        String data;
        if (format.raw) {
            data.push_back(chr);
        }
        else {
            data.reserve(3+outputDelimiter.length());
            if (chr==Char('\''))
                data.push_back(Char('"'));
            else
                data.push_back(Char('\''));
            data.push_back(chr);
            if (chr==Char('\''))
                data.push_back(Char('"'));
            else
                data.push_back(Char('\''));
            data.append(outputDelimiter);
        }
        os.writeRawString(data);
    }

    inline static void writeInteger(OutputStream & os, int value, const IntFormat & format) {
        char al;
        if (format.align==Right) al = 'r';
        else if (format.align==Center) al = 'c';
        else al = 'l';
        const String strval = Converter::sprintfInt(value, format.base, format.width, al);
        String data;
        data.reserve(strval.length()+outputDelimiter.length());
        data.append(strval);
        data.append(outputDelimiter);
        os.writeRawString(data);
    }

    inline static void writeReal(OutputStream & os, real value, const RealFormat & format) {
        char al;
        if (format.align==Right) al = 'r';
        else if (format.align==Center) al = 'c';
        else al = 'l';
        const String strval = Converter::sprintfReal(value, format.delimeter, format.exp, format.width, format.decs, al);
        String data;
        data.reserve(strval.length()+outputDelimiter.length());
        data.append(strval);
        data.append(outputDelimiter);
        os.writeRawString(data);
    }

    inline static void writeBool(OutputStream & os, bool value, const BoolFormat & format) {
        String data;
        const String & sval = value? format.yes.at(0) : format.no.at(0);
        data.reserve(outputDelimiter.length()+sval.length());
        data.append(sval);
        data.append(outputDelimiter);
        os.writeRawString(data);
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

    static IntFormat parseIntFormat(const String & fmt) {
        const StringList lexems = splitIntoLexemsByDelimeter(fmt);
        IntFormat result;
        Converter::ParseError error = Converter::NoError;
        if (lexems.size()>=1) {
            if (lexems[0].length()>0) {
                int base = Converter::parseInt(lexems[0], 10, error);
                if (error!=Converter::NoError || (base!=2 && base!=8 && base!=10 && base!=16)) {
                    Core::abort(Core::fromUtf8("Ошибка в описании формата целого числа: неверное основание системы исчисленния"));
                    return result;
                }
                result.base = static_cast<char>(base);
            }
        }
        if (lexems.size()>=2) {
            int width = Converter::parseInt(lexems[1], 10, error);
            if (error!=Converter::NoError || width<0) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата целого числа: неверная ширина элемента вывода"));
                return result;
            }
            result.width = width;
        }
        static const String LEFT = Core::fromUtf8("LlЛл");
        static const String RIGHT = Core::fromUtf8("RrПп");
        static const String CENTER = Core::fromUtf8("CcЦц");
        if (lexems.size()>=3) {
            const String word = lexems[2];
            if (word.length()==0) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата целого числа: не указано выравнивание"));
                return result;
            }
            if (word.length()>1) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата целого числа: неверно указано выравнивание"));
                return result;
            }
            Char lett = word[0];
            if (LEFT.find_first_of(lett)==String::npos && RIGHT.find_first_of(lett)==String::npos && CENTER.find_first_of(lett)==String::npos) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата целого числа: неверно указано выравнивание"));
                return result;
            }
            if (RIGHT.find_first_of(word[0])!=String::npos) {
                result.align = Right;
            }
            else if (CENTER.find_first_of(word[0])!=String::npos) {
                result.align = Center;
            }
            else {
                result.align = Left;
            }
        }
        return result;
    }

    static RealFormat parseRealFormat(const String & fmt) {
        const StringList lexems = splitIntoLexemsByDelimeter(fmt);
        RealFormat result;
        static const String EEEE = Core::fromUtf8("EeЕе");
        if (lexems.size()>=1) {
            const String word = lexems[0];
            if (word.length()>0) {
                if (word.length()>2 || (word.length()==2 && EEEE.find_first_of(word[1])==String::npos)) {
                    Core::abort(Core::fromUtf8("Ошибка в описании формата вещественного числа: неверный разделитель разрядов"));
                    return result;
                }
                if (word.length()==2) {
                    result.delimeter = word[0];
                    result.exp = word.length()==2;
                }
                else if (EEEE.find_first_of(word[0])==String::npos) {
                    result.delimeter = word[0];
                }
                else {
                    result.exp = true;
                }
            }
        }

        Converter::ParseError error = Converter::NoError;
        if (lexems.size()>=2) {
            int width = Converter::parseInt(lexems[1], 10, error);
            if (error!=Converter::NoError || width<0) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата вещественного числа: неверная ширина элемента вывода"));
                return result;
            }
            result.width = width;
        }
        if (lexems.size()>=3) {
            int decs = Converter::parseInt(lexems[2], 10, error);
            if (error!=Converter::NoError || decs<0) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата вещественного числа: неверное число символов дробной части"));
                return result;
            }
            result.decs = decs;
        }
        static const String LEFT = Core::fromUtf8("LlЛл");
        static const String RIGHT = Core::fromUtf8("RrПп");
        static const String CENTER = Core::fromUtf8("CcЦц");
        if (lexems.size()>=4) {
            const String word = lexems[3];
            if (word.length()==0) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата вещественного числа: не указано выравнивание"));
                return result;
            }
            if (word.length()>1) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата вещественного числа: неверно указано выравнивание"));
                return result;
            }
            Char lett = word[0];
            if (LEFT.find_first_of(lett)==String::npos && RIGHT.find_first_of(lett)==String::npos && CENTER.find_first_of(lett)==String::npos) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата вещественного числа: неверно указано выравнивание"));
                return result;
            }
            if (RIGHT.find_first_of(word[0])!=String::npos) {
                result.align = Right;
            }
            else if (CENTER.find_first_of(word[0])!=String::npos) {
                result.align = Center;
            }
            else {
                result.align = Left;
            }
        }
        return result;
    }

    static BoolFormat parseBoolFormat(const String & fmt) {
        BoolFormat result;
        const StringList alist = splitIntoLexemsByDelimeter(fmt, ',');
        if (alist.size()>0) {
            // clear default values
            result.yes.clear();
            result.no.clear();
        }
        for (size_t i=0; i<alist.size(); i++) {
            const String element = alist[i];
            const StringList pair = splitIntoLexemsByDelimeter(element,'/');
            if (pair.size()!=2) {
                Core::abort(Core::fromUtf8("Ошибка в описании формата логического значения"));
                return result;
            }
            result.yes.push_back(pair[0]);
            result.no.push_back(pair[1]);
        }
        return result;
    }

    static CharFormat parseCharFormat(const String & fmt) {
        static const String RAW1 = Core::fromUtf8("подряд");
        static const String RAW2 = Core::fromUtf8("Подряд");
        static const String RAW3 = Core::fromUtf8("ПОДРЯД");
        CharFormat result;
        result.raw = fmt==RAW1 || fmt==RAW2 || fmt==RAW3;
        if (!result.raw && fmt.length()>0)
            Core::abort(Core::fromUtf8("Неверное описание формата символа"));
        return result;
    }

    static StringFormat parseStringFormat(const String & fmt) {
        static const String WORD1 = Core::fromUtf8("слово");
        static const String WORD2 = Core::fromUtf8("Слово");
        static const String WORD3 = Core::fromUtf8("СЛОВО");
        static const String LITERAL1 = Core::fromUtf8("литерал");
        static const String LITERAL2 = Core::fromUtf8("Литерал");
        static const String LITERAL3 = Core::fromUtf8("ЛИТЕРАЛ");
        static const String LINE1 = Core::fromUtf8("строка");
        static const String LINE2 = Core::fromUtf8("Строка");
        static const String LINE3 = Core::fromUtf8("СТРОКА");
        StringFormat result;
        unsigned char word = (fmt==WORD1 || fmt==WORD2 || fmt==WORD3)? 1 : 0;
        unsigned char literal = (fmt==LITERAL1 || fmt==LITERAL2 || fmt==LITERAL3)? 1 : 0;
        unsigned char line = (fmt==LINE1 || fmt==LINE2 || fmt==LINE3)? 1 : 0;
        unsigned char summ = word+literal+line;
        if ( (summ==0 && fmt.length()>0) || summ>1 ) {
            Core::abort(Core::fromUtf8("Неверное описание формата литерала"));
        }
        if (word)
            result.lexem = StringFormat::Word;
        else if (literal)
            result.lexem = StringFormat::Literal;
        else if (line)
            result.lexem = StringFormat::Line;
        return result;
    }

    // Actual functions to be in use while input from stream

#if !defined(ONEBYTE_LOCALE)
#   if defined(WIN32) || defined(_WIN32)
#       define LOCALE_ENCODING CP866
#   else
#       define LOCALE_ENCODING UTF8
#   endif
#else
#   define LOCALE_ENCODING ##ONEBYTE_LOCALE##
#endif

    static InputStream makeInputStream(int fileNo, bool fromStdIn) {
        if (fromStdIn) {
            return InputStream(stdin, LOCALE_ENCODING);
        }
        else {
            std::list<Files::File>::const_iterator it = Files::openedFiles.begin();
            for ( ; it!=Files::openedFiles.end(); ++it) {
                if (it->key==fileNo) {
                    break;
                }
            }
            if (it==Files::openedFiles.end()) {
                Core::abort(Core::fromUtf8("Файл с таким ключем не открыт"));
                return InputStream();
            }
            const Files::File file = (*it);
            if (file.mode!=Files::FM_Read) {
                Core::abort(Core::fromUtf8("Файл с таким ключем открыт на запись"));
                return InputStream();
            }
            return InputStream(file.d, Files::fileEncoding);
        }
    }

    inline static OutputStream makeOutputStream(int fileNo, bool toStdOut) {
        if (toStdOut) {
            return OutputStream(stdout, LOCALE_ENCODING);
        }
        else {
            const std::list<Files::File> & fs = Files::openedFiles;
            std::list<Files::File>::const_iterator it = fs.begin();
            for ( ; it!=fs.end(); ++it) {
                if (it->key==fileNo) {
                    break;
                }
            }
            if (it==fs.end()) {
                Core::abort(Core::fromUtf8("Файл с таким ключем не открыт"));
                return OutputStream();
            }
            const Files::File file = (*it);
            if (file.mode==Files::FM_Read) {
                Core::abort(Core::fromUtf8("Файл с таким ключем открыт на чтение"));
                return OutputStream();
            }
            return OutputStream(file.d, Files::fileEncoding);
        }
    }

    inline static int readInteger(const String & format, int fileNo = -1, bool fromStdIn = true) {
        IntFormat fmt = parseIntFormat(format);
        if (Core::getError().length()>0) return 0;
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readInteger(stream, fmt);
    }

    inline static real readReal(const String & format, int fileNo = -1, bool fromStdIn = true) {
        RealFormat fmt = parseRealFormat(format);
        if (Core::getError().length()>0) return 0;
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readReal(stream, fmt);
    }

    inline static bool readBool(const String & format, int fileNo = -1, bool fromStdIn = true) {
        BoolFormat fmt = parseBoolFormat(format);
        if (Core::getError().length()>0) return 0;
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readBool(stream, fmt);
    }

    inline static Char readChar(const String & format, int fileNo = -1, bool fromStdIn = true) {
        CharFormat fmt = parseCharFormat(format);
        if (Core::getError().length()>0) return 0;
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readChar(stream, fmt);
    }
    inline static String readString(const String & format, int fileNo = -1, bool fromStdIn = true) {
        StringFormat fmt = parseStringFormat(format);
        if (Core::getError().length()>0) return 0;
        InputStream stream = makeInputStream(fileNo, fromStdIn);
        if (Core::getError().length()>0) return 0;
        return readString(stream, fmt);
    }

    inline static void writeInteger(const String & format, int value, int fileNo = -1, bool toStdOut = true) {
        IntFormat fmt = parseIntFormat(format);
        if (Core::getError().length()>0) return;
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeInteger(stream, value, fmt);
    }

    inline static void writeReal(const String & format, real value, int fileNo = -1, bool toStdOut = true) {
        RealFormat fmt = parseRealFormat(format);
        if (Core::getError().length()>0) return;
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeReal(stream, value, fmt);
    }
    inline static void writeBool(const String & format, bool value, int fileNo = -1, bool toStdOut = true) {
        BoolFormat fmt = parseBoolFormat(format);
        if (Core::getError().length()>0) return;
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeBool(stream, value, fmt);
    }
    inline static void writeString(const String & format, const String & value, int fileNo = -1, bool toStdOut = true) {
        StringFormat fmt = parseStringFormat(format);
        if (Core::getError().length()>0) return;
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeString(stream, value, fmt);
    }
    inline static void writeChar(const String & format, Char value, int fileNo = -1, bool toStdOut = true) {
        CharFormat fmt = parseCharFormat(format);
        if (Core::getError().length()>0) return;
        OutputStream stream = makeOutputStream(fileNo, toStdOut);
        if (Core::getError().length()>0) return;
        writeChar(stream, value, fmt);
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
class System {
public:
    inline static void init() {}
    inline static void finalize() {}

    inline static int time() {
        return 0; // TODO implement me
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
std::list<Files::File> Files::openedFiles;
Encoding Files::fileEncoding;
String Kumir::IO::outputDelimiter = Kumir::Core::fromAscii("");
String Kumir::IO::inputDelimeters = Kumir::Core::fromAscii(",");
bool Kumir::IO::spaceIsDelimeterToo = true;
#endif

}

#endif // KUMIRSTDLIB_H
