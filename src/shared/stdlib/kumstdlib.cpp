#include <QtCore>
#include <wchar.h>
#include <stdio.h>
#include "kumstdlib.h"
#include <stdlib.h>

static wchar_t __error__st_funct[256];

extern QString __get_error__st_funct()
{
    return QString::fromWCharArray(__error__st_funct);
}

#ifdef Q_OS_WIN32
    #define REAL_COMPARE_PRECISION 0.0000001
#else
    #define REAL_COMPARE_PRECISION 0
#endif

static QTextCodec * __koi8Codec__st_funct = 0;

extern "C" void __init__st_funct()
{
    __error__st_funct[0] = L'\0';
    if (__koi8Codec__st_funct==0) {
        __koi8Codec__st_funct = QTextCodec::codecForName("KOI8-R");
    }
}

void __abort__st_funct(const QString &error)
{
    error.toWCharArray(__error__st_funct);
    __error__st_funct[error.length()] = L'\0';
#ifdef ABORT_ON_ERROR
    qFatal(error.toLocal8Bit().data());
#endif
}


extern "C" double __safe_div_rr__(double a, double b)
{
    if (b==0.0) {
        __abort__st_funct(QObject::tr("Division by zero", "StFuncError"));
        return 0.0;
    }
    else {
        return a / b;
    }
}

extern "C" double __safe_div_ri__(double a, int b)
{
    if (b==0) {
        __abort__st_funct(QObject::tr("Division by zero", "StFuncError"));
        return 0.0;
    }
    else {
        return a / b;
    }
}

extern "C" double __safe_div_ir__(int a, double b)
{
    if (b==0.0) {
        __abort__st_funct(QObject::tr("Division by zero", "StFuncError"));
        return 0.0;
    }
    else {
        return a / b;
    }
}

extern "C" double __safe_div_ii__(int a, int b)
{
    if (b==0) {
        __abort__st_funct(QObject::tr("Division by zero", "StFuncError"));
        return 0.0;
    }
    else {
        return ( (double) a ) / ( (double) b );
    }
}

extern "C" double __real_power__(double a, double b)
{
    if (b<0.0) {
        __abort__st_funct(QObject::tr("Can't power to less than zero", "StFuncError"));
        return 0.0;
    }
    else {
        return pow(a, b);
    }
}

extern "C" int __integer_power__(int a, int b)
{
    if (b<0) {
        __abort__st_funct(QObject::tr("Can't power to less than zero", "StFuncError"));
        return 0;
    }
    else {
        double val = pow(a,b);
        val = floor(val);
        return int(val);
    }
}


extern "C" double abs__st_funct(double x)
{
    return ::fabs(x);
}

extern "C" double arccos__st_funct(double x)
{
    if (::fabs(x)>1) {
        __abort__st_funct(QObject::tr("Bad inverse trigonometric argument", "StFuncError"));
        return 0;
    }
    else {
        return ::acos(x);
    }
}

extern "C" double arcctg__st_funct(double x)
{
    if (::fabs(x)==0.0) {
        __abort__st_funct(QObject::tr("Bad inverse trigonometric argument", "StFuncError"));
        return 0;
    }
    else {
        return ::atan(1.0/x);
    }
}

extern "C" double arcsin__st_funct(double x)
{
    if (::fabs(x)>1) {
        __abort__st_funct(QObject::tr("Bad inverse trigonometric argument", "StFuncError"));
        return 0;
    }
    else {
        return ::asin(x);
    }
}

extern "C" double arctg__st_funct(double x)
{
    return ::atan(x);
}

extern "C" double cos__st_funct(double x)
{
    return ::cos(x);
}

extern "C" double ctg__st_funct(double x)
{
    if (::fabs(x)==0.0) {
        __abort__st_funct(QObject::tr("Bad inverse trigonometric argument", "StFuncError"));
        return 0;
    }
    else {
        return 1.0/::tan(x);
    }
}

extern "C" void delay__st_funct(int)
{
    // TODO implement is need
}

extern "C" int div__st_funct(int x, int y)
{
    if (y<=0) {
        __abort__st_funct(QObject::tr("Division by not natural number", "StFuncError"));
        return 0;
    }
    else {
        int xx = x;
        while (xx<0)
            xx += y;
        int mod = xx % y;
        return (x-mod)/y;
    }
}

extern "C" double exp__st_funct(double x)
{
    if (isnan(::exp(x))) {
        __abort__st_funct(QObject::tr("Function result is not a number", "StFuncError"));
        return 0;
    }
    else if (isinf(::exp(x))) {
        __abort__st_funct(QObject::tr("Function result is infinity value", "StFuncError"));
        return 0;
    }
    else {
        return ::exp(x);
    }
}

extern "C" int iabs__st_funct(int x)
{
    return qAbs(x);
}

extern "C" int imax__st_funct(int x, int y)
{
    return qMax(x, y);
}

extern "C" int imin__st_funct(int x, int y)
{
    return qMin(x, y);
}

extern "C" int int__st_funct(double x)
{
    // TODO check for integer overflow
    return int(floor(x));
}

extern "C" int irand__st_funct(int a, int b)
{
    if (a>b) {
        __abort__st_funct(QObject::tr("Invalid range", "StFuncError"));
        return 0;
    }
    else {
        int rndValue = qrand(); // in range [0..2^32]
        qreal scale = qreal(b-a+1)/qreal(RAND_MAX);
        return qMin(b, a+int(scale*rndValue));
    }
}

extern "C" int irnd__st_funct(int x)
{
    int rndValue = qrand();
    qreal scale = qreal(x)/qreal(RAND_MAX);
    return qMin(x, 1+int(scale*rndValue));
}

extern "C" double lg__st_funct(double x)
{
    if (x<=0.0) {
        __abort__st_funct(QObject::tr("Lg function argument must be > 0", "StFuncError"));
        return 0;
    }
    else {
        return ::log(x)/::log(10.0);
    }
}

extern "C" double ln__st_funct(double x)
{
    if (x<=0.0) {
        __abort__st_funct(QObject::tr("Ln function argument must be > 0", "StFuncError"));
        return 0;
    }
    else {
        return ::log(x);
    }
}

extern "C" double max__st_funct(double x, double y)
{
    return qMax(x, y);
}

extern "C" double min__st_funct(double x, double y)
{
    return qMin(x, y);
}

extern "C" int mod__st_funct(int x, int y)
{
    if (y<=0) {
        __abort__st_funct(QObject::tr("Division by not natural number", "StFuncError"));
        return 0;
    }
    else {
        while ( x < 0 )
            x += y;
        return x % y;
    }
}

extern "C" double rand__st_funct(double a, double b)
{
    if (a>b) {
        __abort__st_funct(QObject::tr("Invalid range", "StFuncError"));
        return 0;
    }
    else {
        int rndValue = qrand(); // in range [0..2^32]
        qreal scale = qreal(b-a)/qreal(RAND_MAX);
        return a+int(scale*rndValue);
    }
}

extern "C" double rnd__st_funct(double x)
{
    int rndValue = qrand();
    qreal stdValue = qreal(rndValue)/qreal(RAND_MAX);
    return x * stdValue;
}

extern "C" int sign__st_funct(double x)
{
    if (::fabs(x)<REAL_COMPARE_PRECISION) {
        return 0;
    }
    else if (x<0.0) {
        return -1;
    }
    else {
        return 1;
    }
}

extern "C" double sin__st_funct(double x)
{
    return ::sin(x);
}

extern "C" double sqrt__st_funct(double x)
{
    if (x<0) {
        __abort__st_funct(QObject::tr("Square root of negative value", "StFuncError"));
        return 0;
    }
    else {
        return ::sqrt(x);
    }
}

extern "C" double tg__st_funct(double x)
{
    return ::tan(x);
}

extern "C" double MAXREAL__st_funct()
{
    return 1.797693e+308;
}

extern "C" int MAXINT__st_funct()
{
    return int(89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068608.);
}

extern "C" wchar_t* real_to_string__st_funct(double x)
{
    QString result = QString("%1").arg(x, 0,'f');
    QStringList pair = result.split(".",QString::KeepEmptyParts);
    if ( pair.count() == 2 ) {
            while ( pair[1].endsWith("0") )
                    pair[1].remove(pair[1].length()-1,1);
            result = pair.join(".");
    }
    if ( result.endsWith(".") )
            result.remove(result.length()-1,1);

    wchar_t * buffer = (wchar_t*)calloc(result.length()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.length()] = L'\0';
    return buffer;
}

extern "C" int time__st_funct()
{
    QTime t = QTime::currentTime();
    int sec = t.hour()*3600 + t.minute()*60 + t.second();
    int csec = sec * 100;
    csec += t.msec() / 10;
    return csec;
}

extern "C" int length__st_funct(wchar_t * s)
{
    QString ss = QString::fromWCharArray(s);
    return ss.length();
}
extern "C" int code__st_funct(wchar_t ch)
{
    const wchar_t buffer[1] = { ch };
    const QString str = QString::fromWCharArray(buffer, 1);
    if (ch!='\0' && !__koi8Codec__st_funct->canEncode(str)) {
        __abort__st_funct(QObject::tr("Symbol out of the KOI8-R range", "StFuncError"));
        return 0;
    }
    else {
        QByteArray koi8Data = __koi8Codec__st_funct->fromUnicode(str);
        return int(koi8Data.at(0));
    }
}

extern "C" double string_to_real__st_funct(wchar_t * s, unsigned char * success)
{
    QString str = QString::fromWCharArray(s);
    bool ok = false;
    double res = str.toDouble(&ok);
    *success = ok? 1 : 0;
    return res;
}

extern "C" int string_to_int__st_funct(wchar_t* s, unsigned char *success)
{
    QString str = QString::fromWCharArray(s);
    bool ok = false;
    int res = str.toInt(&ok);
    *success = ok? 1 : 0;
    return res;
}

extern "C" wchar_t symbol__st_funct(int n)
{
    if (n<0 || n>255) {
        __abort__st_funct(QObject::tr("Code out of range [0..255]", "StFuncError"));
        return 0;
    }
    else {
        QChar simbol;
        switch(n) //Таблица кодировки
        {


        case 32:simbol=' ';break;
        case 33:simbol='!';break;
        case 34:simbol='"';break;
        case 35:simbol='#';break;
        case 36:simbol='$';break;
        case 37:simbol='%';break;
        case 38:simbol='&';break;

        case 39:simbol=QChar(0x0027 );break;
        case 40:simbol=QChar(0x0028 );break;
        case 41:simbol=QChar(0x0029 );break;
        case 42:simbol=QChar(0x002A );break;
        case 43:simbol=QChar(0x002B );break;
        case 44:simbol=QChar(0x002C );break;
        case 45:simbol=QChar(0x002D );break;
        case 46:simbol=QChar(0x002E );break;
        case 47:simbol=QChar(0x002F );break;
        case 48:simbol=QChar(0x0030 );break;
        case 49:simbol=QChar(0x0031 );break;
        case 50:simbol=QChar(0x0032 );break;
        case 51:simbol=QChar(0x0033 );break;
        case 52:simbol=QChar(0x0034 );break;
        case 53:simbol=QChar(0x0035 );break;
        case 54:simbol=QChar(0x0036 );break;
        case 55:simbol=QChar(0x0037 );break;
        case 56:simbol=QChar(0x0038 );break;
        case 57:simbol=QChar(0x0039 );break;
        case 58:simbol=QChar(0x003A );break;
        case 59:simbol=QChar(0x003B );break;
        case 60:simbol=QChar(0x003C );break;
        case 61:simbol=QChar(0x003D );break;
        case 62:simbol=QChar(0x003E );break;
        case 63:simbol=QChar(0x003F );break;
        case 64:simbol=QChar(0x0040 );break;
        case 65:simbol=QChar(0x0041 );break;
        case 66:simbol=QChar(0x0042 );break;
        case 67:simbol=QChar(0x0043 );break;
        case 68:simbol=QChar(0x0044 );break;
        case 69:simbol=QChar(0x0045 );break;
        case 70:simbol=QChar(0x0046 );break;
        case 71:simbol=QChar(0x0047 );break;
        case 72:simbol=QChar(0x0048 );break;
        case 73:simbol=QChar(0x0049 );break;
        case 74:simbol=QChar(0x004A );break;
        case 75:simbol=QChar(0x004B );break;
        case 76:simbol=QChar(0x004C );break;
        case 77:simbol=QChar(0x004D );break;
        case 78:simbol=QChar(0x004E );break;
        case 79:simbol=QChar(0x004F );break;
        case 80:simbol=QChar(0x0050 );break;
        case 81:simbol=QChar(0x0051 );break;
        case 82:simbol=QChar(0x0052 );break;
        case 83:simbol=QChar(0x0053 );break;
        case 84:simbol=QChar(0x0054 );break;
        case 85:simbol=QChar(0x0055 );break;
        case 86:simbol=QChar(0x0056 );break;
        case 87:simbol=QChar(0x0057 );break;
        case 88:simbol=QChar(0x0058 );break;
        case 89:simbol=QChar(0x0059 );break;
        case 90:simbol=QChar(0x005A );break;
        case 91:simbol=QChar(0x005B );break;
        case 92:simbol=QChar(0x005C );break;
        case 93:simbol=QChar(0x005D );break;
        case 94:simbol=QChar(0x005E );break;
        case 95:simbol=QChar(0x005F );break;
        case 96:simbol=QChar(0x0060 );break;
        case 97:simbol=QChar(0x0061 );break;
        case 98:simbol=QChar(0x0062 );break;
        case 99:simbol=QChar(0x0063 );break;
        case 100:simbol=QChar(0x0064 );break;
        case 101:simbol=QChar(0x0065 );break;
        case 102:simbol=QChar(0x0066 );break;
        case 103:simbol=QChar(0x0067 );break;
        case 104:simbol=QChar(0x0068 );break;
        case 105:simbol=QChar(0x0069 );break;
        case 106:simbol=QChar(0x006A );break;
        case 107:simbol=QChar(0x006B );break;
        case 108:simbol=QChar(0x006C );break;
        case 109:simbol=QChar(0x006D );break;
        case 110:simbol=QChar(0x006E );break;
        case 111:simbol=QChar(0x006F );break;
        case 112:simbol=QChar(0x0070 );break;
        case 113:simbol=QChar(0x0071 );break;
        case 114:simbol=QChar(0x0072 );break;
        case 115:simbol=QChar(0x0073 );break;
        case 116:simbol=QChar(0x0074 );break;
        case 117:simbol=QChar(0x0075 );break;
        case 118:simbol=QChar(0x0076 );break;
        case 119:simbol=QChar(0x0077 );break;
        case 120:simbol=QChar(0x0078 );break;
        case 121:simbol=QChar(0x0079 );break;
        case 122:simbol=QChar(0x007A );break;
        case 123:simbol=QChar(0x007B );break;
        case 124:simbol=QChar(0x007C );break;
        case 125:simbol=QChar(0x007D );break;
        case 126:simbol=QChar(0x007E );break;
        case 127:simbol=QChar(0x007F );break;
        case 128:simbol=QChar(0x2500 );break;
        case 129:simbol=QChar(0x2502 );break;
        case 130:simbol=QChar(0x250C );break;
        case 131:simbol=QChar(0x2510 );break;
        case 132:simbol=QChar(0x2514 );break;
        case 133:simbol=QChar(0x2518 );break;
        case 134:simbol=QChar(0x251C );break;
        case 135:simbol=QChar(0x2524 );break;
        case 136:simbol=QChar(0x252C );break;
        case 137:simbol=QChar(0x2534 );break;
        case 138:simbol=QChar(0x253C );break;
        case 139:simbol=QChar(0x2580 );break;
        case 140:simbol=QChar(0x2584 );break;
        case 141:simbol=QChar(0x2588 );break;
        case 142:simbol=QChar(0x258C );break;
        case 143:simbol=QChar(0x2590 );break;
        case 144:simbol=QChar(0x2591 );break;
        case 145:simbol=QChar(0x2592 );break;
        case 146:simbol=QChar(0x2593 );break;
        case 147:simbol=QChar(0x2320 );break;
        case 148:simbol=QChar(0x25A0 );break;
        case 149:simbol=QChar(0x2219 );break;
        case 150:simbol=QChar(0x221A );break;
        case 151:simbol=QChar(0x2248 );break;
        case 152:simbol=QChar(0x2264 );break;
        case 153:simbol=QChar(0x2265 );break;
        case 154:simbol=QChar(0x00A0 );break;
        case 155:simbol=QChar(0x2321 );break;
        case 156:simbol=QChar(0x00B0 );break;
        case 157:simbol=QChar(0x00B2 );break;
        case 158:simbol=QChar(0x00B7 );break;
        case 159:simbol=QChar(0x00F7 );break;
        case 160:simbol=QChar(0x2550 );break;
        case 161:simbol=QChar(0x2551 );break;
        case 162:simbol=QChar(0x2552 );break;
        case 163:simbol=QChar(0x0451 );break;
        case 164:simbol=QChar(0x2553 );break;
        case 165:simbol=QChar(0x2554 );break;
        case 166:simbol=QChar(0x2555 );break;
        case 167:simbol=QChar(0x2556 );break;
        case 168:simbol=QChar(0x2557 );break;
        case 169:simbol=QChar(0x2558 );break;
        case 170:simbol=QChar(0x2559 );break;
        case 171:simbol=QChar(0x255A );break;
        case 172:simbol=QChar(0x255B );break;
        case 173:simbol=QChar(0x255C );break;
        case 174:simbol=QChar(0x255D );break;
        case 175:simbol=QChar(0x255E );break;
        case 176:simbol=QChar(0x255F );break;
        case 177:simbol=QChar(0x2560 );break;
        case 178:simbol=QChar(0x2561 );break;
        case 179:simbol=QChar(0x0401 );break;
        case 180:simbol=QChar(0x2562 );break;
        case 181:simbol=QChar(0x2563 );break;
        case 182:simbol=QChar(0x2564 );break;
        case 183:simbol=QChar(0x2565 );break;
        case 184:simbol=QChar(0x2566 );break;
        case 185:simbol=QChar(0x2567 );break;
        case 186:simbol=QChar(0x2568 );break;
        case 187:simbol=QChar(0x2569 );break;
        case 188:simbol=QChar(0x256A );break;
        case 189:simbol=QChar(0x256B );break;
        case 190:simbol=QChar(0x256C );break;
        case 191:simbol=QChar(0x00A9 );break;
        case 192:simbol=QChar(0x044E );break;
        case 193:simbol=QChar(0x0430 );break;
        case 194:simbol=QChar(0x0431 );break;
        case 195:simbol=QChar(0x0446 );break;
        case 196:simbol=QChar(0x0434 );break;
        case 197:simbol=QChar(0x0435 );break;
        case 198:simbol=QChar(0x0444 );break;
        case 199:simbol=QChar(0x0433 );break;
        case 200:simbol=QChar(0x0445 );break;
        case 201:simbol=QChar(0x0438 );break;
        case 202:simbol=QChar(0x0439 );break;
        case 203:simbol=QChar(0x043A );break;
        case 204:simbol=QChar(0x043B );break;
        case 205:simbol=QChar(0x043C );break;
        case 206:simbol=QChar(0x043D );break;
        case 207:simbol=QChar(0x043E );break;
        case 208:simbol=QChar(0x043F );break;
        case 209:simbol=QChar(0x044F );break;
        case 210:simbol=QChar(0x0440 );break;
        case 211:simbol=QChar(0x0441 );break;
        case 212:simbol=QChar(0x0442 );break;
        case 213:simbol=QChar(0x0443 );break;
        case 214:simbol=QChar(0x0436 );break;
        case 215:simbol=QChar(0x0432 );break;
        case 216:simbol=QChar(0x044C );break;
        case 217:simbol=QChar(0x044B );break;
        case 218:simbol=QChar(0x0437 );break;
        case 219:simbol=QChar(0x0448 );break;
        case 220:simbol=QChar(0x044D );break;
        case 221:simbol=QChar(0x0449 );break;
        case 222:simbol=QChar(0x0447 );break;
        case 223:simbol=QChar(0x044A );break;
        case 224:simbol=QChar(0x042E );break;
        case 225:simbol=QChar(0x0410 );break;
        case 226:simbol=QChar(0x0411 );break;
        case 227:simbol=QChar(0x0426 );break;
        case 228:simbol=QChar(0x0414 );break;
        case 229:simbol=QChar(0x0415 );break;
        case 230:simbol=QChar(0x0424 );break;
        case 231:simbol=QChar(0x0413 );break;
        case 232:simbol=QChar(0x0425 );break;
        case 233:simbol=QChar(0x0418 );break;
        case 234:simbol=QChar(0x0419 );break;
        case 235:simbol=QChar(0x041A );break;
        case 236:simbol=QChar(0x041B );break;
        case 237:simbol=QChar(0x041C );break;
        case 238:simbol=QChar(0x041D );break;
        case 239:simbol=QChar(0x041E );break;
        case 240:simbol=QChar(0x041F );break;
        case 241:simbol=QChar(0x042F );break;
        case 242:simbol=QChar(0x0420 );break;
        case 243:simbol=QChar(0x0421 );break;
        case 244:simbol=QChar(0x0422 );break;
        case 245:simbol=QChar(0x0423 );break;
        case 246:simbol=QChar(0x0416 );break;
        case 247:simbol=QChar(0x0412 );break;
        case 248:simbol=QChar(0x042C );break;
        case 249:simbol=QChar(0x042B );break;
        case 250:simbol=QChar(0x0417 );break;
        case 251:simbol=QChar(0x0428 );break;
        case 252:simbol=QChar(0x042D );break;
        case 253:simbol=QChar(0x0429 );break;
        case 254:simbol=QChar(0x0427 );break;
        case 255:simbol=QChar(0x042A );break;
        default: simbol=' ';

        };
        if (n<32)
            simbol = QChar(n);
        wchar_t buffer[1];
        QString(simbol).toWCharArray(buffer);
        return buffer[0];
    }
}

extern "C" wchar_t symbol2__st_funct(int n)
{
    QChar ch(n);
    static const QString cyrillic =
            QString::fromUtf8("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЮЭЯ");
    bool valid = n < 128 || cyrillic.contains(ch.toUpper());
    if (!valid) {
        __abort__st_funct(QObject::tr("Resulting character is not allowed in Russian languange", "StFuncError"));
        return L'\0';
    }
    else {
        wchar_t buffer[1];
        QString(ch).toWCharArray(buffer);
        return buffer[0];
    }

}

extern "C" wchar_t * int_to_string__st_funct(int x)
{
    QString result = QString::number(x);
    wchar_t * buffer = (wchar_t*)calloc(result.length()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    return buffer;
}

extern "C" int unicode__st_funct(wchar_t c)
{
    const wchar_t buffer[1] = { c };
    return QString::fromWCharArray(buffer, 1)[0].unicode();
}


struct GC_element
{
    QList<wchar_t*> strings;
    QList<struct __array__*> arrays;
};

static QStack<GC_element> GarbageCollector;

extern "C" void __init_garbage_collector__()
{
    GarbageCollector.clear();
    __garbage_collector_begin_algorhitm__();
}

extern "C" void __free_garbage_collector__()
{
    while (!GarbageCollector.isEmpty()) {
        __garbage_collector_end_algorhitm__();
    }
}

extern "C" void __garbage_collector_begin_algorhitm__()
{
    GC_element newContext;
    GarbageCollector.push(newContext);
}

extern "C" void __garbage_collector_register_string__(wchar_t * s)
{
    GarbageCollector.last().strings << s;
}

extern "C" void __garbage_collector_register_array__(struct __array__ * a)
{
    GarbageCollector.last().arrays << a;
}

extern "C" void __garbage_collector_set_return_value__(wchar_t * s)
{
    Q_ASSERT(GarbageCollector.size()>=2);
    if (GarbageCollector.last().strings.contains(s)) {
        GarbageCollector.last().strings.removeAll(s);
        GarbageCollector[GarbageCollector.size()-2].strings << s;
    }
}

extern "C" void __garbage_collector_set_return_array__(struct __array__ * a)
{
    Q_ASSERT(GarbageCollector.size()>=2);
    if (a->type=='s') {
        wchar_t ** sd = (wchar_t**)(a->data);
        int size = 1;
        for (int i=0; i<a->dim; i++) {
            size *= a->sizes[i];
        }
        for (int i=0; i<size; i++) {
            wchar_t * s = sd[i];
            if (GarbageCollector.last().strings.contains(s)) {
                GarbageCollector.last().strings.removeAll(s);
                GarbageCollector[GarbageCollector.size()-2].strings << s;
            }
        }
    }
}

extern "C" void __garbage_collector_end_algorhitm__()
{
    GC_element context = GarbageCollector.pop();
    for (int i=0; i<context.arrays.size(); i++) {
        __array__ * a = context.arrays[i];
        void * va = a->data;
        if (a->type=='i') {
            int * ia = (int*)(va);
            free(ia);
        }
        if (a->type=='r') {
            double * ra = (double*)(va);
            free(ra);
        }
        if (a->type=='b') {
            unsigned char * ba = (unsigned char*)(va);
            free(ba);
        }
        if (a->type=='c') {
            wchar_t * ca = (wchar_t*)(va);
            free(ca);
        }
        if (a->type=='s') {
            wchar_t ** sa = (wchar_t**)(va);
            free(sa);
        }
    }
    for (int i=0; i<context.strings.size(); i++) {
        wchar_t * s = context.strings[i];
        free(s);
    }
}

extern "C" void __halt__(int lineNo)
{
    if (lineNo!=-1) {
        fwprintf(stderr, L"STOP AT LINE %i.\n", lineNo);
    }
    else {
        fwprintf(stderr, L"STOP.\n");
    }
    __free_garbage_collector__();
    exit(0);
}

extern "C" void __abort__(const wchar_t * message, int lineNo)
{
    if (lineNo!=-1) {
        fwprintf(stderr, L"RUNTIME ERROR AT LINE %i: %ls\n", lineNo, message);
    }
    else {
        fwprintf(stderr, L"RUNTIME ERROR: %ls\n", message);
    }
    __free_garbage_collector__();
    exit(1);
}

extern "C" void __allocate_array__(struct __array__ * arr)
{
    int size = 1;
    for (int i=0; i<arr->dim; i++) {
      if (arr->rights[i]<arr->lefts[i]) {
          __abort__(L"Incorrect array bound", -1);
      }
      arr->sizes[i] = (arr->rights[i]-arr->lefts[i]+1);
      size *= arr->sizes[i];
    }
    size_t elemSize;
    switch (arr->type) {
      case 'i': elemSize = sizeof(int); break;
      case 'r': elemSize = sizeof(double); break;
      case 'b': elemSize = sizeof(unsigned char); break;
      case 'c': elemSize = sizeof(wchar_t); break;
      case 's': elemSize = sizeof(wchar_t*); break;
      default: elemSize = sizeof(void*);
    }
    //wprintf(L"Total size: %d\n", size);
    arr->data = calloc(size, elemSize);
    __garbage_collector_register_array__(arr);
}

extern "C" void* __element__(struct __array__ * arr, int i0, int i1, int i2)
{
    size_t elemSize;
    switch (arr->type) {
      case 'i': elemSize = sizeof(int); break;
      case 'r': elemSize = sizeof(double); break;
      case 'b': elemSize = sizeof(unsigned char); break;
      case 'c': elemSize = sizeof(wchar_t); break;
      case 's': elemSize = sizeof(wchar_t*); break;
      default: elemSize = sizeof(void*);
    }
    int index = 0;
    switch (arr->dim) {
      case 1:
        index = i0 - arr->lefts[0];
        break;
      case 2: index = ( i0 - arr->lefts[0] ) * arr->sizes[1]
        + i1 - arr->lefts[1];
        break;
      case 3:
        index = ( i0 - arr->lefts[0] ) * arr->sizes[1] * arr->sizes[2]
        + ( i1 - arr->lefts[1] ) * arr->sizes[2]
        + ( i2 - arr->lefts[2] );
        break;
      default:
        index = 0;
    }
    int * i = 0;
    double * r = 0;
    wchar_t * c = 0;
    unsigned char * b = 0;
    wchar_t* * s = 0;
    switch (arr->type) {
      case 'i':
        i = (int*)(arr->data);
        return &(i[index]);
      case 'r':
        r = (double*)(arr->data);
        return &(r[index]);
      case 'c':
        c = (wchar_t*)(arr->data);
        return &(c[index]);
      case 's':
        s = (wchar_t* *)(arr->data);
        return &(s[index]);
    case 'b':
        b = (unsigned char*)(arr->data);
        return &(b[index]);
      default:
        return 0;
    }
}

extern "C"  wchar_t* __string_from_constant__(const wchar_t * str)
{
    int len = wcslen(str);
    wchar_t* result = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcscpy(result, str);
    __garbage_collector_register_string__(result);
    return result;
}

extern "C"  wchar_t* __concatenate_ss__(const wchar_t * s1, const wchar_t * s2)
{
    int len = wcslen(s1) + wcslen(s2);
    wchar_t* result = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcscpy(result, s1);
    wcscat(result, s2);
    __garbage_collector_register_string__(result);
    return result;
}

extern "C"  wchar_t* __concatenate_sc__(const wchar_t * s, wchar_t ch)
{
    int len = wcslen(s) + 1;
    wchar_t* result = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcscpy(result, s);
    result[len-1] = ch;
    result[len] = L'\0';
    __garbage_collector_register_string__(result);
    return result;
}

extern "C"  wchar_t* __concatenate_cs__(wchar_t ch, const wchar_t * s)
{
    int len = wcslen(s);
    wchar_t* result = (wchar_t*)calloc(len+2, sizeof(wchar_t));
    result[0] = ch;
    for (int i=0; i<len; i++) {
        result[i+1] = s[i];
    }
    result[len+1] = L'\0';
    __garbage_collector_register_string__(result);
    return result;
}

extern "C"  wchar_t* __concatenate_cc__(wchar_t ch1, wchar_t ch2)
{
    wchar_t* result = (wchar_t*)calloc(3, sizeof(wchar_t));
    result[0] = ch1;
    result[1] = ch2;
    result[2] = L'\0';
    __garbage_collector_register_string__(result);
    return result;
}


extern "C"  wchar_t __get_char_at__(const wchar_t * s, int index)
{
    int sz = wcslen(s);
    if (index-1>=sz || index-1<0) {
        __abort__(L"String index out of its size", -1);
        return L'\0';
    }
    return s[index-1];
}

extern "C"  void __set_char_at__(wchar_t * s, int index, wchar_t ch)
{
    int sz = wcslen(s);
    if (index-1>=sz || index-1<0) {
        __abort__(L"String index out of its size", -1);
        return;
    }
    s[index-1] = ch;
}

extern "C"  wchar_t* __get_slice__(const wchar_t * s, int start, int end)
{
    if (end<start) {
        __abort__(L"Invalid string slice", -1);
        return 0;
    }
    int sz = wcslen(s);
    if (start-1>=sz || start-1<0 || end-1>=sz || end-1<0) {
        __abort__(L"String index out of its size", -1);
        return 0;
    }
    wchar_t * res = (wchar_t*)calloc(end-start+2, sizeof(wchar_t));
    int index = 1;
    for (int i=start; i<=end; i++) {
        res[index-1] = s[i-1];
        index++;
    }
    res[index] = L'\0';
    __garbage_collector_register_string__(res);
    return res;
}

extern "C"  void __set_slice_s__(wchar_t ** s, int start, int end, const wchar_t * ss)
{
    if (end<start) {
        __abort__(L"Invalid string slice", -1);
        return;
    }
    int sz = wcslen(*s);
    if (start-1>=sz || start-1<0 || end-1>=sz || end-1<0) {
        __abort__(L"String index out of its size", -1);
        return;
    }
    int ss_len = wcslen(ss);
    int s_len = wcslen(*s);
    int res_len = s_len - (end-start+1) + ss_len;
    wchar_t * res = (wchar_t*)calloc(res_len+1, sizeof(wchar_t));
    wcsncpy(res, *s, start-1);
    wcsncpy(res+(start-1), ss, ss_len);
    wcsncpy(res+ss_len+(start-1), *s+end, s_len-end);
    res[res_len] = L'\0';
    __garbage_collector_register_string__(res);
    (*s) = res;
}

extern "C"  void __set_slice_c__(wchar_t ** s, int start, int end, wchar_t ch)
{
    if (end<start) {
        __abort__(L"Invalid string slice", -1);
        return;
    }
    int sz = wcslen(*s);
    if (start-1>=sz || start-1<0 || end-1>=sz || end-1<0) {
        __abort__(L"String index out of its size", -1);
        return;
    }
    int ss_len = 1;
    int s_len = wcslen(*s);
    int res_len = s_len - (end-start+1) + ss_len;
    wchar_t * res = (wchar_t*)calloc(res_len+1, sizeof(wchar_t));
    wcsncpy(res, *s, start-1);
//    wcsncpy(res+(start-1), ss, ss_len);
    res[start-1] = ch;
    wcsncpy(res+ss_len+(start-1), *s+end, s_len-end);
    res[res_len] = L'\0';
    __garbage_collector_register_string__(res);
    (*s) = res;
}

extern "C" wchar_t* __string_of_char__(wchar_t ch)
{
    wchar_t * result = (wchar_t*)calloc(2, sizeof(wchar_t));
    result[0] = ch;
    result[1] = L'\0';
    __garbage_collector_register_string__(result);
    return result;
}
