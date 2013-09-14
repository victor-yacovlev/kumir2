/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef VODOLEYMODULE_H
#define VODOLEYMODULE_H

// Base class include
#include "vodoleymodulebase.h"
#include "vodoley.h"
// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorVodoley {


class VodoleyModule
    : public VodoleyModuleBase
{
    Q_OBJECT
public /* methods */:
    VodoleyModule(ExtensionSystem::KPlugin * parent);
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    QWidget* mainWidget() const;
    QWidget* pultWidget() const;
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void loadActorData(QIODevice * source);
    void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void reset();
    void setAnimationEnabled(bool enabled);
    void runFillA();
    void runFillB();
    void runFillC();
    void runEmptyA();
    void runEmptyB();
    void runEmptyC();
    void runFromAToB();
    void runFromAToC();
    void runFromBToA();
    void runFromBToC();
    void runFromCToB();
    void runFromCToA();
    bool runTaskComplited();



    /* ========= CLASS PRIVATE ========= */

private:
    Vodoley *MainWindow;




};
        

} // namespace ActorVodoley

#endif // VODOLEYMODULE_H
