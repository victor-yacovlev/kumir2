#ifndef ACTORINTERFACE_H
#define ACTORINTERFACE_H
#include <QtCore>
//Kumir actor interface
namespace Shared
{

struct Alg {
    QString kumirHeader; // Function header, Kumir syntax.
    QString cHeader; // ASCII function header in C-syntax
    QString pascalHeader; // ASCII function header in Pascal syntax
    int id; //Function id;
};

enum ProgrammingLanguage {
    PL_Kumir,
    PL_C,
    PL_Pascal
};


class ActorInterface
{
public:
    virtual QList<Alg>  funcList() const =0; // List of public functions;
    virtual QString name(ProgrammingLanguage pl = PL_Kumir, QLocale::Language nl = QLocale::Russian) const =0;
    virtual QVariant     result() const =0;
    virtual QString libraryLinkageOptions() const = 0;
    virtual void   runFunct(const QString & alg,const QList<QVariant> &params)=0;
    virtual QList<QVariant> algOptResults() const =0;//optional results
    virtual QWidget* mainWidget()=0;
    virtual void reset() = 0;
    virtual QString errorText() const = 0;
};

}
Q_DECLARE_INTERFACE(Shared::ActorInterface, "kumir2.Actor")
#endif // ACTORINTERFACE_H
