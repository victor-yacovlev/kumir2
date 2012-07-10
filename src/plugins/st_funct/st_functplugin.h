#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/actorinterface.h"


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

    QStringList  funcList() const ; // List of public functions;
    QString name() const;
    inline bool requiresGui() const { return false; }
    QVariant     result() const;
    QList<QVariant> algOptResults() const;//optional results
    inline class QWidget* mainWidget() { return 0; }
    inline class QWidget* pultWidget() { return 0; }
    inline QList<class QMenu*> moduleMenus() const { return QList<class QMenu*>(); }
    inline void setAnimationEnabled(bool) {}
    void reset();
    QString errorText() const;
public slots:

    EvaluationStatus evaluate(quint32 id, const QVariantList & args);

    // Begin function implementations

    EvaluationStatus Abs(const QVariant & x);
    EvaluationStatus ArcCos(const QVariant & x);
    EvaluationStatus ArcSin(const QVariant & x);
    EvaluationStatus ArcCtg(const QVariant & x);
    EvaluationStatus ArcTg(const QVariant & x);
    EvaluationStatus Cos(const QVariant & x);
    EvaluationStatus Ctg(const QVariant & x);
    EvaluationStatus Delay(const QVariant & x);
    EvaluationStatus Div(const QVariant & x, const QVariant & y);
    EvaluationStatus Exp(const QVariant & x);
    EvaluationStatus Iabs(const QVariant & x);
    EvaluationStatus Imax(const QVariant & x, const QVariant & y);
    EvaluationStatus Imin(const QVariant & x, const QVariant & y);
    EvaluationStatus Int(const QVariant & x);
    EvaluationStatus Irand(const QVariant & x, const QVariant & y);
    EvaluationStatus Irnd(const QVariant & x);
    EvaluationStatus Lg(const QVariant & x);
    EvaluationStatus Ln(const QVariant & x);
    EvaluationStatus Max(const QVariant & x, const QVariant & y);
    EvaluationStatus Min(const QVariant & x, const QVariant & y);
    EvaluationStatus Mod(const QVariant & x, const QVariant & y);
    EvaluationStatus Rand(const QVariant & x, const QVariant & y);
    EvaluationStatus Rnd(const QVariant & x);
    EvaluationStatus Sign(const QVariant & x);
    EvaluationStatus Sin(const QVariant & x);
    EvaluationStatus Sqrt(const QVariant & x);
    EvaluationStatus Tg(const QVariant & x);
    EvaluationStatus MaxReal();
    EvaluationStatus MaxInteger();
    EvaluationStatus StrOfReal(const QVariant & x);
    EvaluationStatus StrOfInteger(const QVariant & x);
    EvaluationStatus Time();
    EvaluationStatus Length(const QVariant & x);
    EvaluationStatus Code(const QVariant & x);
    EvaluationStatus RealOfString(const QVariant & x);
    EvaluationStatus IntegerOfString(const QVariant & x);
    EvaluationStatus Symbol(const QVariant & x);
    EvaluationStatus UniSymbol(const QVariant & x);
    EvaluationStatus Unicode(const QVariant & x);

    EvaluationStatus FileExists(const QVariant & x);
    EvaluationStatus FileUnlink(const QVariant & x);
    EvaluationStatus FileSetEncoding(const QVariant & x);
    EvaluationStatus FileOpenForRead(const QVariant & x);
    EvaluationStatus FileOpenForWrite(const QVariant & x);
    EvaluationStatus FileOpenForAppend(const QVariant & x);
    EvaluationStatus FileClose(const QVariant & x);
    EvaluationStatus FileReset(const QVariant & x);
    EvaluationStatus FileAtEnd(const QVariant & x);


    // End function implementations
signals:
    void sync();
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


#endif
