#ifndef KUMIRSTDLIB_H
#define KUMIRSTDLIB_H

#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include "coder_unicode.hpp"

#ifndef NO_SYSTEM
#   include <time.h>
#endif

#ifdef NO_UNICODE
#include <string>
#define CS(x) "x"
#define CC(x) 'x'
#   if not defined(ONEBYTE_LOCALE)
#       define ONEBYTE_LOCALE CP866
#   endif
#else
#include <wchar.h>
#include <string>
#define CS(x) L"x"
#define CC(x) L'x'
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

#define KASSERT(statement, success, fail, message) \
    if (!statement) { Core::abort(message); return fail; } else { return success; }

class Core {
    friend class Math;
    friend class Random;
    friend class Files;
    friend class StringUtils;
    friend class System;
public:
    inline static void init() {}
    inline static void finalize() {}
    inline static const String & getError() { return error; }
    inline static void reset() { error.clear(); }
protected:
    inline static void abort(const String & err) {
        error = err;
    }
    static String error;
};


String Core::error = CS();

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
        KASSERT(x<=1.0, ::acos(x), 0.0, CS(Bad inverse trigonometric argument))
    }
    inline static real arcctg(real x) { return ::atan(1.0/x);}
    inline static real arcsin(real x) {
        KASSERT(x<=1.0, ::asin(x), 0.0, CS(Bad inverse trigonometric argument))
    }
    inline static real arctg(real x) { return ::atan(x); }

    inline static real cos(real x) { return ::cos(x); }
    inline static real sin(real x) { return ::sin(x); }
    inline static real tg(real x) { return ::tan(x); }
    inline static real ctg(real x) {
        KASSERT(x!=0.0, 1.0/::tan(x), 0.0, CS(Bad trigonometric argument))
    }
    inline static real lg(real x) {
        KASSERT(x>0.0, ::log(x)/::log(10.0), 0.0, CS(Lg function argument must be > 0))
    }
    inline static real ln(real x) {
        KASSERT(x>0.0, ::log(x), 0.0, CS(Ln function argument must be > 0))
    }
    inline static real exp(real x) { return ::exp(x); }

    inline static real sqrt(real x) {
        KASSERT(x>=0.0, ::sqrt(x), 0.0, CS(Square root of negative value))
    }
    inline static real pow(real a, real b) {
        KASSERT(a>=0 && b>=0.0, ::pow(a, b), 0.0, CS(Can not power to less than zero))
    }
    inline static int ipow(int a, int b) {
        KASSERT(a>=0 && b>=0.0, static_cast<int>(::floor(::pow(a, b))), 0, CS(Can not power to less than zero))
    }
    template <typename A, typename B> inline static real safediv(A a, B b) {
#ifdef NO_ZERODIV_CHECK
        return static_cast<real>(a) / static_cast<real>(b);
#else
        KASSERT(b!=0, static_cast<real>(a)/static_cast<real>(b), 0.0, CS(Division by zero))
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
            Core::abort(CS(Division by not natural number));
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
            Core::abort(CS(Division by not natural number));
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
            Core::abort(CS(Invalid range));
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
    inline static void init() {}
    inline static void finalize() {}

    template <typename T> inline static T stringToValue(const String & str, bool & ok)
    {
        StringStream ss(str);
        T value;
        ss >> value;
        ok = !ss.fail();
    }
    template <typename T> inline static String valueToString(T value)
    {
        StringStream ss;
        ss << value;
        return ss.str();
    }
};

class StringUtils {
public:
    inline static void init() {}
    inline static void finalize() {}
    inline static int length(const String & str) { return str.length(); }
#ifdef NO_UNICODE
    inline static int code(char ch) {
        int value = 0;
        uint32_t univalue = 0;
        const std::string buf(1, ch);
        std::string::const_iterator it = buf.begin();
        try {
            univalue = CodingTable<ONEBYTE_LOCALE>::dec(it, buf.end());
            value = static_cast<int>(CodingTable<KOI8R>::enc(univalue));
        }
        catch (EncodingError e) {
            if (e==OutOfTable)
                Core::abort(CS(Symbol out of encoding));
            else
                Core::abort(CS(Error encoding symbol));
        }
        return value;
    }
    inline static char symbol(int code) {
        if (code<0 || code>255) {
            Core::abort(CS(Code out of range));
            return L'\0';
        }
        else {
            char value = '\0';
            uint32_t univalue = 0;
            const std::string buf(1, static_cast<char>(code));
            std::string::const_iterator it = buf.begin();
            try {
                univalue = CodingTable<KOI8R>::dec(it, buf.end());
                value = static_cast<char>(CodingTable<ONEBYTE_LOCALE>::enc(univalue));
            }
            catch (EncodingError e) {
                if (e==OutOfTable)
                    Core::abort(CS(Symbol out of encoding));
                else
                    Core::abort(CS(Error encoding symbol));
            }
            return value;
        }
    }
    inline static int unicode(char ch) {
        int value = 0;
        const std::string buf(1, ch);
        std::string::const_iterator it = buf.begin();
        try {
            value = CodingTable<ONEBYTE_LOCALE>::dec(it, buf.end());
        }
        catch (EncodingError e) {
            if (e==OutOfTable)
                Core::abort(CS(Symbol out of encoding));
            else
                Core::abort(CS(Error encoding symbol));
        }
        return value;
    }
    inline static char unisymbol(int code) {
        if (code<0 || code>255) {
            Core::abort(CS(Code out of range));
            return '\0';
        }
        else {
            char value = '\0';
            try {
                value = static_cast<char>(CodingTable<ONEBYTE_LOCALE>::enc(static_cast<uint32_t>(code)));
            }
            catch (EncodingError e) {
                if (e==OutOfTable)
                    Core::abort(CS(Symbol out of encoding));
                else
                    Core::abort(CS(Error encoding symbol));
            }
            return value;
        }
    }
#else
    inline static int code(wchar_t ch) {
        unsigned char value = 0;
        try {
            value = CodingTable<KOI8R>::enc(ch);
        }
        catch (EncodingError e) {
            if (e==OutOfTable)
                Core::abort(CS(Symbol out of encoding));
            else
                Core::abort(CS(Error encoding symbol));
        }
        return static_cast<int>(value);
    }
    inline static wchar_t symbol(int code) {
        if (code<0 || code>255) {
            Core::abort(CS(Code out of range));
            return L'\0';
        }
        else {
            const std::string buf(1, static_cast<char>(code));
            std::string::const_iterator it = buf.begin();
            uint32_t val = CodingTable<KOI8R>::dec(it, buf.end());
            return static_cast<wchar_t>(val);
        }
    }
    inline static int unicode(wchar_t ch) {
        return static_cast<int>(ch);
    }
    inline static wchar_t unisymbol(int code) {
        if (code<0 || code>65535) {
            Core::abort(CS(Code out of range));
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
public:
    inline static void init() {
        fileEncoding = UTF8;
    }
    inline static void finalize() {}

    inline static void setFileEncoding(const String & encoding) {
        if (encoding==CS(cp1251) || encoding==CS(windows1251) || encoding==CS(cp-1251) || encoding==CS(windows-1251)) {
            fileEncoding = CP1251;
        }
        else if (encoding==CS(cp866) || encoding==CS(ibm866) || encoding==CS(cp-866) || encoding==CS(ibm-866)) {
            fileEncoding = CP866;
        }
        else if (encoding==CS(koi8r) || encoding==CS(koi8-r)) {
            fileEncoding = KOI8R;
        }
        else if (encoding==CS(utf8) || encoding==CS(utf-8)) {
            fileEncoding = UTF8;
        }
        else {
            Core::abort(CS(Unknown character encoding));
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

    inline static int open(const String & fileName, const char * mode) {
        for (size_t i=0; i<openedFileNames.size(); i++) {
            if (openedFileNames[i]==fileName) {
                Core::abort(String(CS(File already opened: ))+fileName);
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

        FILE * res = fopen(path, mode);
        if (res==0) {
            Core::abort(String(CS(Can not open file :))+fileName);
        }
        else {
            openedFileNames.push_back(fileName);
            openedFileHandles.push_back(res);
            writableFiles[res] = strchr(mode, 'r')==0;
        }
#   ifndef NO_UNICODE
        free(path);
#   endif
        return openedFileHandles.size()-1;
    }
#endif
    inline static void close(int key) {
        if (key<0) {
            Core::abort(CS(Wrong key));
            return;
        }
        if (openedFileHandles.size()<=key || openedFileHandles[key]==0) {
            Core::abort(CS(File not opened with this key));
        }
        fclose(openedFileHandles[key]);
        openedFileHandles[key] = 0;
    }

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
private:
    inline static Char SL() {
#if defined(WIN32) || defined(_WIN32)
        return CC(\\);
#else
        return CC(/);
#endif
    }
    static std::deque<FILE*> openedFileHandles;
    static std::deque<String> openedFileNames;
    static std::map<FILE*,bool> writableFiles;
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

std::deque<FILE*> Files::openedFileHandles;
std::deque<String> Files::openedFileNames;
std::map<FILE*,bool> Files::writableFiles;
Encoding Files::fileEncoding;
#endif


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

}

#endif // KUMIRSTDLIB_H
