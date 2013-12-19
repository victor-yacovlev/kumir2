/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef TURTLEMODULE_H
#define TURTLEMODULE_H

// Base class include
#include "turtlemodulebase.h"

// Kumir includes
#include "extensionsystem/kplugin.h"
#include "turtle.h"
#include "pult.h"
// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorTurtle {


class TurtleModule
    : public TurtleModuleBase
{
    Q_OBJECT
public /* methods */:
    TurtleModule(ExtensionSystem::KPlugin * parent);
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    QWidget* mainWidget() const;
    QWidget* pultWidget() const;
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void loadActorData(QIODevice * source);
    void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void reset();
    void setAnimationEnabled(bool enabled);
    void runTailUp();
    void runTailDown();
    void runForward(const int dist);
    void runBack(const int dist);
    void runLeft(const int angle);
    void runRight(const int angle);



    /* ========= CLASS PRIVATE ========= */

private:
    turtle* Turtle;
    TurtlePult* Tpult;



};
        

} // namespace ActorTurtle

#endif // TURTLEMODULE_H
