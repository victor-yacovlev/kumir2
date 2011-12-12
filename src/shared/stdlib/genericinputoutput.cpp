#include "genericinputoutput.h"
#include "connector.h"

#include <stdio.h>
#include <limits>

#include "kumstdlib.h"

namespace StdLib {

GenericInputOutput::GenericInputOutput(QObject *parent) :
    QObject(parent)
{
}

QFile __output_file__;
QFile __input_file__;

QString GenericInputOutput::output(int v, int width, QString &err)
{
    err = "";
    if (width<0) {
        err = tr("Negative field width in format");
        return "";
    }
    QString result = QString::number(v);
    if (width!=std::numeric_limits<qint32>::max())
        result = result.rightJustified(width, ' ', true);
    return result;
}

QString GenericInputOutput::output(double v, int width, int prec, QString &err)
{
    if (width<0) {
        err = tr("Negative field width in format");
        return "";
    }
    if (prec<0) {
        err = tr("Negative precision field in format");
        return "";
    }
    QString result = QString::number(v, 'f', prec);
    if (width!=std::numeric_limits<qint32>::max())
        result = result.rightJustified(width, ' ', true);
    return result;
}

QString GenericInputOutput::output(bool v, int width, QString &err)
{
    if (width<0) {
        err = tr("Negative field width in format");
        return "";
    }
    QString result = v? tr("yes") : tr("no");
    if (width!=std::numeric_limits<qint32>::max())
        result = result.rightJustified(width, ' ', true);
    return result;
}

QString GenericInputOutput::output(const QString &v, int width, QString &err)
{
    if (width<0) {
        err = tr("Negative field width in format");
        return "";
    }
    QString result = v;
    if (width!=std::numeric_limits<qint32>::max())
        result = result.rightJustified(width, ' ', true);
    return result;
}

QString GenericInputOutput::prepareOutput(const std::string &format, QVariantList vals, QString &err)
{
    err = "";
    Q_ASSERT(vals.size() == format.length()*3);
    QString result;
    for (int i=0; i<format.length(); i++) {
        if (format[i]=='i') {
            int v = vals.first().toInt();
            vals.pop_front();
            int w = vals.first().toInt();
            vals.pop_front();
            vals.pop_front();
            result += output(v,w,err);
        }
        else if (format[i]=='r') {
            double v = vals.first().toDouble();
            vals.pop_front();
            double w = vals.first().toInt();
            vals.pop_front();
            double p = vals.first().toInt();
            vals.pop_front();
            result += output(v,w,p,err);
        }
        else if (format[i]=='s' || format[i]=='c') {
            QString v = vals.first().toString();
            vals.pop_front();
            int w = vals.first().toInt();
            vals.pop_front();
            vals.pop_front();
            result += output(v,w,err);
        }
        else if (format[i]=='b') {
            bool v = vals.first().toBool();
            vals.pop_front();
            int w = vals.first().toInt();
            vals.pop_front();
            vals.pop_front();
            result += output(v,w,err);
        }
        if (!err.isEmpty())
            return "";
    }
    return result;
}

int GenericInputOutput::prepareString(QString & s)
{
    bool inQuote = false;
    QChar quote;
    int quotePos = -1;
    for ( int i=0; i<s.length(); i++ )
    {
        if ( !inQuote ) {
            if ( s[i]=='"' || s[i]=='\'' ) {
                inQuote = true;
                quote = s[i];
                quotePos = i;
            }
        }
        else {
            if ( s[i] == quote ) {
                inQuote = false;
            }
            if ( s[i] == '"' && s[i]!=quote ) {
                s[i] = QChar(1003);
            }
            if ( s[i] == '\'' && s[i]!=quote ) {
                s[i] = QChar(1004);
            }
            if ( s[i] == ' ' ) {
                s[i] = QChar(1001);
            }
            if ( s[i] == '\t' ) {
                s[i] = QChar(1002);
            }
            if ( s[i] == ',' ) {
                s[i] = QChar(1000);
            }
        }
    }
    if ( inQuote ) {
        for ( int i=quotePos+1; i<s.length(); i++ ) {
            if ( s[i] == QChar(1003) )
                s[i] = '"';
            if ( s[i] == QChar(1004) )
                s[i] = '\'';
            if ( s[i] == QChar(1001) )
                s[i] = ' ';
            if ( s[i] == QChar(1002) )
                s[i] = '\t';
            if ( s[i] == QChar(1000) )
                s[i] = ',';
        }
        return quotePos;
    }
    else {
        return -1;
    }
}

void GenericInputOutput::split(QString S, QRegExp R, QList<InputTerm> &T)
{
    int cur;
    int prev;
    cur = 0;
    QRegExp nonSpace("\\S");
    prev = nonSpace.indexIn(S) > -1 ? nonSpace.indexIn(S) : 0;
    forever {
        InputTerm t;
        bool stop = false;
        cur = R.indexIn(S,prev);
        if ( cur > -1 ) {
            t.text = S.mid(prev,cur-prev);
            t.allow_left = ! (t.text.startsWith("\"") || t.text.startsWith("'") );
            t.allow_right = ! (t.text.endsWith("\"") || t.text.endsWith("'") );
            t.allow_right = t.allow_right && (! R.cap().contains(","));
            t.start = prev;
            prev = cur + R.matchedLength();
        }
        else {
            t.text = S.mid(prev);
            if ( t.text.trimmed().isEmpty() )
                break;
            t.allow_left = ! (t.text.startsWith("\"") || t.text.startsWith("'") );
            t.allow_right = false;
            t.start = prev;
            stop = true;
        }
        T << t;
        if ( stop )
            break;
    }
}

void GenericInputOutput::validate(QList<QVariant::Type> L, QList<InputTerm> S, QList<InputTerm> &T, bool &R, int &G)
{
    int i;
    InputTerm t;
    QVariant::Type l;
    i = 0;
    foreach ( l, L ) {
        if ( i >= S.size() )
            break;
        t = InputTerm();
        if ( l == QVariant::String ) {
            t = S[i];
            while ( t.allow_right && i<(S.size()-1) && S[i].allow_left ) {
                i = i + 1;
                t = t + S[i];
            }
            if ( t.text.contains("\"") ) {
                bool valid = t.text.startsWith("\"") && t.text.endsWith("\"") && t.text.count("\"")==2;
                if ( valid )
                    t.error = OK;
                else {
                    if ( t.text.count("\"") != 2 )
                        t.error = unpairedQuote;
                    else
                        t.error = quoteMisplace;
                }
                t.valid = valid;

            }
            else if ( t.text.contains("'") ) {
                bool valid = t.text.startsWith("'") && t.text.endsWith("'") && t.text.count("'")==2;
                if ( valid )
                    t.error = OK;
                else {
                    if ( t.text.count("'") != 2 )
                        t.error = unpairedQuote;
                    else
                        t.error = quoteMisplace;
                }
                t.valid = valid;
            }
            else {
                t.valid = true;
            }
        }
        if ( l == QVariant::Char ) {
            t = S[i];
            if ( t.text.length() == 1 ) {
                t.valid = true;
                t.error = OK;
            }
            else if ( S[i].text.length()==3 && (S[i].text[0]=='"' || S[i].text[0]=='\'') && S[i].text[0]==S[i].text[2] ) {
                t.error = OK;
                t.valid = true;
            }
            else {
                bool hasQuote = S[i].text.contains("'") || S[i].text.contains("\"");
                if ( S[i].text.length()>3 || !hasQuote )
                    t.error = longCharect;
                else if ( S[i].text[0] != S[i].text[2] )
                    t.error = differentQuotes;
                else
                    t.error = unpairedQuote;
                t.valid = false;
            }
        }
        if ( l == QVariant::Int ) {
            t = S[i];
            bool ok;
            t.text.toInt(&ok);
            if ( !ok && t.text.trimmed().startsWith("$") ) {
                QString h = t.text;
                h.remove(0,1);
                h.toInt(&ok,16);
            }
            //            t.valid = ok && IntegerOverflowChecker::checkFromString(t.text);
            t.valid = ok;
        }
        if ( l == QVariant::Double ) {
            t = S[i];
            bool ok;
            t.text.replace(QString::fromUtf8("Е"),"E");
            t.text.replace(QString::fromUtf8("е"),"e");
            t.text.toDouble(&ok);
            t.valid = ok && t.text.toLower() != "nan" && t.text.toLower() != "inf";
            if  ( t.valid )
                t.error = OK;
            else
                t.error = notReal;
        }
        if ( l == QVariant::Bool ) {
            t = S[i];
            bool ok;
            const QString YES = tr("yes", "GenericInputOutput");
            const QString NO = tr("no", "GenericInputOutput");
            ok = t.text.toLower() == YES;
            ok = ok || t.text.toLower() == NO;
            t.valid = ok;
            if ( ok )
                t.error = OK;
            else
                t.error = notBool;
        }
        t.pType = l;
        T << t;
        i = i + 1;
    }
    if ( i < S.size() ) {
        R = false;
        G = S[i].start;
    }
    else if ( T.size() < L.size() ) {
        R = false;
        G = -1;
    }
    else {
        R = true;
        G = -1;
        foreach ( t, T ) {
            R = R && t.valid;
        }
    }
}

bool GenericInputOutput::validateInput(QString txt)
{

    prepareString(txt);
    QList<InputTerm> terms;
    QRegExp rxDelim = QRegExp("(\\s*,\\s*)|(\\s+)");
    split(txt,rxDelim,terms);
    bool correct;
    words.clear();

    validate(types,terms,words,correct,inputGarbageStart);
    if (types.count()==1 && types.first()==QVariant::String) {
        InputTerm term;
        term.pType = QVariant::String;
        term.text = "";
        term.start = 0;
        term.valid = true;
        terms << term;
        words << term;
        correct = true;
    }
    if ( txt.trimmed().endsWith(",") && correct && ( types.last()!=QVariant::String || words.last().text.endsWith("\"") || words.last().text.endsWith("'") ) ) {
        inputGarbageStart = txt.lastIndexOf(",");
        correct = false;
    }
    else if ( txt.trimmed().endsWith(",") && correct && types.last()==QVariant::String && !(  words.last().text.endsWith("\"") || words.last().text.endsWith("'") ) ) {
        words.last().text+=",";
    }

    QString msg = tr("INPUT ");
    QStringList m;
    foreach ( QVariant::Type pType, types ) {
        QString t;
        if ( pType == QVariant::Int )
            t = tr("integer");
        else if ( pType == QVariant::Double )
            t = tr("real");
        else if ( pType == QVariant::Char )
            t = tr("charect");
        else if ( pType == QVariant::String )
            t = tr("string");
        else if ( pType == QVariant::Bool )
            t = tr("boolean");
        m << t;
    }
    msg += m.join(", ");
    return correct;
}

void GenericInputOutput::doInput(const QString &format)
{
    words.clear();
    types.clear();
    formats = format.split("%",QString::SkipEmptyParts);
    foreach (const QString &fmt, formats )
    {
        if ( fmt.isEmpty() )
            continue;
        QVariant::Type pType;
        if ( fmt=="s" )
            pType = QVariant::String;
        if ( fmt=="d" )
            pType = QVariant::Int;
        if ( fmt=="c" )
            pType = QVariant::Char;
        if ( fmt=="f" )
            pType = QVariant::Double;
        if ( fmt=="b" )
            pType = QVariant::Bool;
        types << pType;
    }
    inputGarbageStart = -1;
}

bool GenericInputOutput::tryFinishInput(const QString &txt,
                                        QList<QVariant> &result,
                                        QSet< QPair<int,int> > &errpos,
                                        bool colorize,
                                        QString &msg)
{
    msg = "";
    bool ok = validateInput(txt);

    if ( ok )
        result = finishInput();
    else {
        foreach ( InputTerm word, words )
        {
            if ( !word.valid && word.start>-1) {
                msg = tr("INPUT ");
                QStringList m;
                foreach ( InputTerm word, words ) {
                    QString t;
                    if ( word.pType == QVariant::Int )
                        t = tr("integer");
                    else if ( word.pType == QVariant::Double )
                        t = tr("real");
                    else if ( word.pType == QVariant::Char )
                        t = tr("charect");
                    else if ( word.pType == QVariant::String )
                        t = tr("string");
                    else if ( word.pType == QVariant::Bool )
                        t = tr("boolean");
                    if ( word.valid )
                        if (colorize)
                            m << "<font color='green'><b>"+t+"</b></font>";
                        else
                            m << t;
                    else if ( word.text.length()>0 ) {
                        if (colorize)
                            m << "<font color='red'><b>"+t+"</b></font>";
                        else
                            m << t;
                        errpos.insert(QPair<int,int>(word.start, word.text.length()));
                    }
                    else
                        m << t;

                }
                for ( int i=words.count(); i<types.count() ; i++ ) {
                    if ( types[i]==QVariant::Int ) m << tr("integer");
                    if ( types[i]==QVariant::Double ) m << tr("real");
                    if ( types[i]==QVariant::Char ) m << tr("charect");
                    if ( types[i]==QVariant::String ) m << tr("string");
                    if ( types[i]==QVariant::Bool ) m << tr("boolean");
                }
                msg += m.join(", ")+".";
                if ( word.text.length()>0 )
                {
                    QString a;
                    if ( word.error == notInteger )
                        a = tr("It is not integer.");
                    else if ( word.error == notReal )
                        a = tr("It is not real.");
                    else if ( word.error == notBool )
                        a = tr("It is not kumBoolean.");
                    else if ( word.error == unpairedQuote )
                        a = tr("Unpaired quote.");
                    else if ( word.error == longCharect )
                        a = tr("More then one character.");
                    else if ( word.error == quoteMisplace )
                        a = tr("Quotes misplaced.");
                    else if ( word.error == differentQuotes )
                        a = tr("Different quotes.");
                    if (colorize)
                        msg += " <font color='red'>"+a+"</font>";
                    else
                        msg += " " + a;
                    errpos.insert(QPair<int,int>(word.start, word.text.length()));
                }
                else {
                    QString a;
                    if ( word.pType==QVariant::Int )
                        a = tr("Integer not entered.");
                    if ( word.pType==QVariant::Double )
                        a = tr("Real not entered.");
                    if ( word.pType==QVariant::Bool )
                        a = tr("Boolean not entered.");
                    if ( word.pType==QVariant::Char )
                        a = tr("Charect not entered.");
                    if (colorize)
                        msg += " <font color='red'>"+a+"</font>";
                    else
                        msg += " " + a;
                }
                return ok;
            }
            else if ( !word.valid ) {
            }
        }
        if ( words.count() < types.count() ) {
            msg = tr("INPUT ");
            QStringList m;
            foreach ( InputTerm word, words ) {
                QString t;
                if ( word.pType == QVariant::Int )
                    t = tr("integer");
                else if ( word.pType == QVariant::Double )
                    t = tr("real");
                else if ( word.pType == QVariant::Char )
                    t = tr("charect");
                else if ( word.pType == QVariant::String )
                    t = tr("string");
                else if ( word.pType == QVariant::Bool )
                    t = tr("boolean");
                if ( word.valid )
                    if (colorize)
                        m << "<font color='green'><b>"+t+"</b></font>";
                    else
                        m << t;
                else if ( word.text.length()>0 ) {
                    if (colorize)
                        m << "<font color='red'><b>"+t+"</b></font>";
                    else
                        m << t;
                    errpos.insert(QPair<int,int>(word.start, word.text.length()));
                }
                else
                    m << t;
            }
            for ( int i=words.count(); i<types.count(); i++ ) {
                QString t;
                QVariant::Type pType = types[i];
                if ( pType == QVariant::Int )
                    t = tr("integer");
                else if ( pType == QVariant::Double )
                    t = tr("real");
                else if ( pType == QVariant::Char )
                    t = tr("charect");
                else if ( pType == QVariant::String )
                    t = tr("string");
                else if ( pType == QVariant::Bool )
                    t = tr("boolean");
                m << t;
            }
            msg += m.join(", ")+".";
            QVariant::Type pType = types[words.count()];
            QString a;
            if ( pType==QVariant::Int )
                a = tr("Integer not entered.");
            if ( pType==QVariant::Double )
                a = tr("Real not entered.");
            if ( pType==QVariant::Bool )
                a = tr("Boolean not entered.");
            if ( pType==QVariant::Char )
                a = tr("Charect not entered.");
            if ( pType==QVariant::String )
                a = tr("String not entered.");
            if (colorize)
                msg += " <font color='red'>"+a+"</font>";
            else
                msg += " "+a;
        }
        if ( inputGarbageStart > -1 ) {
            msg = tr("INPUT ");
            QStringList m;
            foreach ( InputTerm word, words ) {
                QString t;
                if ( word.pType == QVariant::Int)
                    t = tr("integer");
                else if ( word.pType == QVariant::Double )
                    t = tr("real");
                else if ( word.pType == QVariant::Char )
                    t = tr("charect");
                else if ( word.pType == QVariant::String )
                    t = tr("string");
                else if ( word.pType == QVariant::Bool )
                    t = tr("boolean");
                if ( word.valid )
                    if (colorize)
                        m << "<font color='green'><b>"+t+"</b></font>";
                    else
                        m << t;
                else if ( word.text.length()>0 ) {
                    if (colorize)
                        m << "<font color='red'><b>"+t+"</b></font>";
                    else
                        m << t;
                    errpos.insert(QPair<int,int>(word.start, word.text.length()));
                }
                else
                    m << t;

            }
            msg += m.join(", ") + ".";
            const QString a = tr("Garbage after input string.");
            if (colorize)
                msg += " <font color='red'>"+a+"</font>";
            else
                msg += " "+a;
            errpos.insert(QPair<int,int>(inputGarbageStart, txt.length()-inputGarbageStart));

        }
    }
    return ok;
}

QList<QVariant> GenericInputOutput::finishInput()
{
    QList<QVariant> ret;
    foreach ( InputTerm word, words ) {
        if ( word.pType == QVariant::String ) {
            if (word.text.startsWith("\"") && word.text.endsWith("\"")) {
                word.text = word.text.mid(1,word.text.length()-2);
            }
            if (word.text.startsWith("'") && word.text.endsWith("'")) {
                word.text = word.text.mid(1,word.text.length()-2);
            }
            word.text.replace(QString(QChar(1000)),",");
            word.text.replace(QString(QChar(1001))," ");
            word.text.replace(QString(QChar(1002)),"\t");
            word.text.replace(QString(QChar(1003)),"\"");
            word.text.replace(QString(QChar(1004)),"'");
            ret << QVariant(word.text);
        }
        else if ( word.pType==QVariant::Char )  {
            QString QUOTE = QString(QChar(1001)) + QString(QChar(1001));
            if ( word.text.length() == 3 ) {
                word.text.remove(0,1);
                word.text.remove(1,1);
                word.text.replace(QString(QChar(1000)),",");
                word.text.replace(QString(QChar(1001))," ");
                word.text.replace(QString(QChar(1002)),"\t");
                word.text.replace(QString(QChar(1003)),"\"");
                word.text.replace(QString(QChar(1004)),"'");
            }
            if ( word.text.length() > 1 )
                word.text = word.text.trimmed();
            if ( word.text.isEmpty() )
                word.text = " ";
            ret << QVariant(word.text[0]);
        }
        else if ( word.pType==QVariant::Int ) {
            if ( word.text.trimmed().startsWith("$") ) {
                word.text.remove(0,1);
                bool ok;
                int value = word.text.toInt(&ok,16);
                Q_ASSERT(ok);
                ret << QVariant(value);
            }
            else {
                ret << QVariant(word.text.trimmed().toInt());
            }
        }
        else if ( word.pType==QVariant::Double ) {
            word.text.replace(QString::fromUtf8("Е"),"E");
            word.text.replace(QString::fromUtf8("е"),"e");
            word.text.replace("E", "e");
            ret << QVariant(word.text.trimmed().toDouble());
        }
        else if ( word.pType==QVariant::Bool ){
            ret << QVariant(word.text.trimmed().toLower()==tr("yes"));
        }
        else {
            ret << QVariant::Invalid;
        }
    }
    return ret;
}

GenericInputOutput* GenericInputOutput::m_instance = 0;
GenericInputOutput* GenericInputOutput::instance()
{
    if (!m_instance)
        m_instance = new GenericInputOutput(0);
    return m_instance;
}

}

#include <stdarg.h>
#include <wchar.h>
//#include <iostream>

extern "C" void __input__st_funct(const char * format, int args, ...)
{
    const QString fmt = QString::fromLatin1(format);
    QList<QVariant> result;
    if (StdLib::__input_file__.isOpen()) {
        QString line = QString::fromLocal8Bit(StdLib::__input_file__.readLine());
        StdLib::GenericInputOutput * inp = StdLib::GenericInputOutput::instance();
        inp->doInput(fmt);
        QSet<QPair<int,int> > errpos;
        QString error;
        bool ok = inp->tryFinishInput(line, result, errpos, false, error);
        if (!ok) {
            QString msg = QString::fromAscii("Can't read from file %1: %2\n")
                    .arg(StdLib::__input_file__.fileName())
                    .arg(error);
            wchar_t * msgW = (wchar_t*)calloc(msg.length()+1, sizeof(wchar_t));
            msg.toWCharArray(msgW);
            msgW[msg.length()] = L'\0';
            __abort__(msgW, -1);
        }
    }
    else if ( ! __connected_to_kumir__()) {
//        std::cerr << "Not connected to kumir\n";
        StdLib::GenericInputOutput * inp = StdLib::GenericInputOutput::instance();
        inp->doInput(fmt);
        wchar_t buffer[4096];
        wchar_t err[256];
        QString error;
        bool ok;
        do {
            wscanf(L"%ls", &buffer);
            QSet<QPair<int,int> > errpos;
            ok = inp->tryFinishInput(QString::fromWCharArray(buffer), result, errpos, false, error);
            if (!ok) {
                err[error.toWCharArray(err)] = L'\0';
                fwprintf(stderr, L"%ls\n", err);
            }
        } while (!ok);
    }
    else {
//        std::cerr << "Connected to kumir\n";
        result = StdLib::Connector::instance()->input(fmt);
    }
    va_list vl;
    va_start(vl, args);
    for (int i=0; i<args; i++) {
        if (result[i].type()==QVariant::Int) {
            int * v = va_arg(vl, int*);
            *v = result.at(i).toInt();
        }
        else if (result[i].type()==QVariant::Double) {
            double * v = va_arg(vl, double*);
            *v = result.at(i).toDouble();
        }
        else if (result[i].type()==QVariant::String) {
            wchar_t* * v = va_arg(vl, wchar_t**);
            wchar_t * res = (wchar_t*)calloc(result.at(i).toString().length()+1, sizeof(wchar_t));
            result.at(i).toString().toWCharArray(res);
            res[result.at(i).toString().length()] = L'\0';
            *v = res;
        }
        else if (result[i].type()==QVariant::Char) {
            wchar_t * v = va_arg(vl, wchar_t*);
            wchar_t buf[2];
            result.at(i).toString().toWCharArray(buf);
            *v = buf[0];
        }
        else if (result[i].type()==QVariant::Bool) {
            unsigned char * v = va_arg(vl, unsigned char *);
            *v = result.at(i).toBool()? 1 : 0;
        }
    }
    va_end(vl);
}

extern "C" void __output__st_funct(const char * format, int args, ...)
{
    QString result;
    QString fmt = QString::fromLatin1(format);
    va_list vl;
    va_start(vl, args);
    QString err;
    for (int i=0; i<fmt.length(); i++) {
        if (fmt[i]=='i') {
            int v = va_arg(vl, int);
            int width = va_arg(vl, int);
            int prec = va_arg(vl, int);
            result += StdLib::GenericInputOutput::output(v, width, err);
        }
        else if (fmt[i]=='r') {
            double v = va_arg(vl, double);
            int width = va_arg(vl, int);
            int prec = va_arg(vl, int);
            result += StdLib::GenericInputOutput::output(v, width, prec, err);
        }
        else if (fmt[i]=='c') {
            wchar_t v = va_arg(vl, int);
            int width = va_arg(vl, int);
            int prec = va_arg(vl, int);
            result += StdLib::GenericInputOutput::output(v, width, err);
        }
        else if (fmt[i]=='s') {
            wchar_t * v = va_arg(vl, wchar_t*);
            int width = va_arg(vl, int);
            int prec = va_arg(vl, int);
            result += StdLib::GenericInputOutput::output(v, width, err);
        }
        else if (fmt[i]=='b') {
            unsigned char v = va_arg(vl, int);
            int width = va_arg(vl, int);
            int prec = va_arg(vl, int);
            result += StdLib::GenericInputOutput::output(v, width, err);
        }
        if (!err.isEmpty())
            break;
    }
    va_end(vl);
    if (err.isEmpty()) {
        if (StdLib::__output_file__.isOpen()) {
            StdLib::__output_file__.write(result.toLocal8Bit());
        }
        else {
            if (!__connected_to_kumir__()) {
                wchar_t * buffer = (wchar_t*)calloc(result.length()+1, sizeof(wchar_t));
                result.toWCharArray(buffer);
                buffer[result.length()] = L'\0';
                wprintf(L"%ls", buffer);
                free(buffer);
            }
            else {
                StdLib::Connector::instance()->output(result);
            }
        }
    }
    else {
        __set_error__st_funct(err);
    }
}

extern "C" void __assign_stream__st_funct(const wchar_t * streamname, const wchar_t * filename)
{
    const QString streamName = QString::fromWCharArray(streamname).toLower().trimmed();
    const QString fileName = QString::fromWCharArray(filename).trimmed();
    if (streamName=="stdin") {
        StdLib::__input_file__.close();
        if (!fileName.isEmpty()) {
            StdLib::__input_file__.setFileName(fileName);
            if (!StdLib::__input_file__.exists()) {
                QString msg = QString::fromAscii("Input file not exists: %1\n")
                        .arg(StdLib::__input_file__.fileName())
                        ;
                wchar_t * msgW = (wchar_t*)calloc(msg.length()+1, sizeof(wchar_t));
                msg.toWCharArray(msgW);
                msgW[msg.length()] = L'\0';
                __abort__(msgW, -1);
            }
            if (!StdLib::__input_file__.open(QIODevice::ReadOnly)) {
                QString msg = QString::fromAscii("Can't open file %1: Access denied\n")
                        .arg(StdLib::__input_file__.fileName())
                        ;
                wchar_t * msgW = (wchar_t*)calloc(msg.length()+1, sizeof(wchar_t));
                msg.toWCharArray(msgW);
                msgW[msg.length()] = L'\0';
                __abort__(msgW, -1);
            }
        }
    }
    else if (streamName=="stdout") {
        StdLib::__output_file__.close();
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            if (!info.dir().exists()) {
                if (QDir::root().mkpath(info.dir().absolutePath())) {
                    QString msg = QString::fromAscii("Can't create directory %1: Access denied\n")
                            .arg(info.dir().absolutePath())
                            ;
                    wchar_t * msgW = (wchar_t*)calloc(msg.length()+1, sizeof(wchar_t));
                    msg.toWCharArray(msgW);
                    msgW[msg.length()] = L'\0';
                    __abort__(msgW, -1);
                }
            }
            if (!StdLib::__output_file__.open(QIODevice::WriteOnly)) {
                QString msg = QString::fromAscii("Can't open output file %1 for writing: Access denied\n")
                        .arg(info.absoluteFilePath())
                        ;
                wchar_t * msgW = (wchar_t*)calloc(msg.length()+1, sizeof(wchar_t));
                msg.toWCharArray(msgW);
                msgW[msg.length()] = L'\0';
                __abort__(msgW, -1);
            }
        }
    }
}
