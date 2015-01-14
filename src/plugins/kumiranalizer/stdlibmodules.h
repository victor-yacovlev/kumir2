#ifndef STDLIBMODULES_H
#define STDLIBMODULES_H
#include <QtCore>
#include "interfaces/actorinterface.h"
#include "stdlib/kumirstdlib.hpp"

namespace StdLibModules {

class RTL
        : public Shared::ActorInterface

{
public:
    inline QByteArray asciiModuleName() const { return QByteArray("Kumir Standard Library"); }
    inline QString localizedModuleName(const QLocale::Language) const {return QString::fromUtf8("Стандартные функции");}

    inline FunctionList functionList() const {
        FunctionList result;
        Function func;
        func.accessType = PublicFunction;

        /* алг вещ abs(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0000;
        func.asciiName = "abs";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ arccos(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0001;
        func.asciiName = "arccos";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ arcctg(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0002;
        func.asciiName = "arcctg";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ arcsin(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0003;
        func.asciiName = "arcsin";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ arctg(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0004;
        func.asciiName = "arctg";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ cos(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0005;
        func.asciiName = "cos";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ ctg(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0006;
        func.asciiName = "ctg";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг ждать(цел x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0007;
        func.asciiName = "wait";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("ждать");
        func.returnType = Void;
        func.arguments << Argument("ms", Int);
        result << func;

        /* алг цел div(цел x, цел y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0008;
        func.asciiName = "div";
        func.returnType = Int;
        func.arguments << Argument("x", Int) << Argument("y", Int);
        result << func;

        /* алг вещ exp(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0009;
        func.asciiName = "exp";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг цел iabs(цел x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x000a;
        func.asciiName = "iabs";
        func.returnType = Int;
        func.arguments << Argument("x", Int);
        result << func;

        /* алг цел imax(цел x, цел y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x000b;
        func.asciiName = "imax";
        func.returnType = Int;
        func.arguments << Argument("x", Int) << Argument("y", Int);
        result << func;

        /* алг цел imin(цел x, цел y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x000c;
        func.asciiName = "imin";
        func.returnType = Int;
        func.arguments << Argument("x", Int) << Argument("y", Int);
        result << func;

        /* алг цел int(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x000d;
        func.asciiName = "int";
        func.returnType = Int;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг цел irand(цел x, цел y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x000e;
        func.asciiName = "irand";
        func.returnType = Int;
        func.arguments << Argument("x", Int) << Argument("y", Int);
        result << func;

        /* алг цел irnd(цел x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x000f;
        func.asciiName = "irnd";
        func.returnType = Int;
        func.arguments << Argument("x", Int);
        result << func;

        /* алг вещ lg(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0010;
        func.asciiName = "lg";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ ln(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0011;
        func.asciiName = "ln";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ max(вещ x, вещ y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0012;
        func.asciiName = "max";
        func.returnType = Real;
        func.arguments << Argument("x", Real) << Argument("y", Real);
        result << func;

        /* алг вещ min(вещ x, вещ y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0013;
        func.asciiName = "min";
        func.returnType = Real;
        func.arguments << Argument("x", Real) << Argument("y", Real);
        result << func;

        /* алг цел mod(цел x, цел y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0014;
        func.asciiName = "mod";
        func.returnType = Int;
        func.arguments << Argument("x", Int) << Argument("y", Int);
        result << func;

        /* алг вещ rand(вещ x, вещ y) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0015;
        func.asciiName = "rand";
        func.returnType = Real;
        func.arguments << Argument("x", Real) << Argument("y", Real);
        result << func;

        /* алг вещ rnd(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0016;
        func.asciiName = "rnd";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг цел sign(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0017;
        func.asciiName = "sign";
        func.returnType = Int;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ sin(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0018;
        func.asciiName = "sin";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ sqrt(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0019;
        func.asciiName = "sqrt";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ tg(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x001a;
        func.asciiName = "tg";
        func.returnType = Real;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг вещ МАКСВЕЩ */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x001b;
        func.asciiName = "MAXREAL";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("МАКСВЕЩ");
        func.returnType = Real;
        result << func;

        /* алг вещ МАКСЦЕЛ */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x001c;
        func.asciiName = "MAXINT";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("МАКСЦЕЛ");
        func.returnType = Int;
        result << func;

        /* алг лит вещ_в_лит(вещ x) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x001d;
        func.asciiName = "string_of_real";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("вещ_в_лит");
        func.returnType = String;
        func.arguments << Argument("x", Real);
        result << func;

        /* алг цел время */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x001e;
        func.asciiName = "time";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("время");
        func.returnType = Int;
        result << func;

        /* алг цел длин(лит s) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x001f;
        func.asciiName = "length";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("длин");
        func.returnType = Int;
        func.arguments << Argument("s", String);
        result << func;

        /* алг цел код(сим ch) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0020;
        func.asciiName = "code";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("код");
        func.returnType = Int;
        func.arguments << Argument("ch", Char);
        result << func;

        /* алг вещ лит_в_вещ(лит s, рез лог success) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0021;
        func.asciiName = "string_to_real";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("лит_в_вещ");
        func.returnType = Real;
        func.arguments << Argument("s", String) << Argument("success", Bool);
        func.arguments.last().accessType = OutArgument;
        result << func;

        /* алг цел лит_в_цел(лит s, рез лог success) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0022;
        func.asciiName = "string_to_int";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("лит_в_цел");
        func.returnType = Int;
        func.arguments << Argument("s", String) << Argument("success", Bool);
        func.arguments.last().accessType = OutArgument;
        result << func;

        /* алг сим символ(цел n) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0023;
        func.asciiName = "symbol";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("символ");
        func.returnType = Char;
        func.arguments << Argument("n", Int);
        result << func;

        /* алг сим символ2(цел n) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0024;
        func.asciiName = "unisymbol";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("символ2");
        func.returnType = Char;
        func.arguments << Argument("n", Int);
        result << func;

        /* алг лит цел_в_лит(цел n) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0025;
        func.asciiName = "string_of_int";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("цел_в_лит");
        func.returnType = String;
        func.arguments << Argument("n", Int);
        result << func;

        /* алг цел юникод(сим ch) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0026;
        func.asciiName = "unicode";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("юникод");
        func.returnType = Int;
        func.arguments << Argument("ch", Char);
        result << func;

        /* алг цел Цел(лит строка, цел по умолчанию) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0027;
        func.asciiName = "Integer";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("Цел");
        func.returnType = Int;
        func.arguments << Argument("lexem", String);
        func.arguments << Argument("defvalue", Int);
        result << func;

        /* алг вещ Вещ(лит строка, вещ по умолчанию) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0028;
        func.asciiName = "Real";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("Вещ");
        func.returnType = Int;
        func.arguments << Argument("lexem", String);
        func.arguments << Argument("defvalue", Real);
        result << func;

        /* алг лог Лог(лит строка, лог по умолчанию) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0029;
        func.asciiName = "Boolean";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("Лог");
        func.returnType = Int;
        func.arguments << Argument("lexem", String);
        func.arguments << Argument("defvalue", Bool);
        result << func;

        /* алг @поставить оценку(цел значение) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x002A;
        func.asciiName = "@Evaluate";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("@поставить оценку");
        func.returnType = Void;
        func.arguments << Argument("value", Int);
        result << func;


        return result;
    }

};

class Files
        : public Shared::ActorInterface
{
    inline QByteArray asciiModuleName() const { return QByteArray("Files"); }
    inline QString localizedModuleName(const QLocale::Language) const {return QString::fromUtf8("Файлы");}
    inline TypeList typeList() const {
        TypeList result;
        Field fileKey(QByteArray("key"), Int);
        Field openMode(QByteArray("mode"), Int);
        Field specialType(QByteArray("type"), Int);
        Field fileName(QByteArray("name"), String);
        Record fileRecord;
        fileRecord << fileKey << openMode << specialType << fileName;
        RecordSpecification fileType;
        fileType.asciiName = "file";
        fileType.localizedNames[QLocale::Russian] = QString::fromUtf8("файл");
        fileType.record = fileRecord;
        result << fileType;
        return result;
    }

    inline FunctionList functionList() const {
        FunctionList result;
        Function func;
        func.accessType = PublicFunction;
        const RecordSpecification FileType = typeList().first();

        /* алг файл открыть на чтение(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0000;
        func.asciiName = "read open";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("открыть на чтение");
        func.returnType = RecordType;
        func.returnTypeSpecification = FileType;
        func.arguments << Argument("name", String);
        result << func;

        /* алг файл открыть на запись(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0001;
        func.asciiName = "write open";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("открыть на запись");
        func.returnType = RecordType;
        func.returnTypeSpecification = FileType;
        func.arguments << Argument("name", String);
        result << func;

        /* алг файл открыть на добавление(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0002;
        func.asciiName = "append open";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("открыть на добавление");
        func.returnType = RecordType;
        func.returnTypeSpecification = FileType;
        func.arguments << Argument("name", String);
        result << func;

        /* алг закрыть(файл ключ) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0003;
        func.asciiName = "close";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("закрыть");
        func.returnType = Void;
        func.arguments << Argument("key", RecordType);
        func.arguments.last().typeSpecification = FileType;
        result << func;

        /* алг начать чтение(файл ключ) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0004;
        func.asciiName = "start reading";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("начать чтение");
        func.returnType = Void;
        func.arguments << Argument("key", RecordType);
        func.arguments.last().typeSpecification = FileType;
        result << func;

        /* алг лог конец файла(файл ключ) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0005;
        func.asciiName = "eof";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("конец файла");
        func.returnType = Bool;
        func.arguments << Argument("key", RecordType);
        func.arguments.last().typeSpecification = FileType;
        result << func;

        /* алг установить кодировку(лит кодировка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0006;
        func.asciiName = "set encoding";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("установить кодировку");
        func.returnType = Void;
        func.arguments << Argument("name", String);
        result << func;

        /* алг лог можно открыть на чтение(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0007;
        func.asciiName = "readable";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("можно открыть на чтение");
        func.returnType = Bool;
        func.arguments << Argument("name", String);
        result << func;

        /* алг лог можно открыть на запись(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0008;
        func.asciiName = "writable";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("можно открыть на запись");
        func.returnType = Bool;
        func.arguments << Argument("name", String);
        result << func;

        /* алг лог есть данные(файл ключ) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0009;
        func.asciiName = "nonempty";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("есть данные");
        func.returnType = Bool;
        func.arguments << Argument("key", RecordType);
        func.arguments.last().typeSpecification = FileType;
        result << func;

        /* алг лог существует(лит имя файла Или каталога) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x000a;
        func.asciiName = "exists";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("существует");
        func.returnType = Bool;
        func.arguments << Argument("path", String);
        result << func;

        /* алг лог является каталогом(лит имя файла Или каталога) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x000b;
        func.asciiName = "isdir";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("является каталогом");
        func.returnType = Bool;
        func.arguments << Argument("path", String);
        result << func;

        /* алг лог создать каталог(лит имя файла Или каталога) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x000c;
        func.asciiName = "mkdir";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("создать каталог");
        func.returnType = Bool;
        func.arguments << Argument("path", String);
        result << func;

        /* алг лит полный путь(лит имя файла Или каталога) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x000d;
        func.asciiName = "abspath";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("полный путь");
        func.returnType = String;
        func.arguments << Argument("path", String);
        result << func;

        /* алг лог удалить_файл(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x000e;
        func.asciiName = "rm";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("удалить_файл");
        func.returnType = Bool;
        func.arguments << Argument("path", String);
        result << func;

        /* алг НАЗНАЧИТЬ ВВОД(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x000f;
        func.asciiName = "ASSIGN INPUT";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("НАЗНАЧИТЬ ВВОД");
        func.returnType = Void;
        func.arguments << Argument("path", String);
        result << func;

        /* алг НАЗНАЧИТЬ ВЫВОД(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0010;
        func.asciiName = "ASSIGN OUTPUT";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("НАЗНАЧИТЬ ВЫВОД");
        func.returnType = Void;
        func.arguments << Argument("path", String);
        result << func;

        /* алг лит РАБОЧИЙ КАТАЛОГ */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0011;
        func.asciiName = "CURRENT DIR";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("РАБОЧИЙ КАТАЛОГ");
        func.returnType = String;
        result << func;

        /* алг лит КАТАЛОГ ПРОГРАММЫ */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0012;
        func.asciiName = "PROGRAM DIR";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("КАТАЛОГ ПРОГРАММЫ");
        func.returnType = String;
        result << func;

        /* алг лог удалить_каталог(лит имя файла) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0013;
        func.asciiName = "rmdir";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("удалить_каталог");
        func.returnType = Bool;
        func.arguments << Argument("path", String);
        result << func;

        /* алг файл консоль */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0014;
        func.asciiName = "get console";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("консоль");
        func.returnType = RecordType;
        func.returnTypeSpecification = FileType;
        result << func;

        /* алг лог =(файл ф1, файл ф2) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0015;
        func.asciiName = "=";
        func.returnType = Bool;
        func.arguments << Argument("first", RecordType) << Argument("second", RecordType);
        func.arguments[0].typeSpecification = func.arguments[1].typeSpecification = FileType;
        result << func;

        /* алг лог <>(файл ф1, файл ф2) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.returnTypeSpecification = RecordSpecification();
        func.id = 0x0016;
        func.asciiName = "<>";
        func.returnType = Bool;
        func.arguments << Argument("first", RecordType) << Argument("second", RecordType);
        func.arguments[0].typeSpecification = func.arguments[1].typeSpecification = FileType;
        result << func;

        return result;

    }
};

class Strings
        : public Shared::ActorInterface
{
    inline QByteArray asciiModuleName() const { return QByteArray("String Utilities"); }
    inline QString localizedModuleName(const QLocale::Language) const {return QString::fromUtf8("Строки");}

    inline FunctionList functionList() const {
        FunctionList result;
        Function func;
        func.accessType = PublicFunction;

        /* алг лит верхний регистр(лит строка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0000;
        func.asciiName = "uppercase";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("верхний регистр");
        func.returnType = String;
        func.arguments << Argument("s", String);
        result << func;

        /* алг лит нижний регистр(лит строка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0001;
        func.asciiName = "lowercase";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("нижний регистр");
        func.returnType = String;
        func.arguments << Argument("s", String);
        result << func;

        /* алг цел позиция после(цел От, лит фрагмент, лит строка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0002;
        func.asciiName = "index after";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("позиция после");
        func.returnType = Int;
        func.arguments << Argument("from", Int) << Argument("pattern", String) << Argument("source", String);
        result << func;

        /* алг цел позиция(лит фрагмент, лит строка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0003;
        func.asciiName = "index";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("позиция");
        func.returnType = Int;
        func.arguments << Argument("pattern", String) << Argument("source", String);
        result << func;

        /* алг цел поз после(цел От, лит фрагмент, лит строка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0007;
        func.asciiName = "find after";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("поз после");
        func.returnType = Int;
        func.arguments << Argument("from", Int) << Argument("pattern", String) << Argument("source", String);
        result << func;

        /* алг цел поз(лит фрагмент, лит строка) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0008;
        func.asciiName = "find";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("поз");
        func.returnType = Int;
        func.arguments << Argument("pattern", String) << Argument("source", String);
        result << func;

        /* алг вставить(лит фрагмент, аргрез лит строка, арг цел позиция) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0004;
        func.asciiName = "insert";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("вставить");
        func.returnType = Void;
        func.arguments << Argument("data", String) << Argument("source", String) << Argument("ind", Int);
        func.arguments[1].accessType = InOutArgument;
        result << func;

        /* алг заменить(аргрез лит строка, арг лит старый фрагмент, лит новый фрагмент, лог каждый) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0005;
        func.asciiName = "replace";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("заменить");
        func.returnType = Void;
        func.arguments << Argument("source", String) << Argument("pattern", String) << Argument("data", String) << Argument("each", Bool);
        func.arguments[0].accessType = InOutArgument;
        result << func;

        /* алг удалить(аргрез лит строка, арг цел начало, цел количество) */
        func.localizedNames.clear();
        func.arguments.clear();
        func.id = 0x0006;
        func.asciiName = "remove";
        func.localizedNames[QLocale::Russian] = QString::fromUtf8("удалить");
        func.returnType = Void;
        func.arguments << Argument("source", String) << Argument("start", Int) << Argument("count", Int);
        func.arguments[0].accessType = InOutArgument;
        result << func;

        return result;
    }
};

}



#endif
