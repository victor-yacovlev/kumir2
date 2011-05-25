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
    QString name() const;
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

namespace st_funct {


EXTERN_FUNC_DECL void __init__();

EXTERN_FUNC_DECL double abs(double x);
EXTERN_FUNC_DECL double arccos(double x);
EXTERN_FUNC_DECL double arcctg(double x);
EXTERN_FUNC_DECL double arcsin(double x);
EXTERN_FUNC_DECL double arctg(double x);
EXTERN_FUNC_DECL double cos(double x);
EXTERN_FUNC_DECL double ctg(double x);
EXTERN_FUNC_DECL void delay(int s);
EXTERN_FUNC_DECL int div(int x, int y);
EXTERN_FUNC_DECL double exp(double x);
EXTERN_FUNC_DECL int iabs(int x);
EXTERN_FUNC_DECL int imax(int x, int y);
EXTERN_FUNC_DECL int imin(int x, int y);
EXTERN_FUNC_DECL int int_(double x);
EXTERN_FUNC_DECL int irand(int a, int b);
EXTERN_FUNC_DECL int irnd(int x);
EXTERN_FUNC_DECL double lg(double x);
EXTERN_FUNC_DECL double ln(double x);
EXTERN_FUNC_DECL double max(double x, double y);
EXTERN_FUNC_DECL double min(double x, double y);
EXTERN_FUNC_DECL int mod(int x, int y);
EXTERN_FUNC_DECL double rand(double a, double b);
EXTERN_FUNC_DECL double rnd(double x);
EXTERN_FUNC_DECL int sign(double x);
EXTERN_FUNC_DECL double sin(double x);
EXTERN_FUNC_DECL double sqrt(double x);
EXTERN_FUNC_DECL double tg(double x);
EXTERN_FUNC_DECL double MAXREAL();
EXTERN_FUNC_DECL int MAXINT();
EXTERN_FUNC_DECL wchar_t* real_to_string(double x);
EXTERN_FUNC_DECL int time();
EXTERN_FUNC_DECL int length(wchar_t * s);
EXTERN_FUNC_DECL int code(wchar_t c);
EXTERN_FUNC_DECL double string_to_real(wchar_t * s, unsigned char *success);
EXTERN_FUNC_DECL int string_to_int(wchar_t * s, unsigned char *success);
EXTERN_FUNC_DECL wchar_t symbol(int n);
EXTERN_FUNC_DECL wchar_t symbol2(int n);
EXTERN_FUNC_DECL wchar_t * int_to_string(int x);
EXTERN_FUNC_DECL int unicode(wchar_t c);


}

#endif
