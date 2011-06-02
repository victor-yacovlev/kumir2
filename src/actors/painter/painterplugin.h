#ifndef ACTORPAINTER_PAINTERPLUGIN_H
#define ACTORPAINTER_PAINTERPLUGIN_H

#include <QtCore>
#include "extensionsystem/kplugin.h"
#include "interfaces/actorinterface.h"

using namespace Shared;

namespace ActorPainter {

class PainterPlugin
        : public ExtensionSystem::KPlugin
        , public ActorInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::ActorInterface)
public:
    PainterPlugin();
    QList<Alg>  funcList() const;
    QString name(ProgrammingLanguage pl = PL_Kumir, QLocale::Language nl = QLocale::Russian) const;
    inline QVariant     result() const { return v_result; }
    inline QStringList actorLibraries() const { return QStringList() << "ActorPainterC"; }
    inline QStringList usedQtLibraries() const { return QStringList() << "QtCore" << "QtGui"; }
    inline bool requiresGui() const { return true; }
    void   runFunct(const QString & alg,const QList<QVariant> &params);
    inline QList<QVariant> algOptResults() const { return l_optResults; }
    VisualComponent* mainWidget();
    void reset();
    QString errorText() const;
protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    QVariantList l_optResults;
    QVariant v_result;
    VisualComponent * m_mainWidget;
};

} // namespace ActorPainter

#endif // ACTORPAINTER_PAINTERPLUGIN_H
