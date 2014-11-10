/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef KEYBOARDMODULE_H
#define KEYBOARDMODULE_H

// Base class include
#include "keyboardmodulebase.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#if QT_VERSION >= 0x050000
#   include <QtWidgets>
#else
#   include <QtGui>
#endif

namespace ActorKeyboard {


class KeyboardModule
    : public KeyboardModuleBase
{
    Q_OBJECT
public /* methods */:
    KeyboardModule(ExtensionSystem::KPlugin * parent);
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    QWidget* mainWidget() const;
    QWidget* pultWidget() const;
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void loadActorData(QIODevice * source);
    void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void reset();
    void finalizeRun();
    void setAnimationEnabled(bool enabled);
    bool runKeyHit();
    int runKeyCode();
    void runClearKeyBuffer();
    int runKEY_UP();
    int runKEY_DOWN();
    int runKEY_LEFT();
    int runKEY_RIGHT();
    Keycode runOperatorINPUT(const QString& s, bool& ok);
    QString runOperatorOUTPUT(const Keycode &x);

    Keycode runOperatorASSIGN(const int x);
    int runOperatorASSIGN(const ActorKeyboard::Keycode &x);

    bool runOperatorEQUAL(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    bool runOperatorEQUAL(const ActorKeyboard::Keycode &self, int other);
    bool runOperatorEQUAL(int other, const ActorKeyboard::Keycode &self);

    bool runOperatorNOTEQUAL(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    bool runOperatorNOTEQUAL(const ActorKeyboard::Keycode &self, int other);
    bool runOperatorNOTEQUAL(int other, const ActorKeyboard::Keycode &self);

    bool runOperatorLESS(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    bool runOperatorLESS(const ActorKeyboard::Keycode &self, int other);
    bool runOperatorLESS(int self, const ActorKeyboard::Keycode &other);

    bool runOperatorGREATER(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    bool runOperatorGREATER(const ActorKeyboard::Keycode &self, int other);
    bool runOperatorGREATER(int self, const ActorKeyboard::Keycode &other);

    int runOperatorPLUS(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    int runOperatorPLUS(const ActorKeyboard::Keycode &self, int other);
    int runOperatorPLUS(int self, const ActorKeyboard::Keycode &other);

    int runOperatorMINUS(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    int runOperatorMINUS(const ActorKeyboard::Keycode &self, int other);
    int runOperatorMINUS(int self, const ActorKeyboard::Keycode &other);

    int runOperatorASTERISK(const ActorKeyboard::Keycode &self, const ActorKeyboard::Keycode &other);
    int runOperatorASTERISK(const ActorKeyboard::Keycode &self, int other);
    int runOperatorASTERISK(int self, const ActorKeyboard::Keycode &other);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    static int polyakovCodeOfKey(int qtCode, const QString & text);

    QQueue<int> buffer_;
    QMutex bufferLock_;




};
        

} // namespace ActorKeyboard

#endif // KEYBOARDMODULE_H
