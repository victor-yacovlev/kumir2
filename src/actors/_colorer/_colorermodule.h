/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef _COLORERMODULE_H
#define _COLORERMODULE_H

// Base class include
#include "_colorermodulebase.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace Actor_Colorer {


class _ColorerModule
    : public _ColorerModuleBase
{
    Q_OBJECT
public /* methods */:
    _ColorerModule(ExtensionSystem::KPlugin * parent);
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void loadActorData(QIODevice * source);
    void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void reset();
    Color runOperatorINPUT(const QString& x, bool& ok);
    QString runOperatorOUTPUT(const Color& x);
    bool runOperatorEQUAL(const Color& x, const Color& y);
    bool runOperatorNOTEQUAL(const Color& x, const Color& y);


    /* ========= CLASS PRIVATE ========= */






};
        

} // namespace Actor_Colorer

#endif // _COLORERMODULE_H
