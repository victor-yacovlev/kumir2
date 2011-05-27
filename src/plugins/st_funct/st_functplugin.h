#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/actorinterface.h"

#ifdef __cplusplus

using namespace Shared;

namespace st_funct {

class St_functPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::ActorInterface

{
    Q_OBJECT
    Q_INTERFACES(Shared::ActorInterface)
public:
    St_functPlugin();
    ~St_functPlugin();

    QList<Alg>  funcList() const ; // List of public functions;
    QString name(ProgrammingLanguage pl = PL_Kumir, QLocale::Language nl = QLocale::Russian) const;
    QString libraryLinkageName() const;
    QVariant     result() const;
    void   runFunct(const QString &alg, const QList<QVariant> &params);
    QList<QVariant> algOptResults() const;//optional results
    QWidget* mainWidget();
    void reset();
    QString errorText() const;

protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    QString errText;
    QVariant Res;
    QList<QVariant> optResults;

};

}

#endif // __cplusplus

// Begin C-headers

#ifdef __cplusplus
#   define EXTERN_FUNC_DECL extern "C"
#else
#   define EXTERN_FUNC_DECL extern
#endif

EXTERN_FUNC_DECL void __init__st_funct();

EXTERN_FUNC_DECL double abs__st_funct(double x);
EXTERN_FUNC_DECL double arccos__st_funct(double x);
EXTERN_FUNC_DECL double arcctg__st_funct(double x);
EXTERN_FUNC_DECL double arcsin__st_funct(double x);
EXTERN_FUNC_DECL double arctg__st_funct(double x);
EXTERN_FUNC_DECL double cos__st_funct(double x);
EXTERN_FUNC_DECL double ctg__st_funct(double x);
EXTERN_FUNC_DECL void delay__st_funct(int s);
EXTERN_FUNC_DECL int div__st_funct(int x, int y);
EXTERN_FUNC_DECL double exp__st_funct(double x);
EXTERN_FUNC_DECL int iabs__st_funct(int x);
EXTERN_FUNC_DECL int imax__st_funct(int x, int y);
EXTERN_FUNC_DECL int imin__st_funct(int x, int y);
EXTERN_FUNC_DECL int int__st_funct(double x);
EXTERN_FUNC_DECL int irand__st_funct(int a, int b);
EXTERN_FUNC_DECL int irnd__st_funct(int x);
EXTERN_FUNC_DECL double lg__st_funct(double x);
EXTERN_FUNC_DECL double ln__st_funct(double x);
EXTERN_FUNC_DECL double max__st_funct(double x, double y);
EXTERN_FUNC_DECL double min__st_funct(double x, double y);
EXTERN_FUNC_DECL int mod__st_funct(int x, int y);
EXTERN_FUNC_DECL double rand__st_funct(double a, double b);
EXTERN_FUNC_DECL double rnd__st_funct(double x);
EXTERN_FUNC_DECL int sign__st_funct(double x);
EXTERN_FUNC_DECL double sin__st_funct(double x);
EXTERN_FUNC_DECL double sqrt__st_funct(double x);
EXTERN_FUNC_DECL double tg__st_funct(double x);
EXTERN_FUNC_DECL double MAXREAL__st_funct();
EXTERN_FUNC_DECL int MAXINT__st_funct();
EXTERN_FUNC_DECL wchar_t* real_to_string__st_funct(double x);
EXTERN_FUNC_DECL int time__st_funct();
EXTERN_FUNC_DECL int length__st_funct(wchar_t * s);
EXTERN_FUNC_DECL int code__st_funct(wchar_t c);
EXTERN_FUNC_DECL double string_to_real__st_funct(wchar_t * s, unsigned char *success);
EXTERN_FUNC_DECL int string_to_int__st_funct(wchar_t * s, unsigned char *success);
EXTERN_FUNC_DECL wchar_t symbol__st_funct(int n);
EXTERN_FUNC_DECL wchar_t symbol2__st_funct(int n);
EXTERN_FUNC_DECL wchar_t * int_to_string__st_funct(int x);
EXTERN_FUNC_DECL int unicode__st_funct(wchar_t c);


#endif
