#ifndef ST_FUNCT_GENERICINPUT_H
#define ST_FUNCT_GENERICINPUT_H

#include <QtCore>

namespace st_funct {

class GenericInputOutput : public QObject
{
public:
    explicit GenericInputOutput(QObject *parent = 0);
    void doInput(const QString &format);
    bool tryFinishInput(const QString & txt, QList<QVariant> & result, QString & error);
    static class GenericInputOutput * instance();
    inline static QString output(int v) { return QString::number(v); }
    inline static QString output(double v) { return QString::number(v, 'g', 6); }
    inline static QString output(bool v) { return v? tr("yes") : tr("no"); }
    inline static QString output(unsigned char v) { return output(v>0); }
    inline static QString output(wchar_t v) { wchar_t buf[1]; buf[0]=v; return QString::fromWCharArray(buf, 1); }
    inline static QString output(wchar_t * v) { return QString::fromWCharArray(v); }

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

} // namespace st_funct

extern "C" void __input__st_funct(const char * format, int args, ...);
extern "C" void __output__st_funct(const char * format, int args, ...);

#endif // ST_FUNCT_GENERICINPUT_H
