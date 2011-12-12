#ifndef ST_FUNCT_GENERICINPUT_H
#define ST_FUNCT_GENERICINPUT_H

#include <QtCore>

#ifdef STDLIB_LIBRARY
#define STDLIB_EXPORT Q_DECL_EXPORT
#else
#define STDLIB_EXPORT Q_DECL_IMPORT
#endif

namespace StdLib {

class STDLIB_EXPORT GenericInputOutput : public QObject
{
    Q_OBJECT
public:
    explicit GenericInputOutput(QObject *parent = 0);
    void doInput(const QString &format);
    bool tryFinishInput(const QString & txt,
                        QList<QVariant> & result,
                        QSet< QPair<int,int> > &errpos,
                        bool colorize,
                        QString & error);
    static class GenericInputOutput * instance();
    static QString output(int v, int width, QString & err);
    static QString output(double v, int width, int prec, QString & err);
    static QString output(bool v, int width, QString & err);
    static QString output(const QString &v, int width, QString & err);
    inline static QString output(const QChar &v, int width, QString & err) { return output(QString(v), width, err); }
    inline static QString output(unsigned char v, int width, QString &err) { return output(v>0, width, err); }
    static QString output(wchar_t v, int width, QString & err) { wchar_t buf[1]; buf[0]=v; return output(QString::fromWCharArray(buf, 1), width, err); }
    static QString output(wchar_t * v, int width, QString & err) { return output(QString::fromWCharArray(v), width, err); }
    static QString prepareOutput(const std::string & format, QVariantList vals, QString & err);

private:
    enum InputError {
            OK,
            unpairedQuote,
            quoteMisplace,
            differentQuotes,
            longCharect,
            notReal,
            notBool,
            notInteger
    };
    struct InputTerm {
            QVariant::Type pType;
            QString text;
            bool valid;
            InputError error;
            bool allow_left;
            bool allow_right;
            int start;
            inline InputTerm operator+ (const InputTerm &other) {
                    InputTerm result;
                    QString sp;
                    for ( int i=start+text.length(); i<other.start; i++ )
                            sp += " ";
                    result.text = text + sp + other.text;
                    result.valid = valid && other.valid;
                    result.allow_right = other.allow_right;
                    result.allow_left = allow_left;
                    result.start = start;
                    return result;
            }
    };
    QList<QVariant> finishInput();
    int prepareString(QString &s);
    void split(QString S, QRegExp R, QList<InputTerm> &T);
    bool validateInput(QString txt);
    void validate(QList<QVariant::Type> L, QList<InputTerm> S, QList<InputTerm> &T, bool &R, int &G);
    QList<InputTerm> words;
    QList<QVariant::Type> types;
    QStringList formats;
    int inputGarbageStart;
    static class GenericInputOutput * m_instance;

};

}

extern "C" STDLIB_EXPORT void __input__st_funct(const char * format, int args, ...);
extern "C" STDLIB_EXPORT void __output__st_funct(const char * format, int args, ...);
extern "C" STDLIB_EXPORT void __assign_stream__st_funct(const wchar_t * streamname, const wchar_t * filename);

#endif // ST_FUNCT_GENERICINPUT_H
