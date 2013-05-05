/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef DRAWMODULE_H
#define DRAWMODULE_H

// Base class include
#include "drawmodulebase.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorDraw {


class DrawModule
    : public DrawModuleBase
{
    Q_OBJECT
public /* methods */:
    DrawModule(ExtensionSystem::KPlugin * parent);
    QWidget* mainWidget() const;
    QWidget* pultWidget() const;
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void reloadSettings(QSettings * settings);
    void reset();
    void setAnimationEnabled(bool enabled);
    void runSetupPen();
    void runReleasePen();
    void runSetPenColor(const Color& color);
    void runMoveTo(const qreal x, const qreal y);
    void runMoveBy(const qreal dX, const qreal dY);
    void runAddCaption(const qreal width, const QString& text);
    Color runOperatorINPUT(const QString& x, bool& ok);
    QString runOperatorOUTPUT(const Color& x);



    /* ========= CLASS PRIVATE ========= */






};
        

} // namespace ActorDraw

#endif // DRAWMODULE_H
