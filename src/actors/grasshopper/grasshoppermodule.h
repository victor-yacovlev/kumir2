/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef GRASSHOPPERMODULE_H
#define GRASSHOPPERMODULE_H

// Base class include
#include "grasshoppermodulebase.h"

// Kumir includes
#include <kumir2-libs/extensionsystem/kplugin.h>
//module includes
#include "kuznec.h"
// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorGrasshopper {


class GrasshopperModule
    : public GrasshopperModuleBase
{
    Q_OBJECT
public /* methods */:
    GrasshopperModule(ExtensionSystem::KPlugin * parent);
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    QWidget* mainWidget() const;
    QWidget* pultWidget() const;
    QString initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &runtimeParameters);
    QVariantList templateParameters() const;
    void jumpsChanged();
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void loadActorData(QIODevice * source);
    void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void reset();
    inline void terminateEvaluation() {}
    void setAnimationEnabled(bool enabled);
    void runGoForward(const int distance);
    void runGoBackward(const int distance);
    void runDoRepaint();



    /* ========= CLASS PRIVATE ========= */


private:
    void createGui();
 KumKuznec*  kuznec;
    


};
        

} // namespace ActorGrasshopper

#endif // GRASSHOPPERMODULE_H
