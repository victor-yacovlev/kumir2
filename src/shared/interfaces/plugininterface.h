#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QString>

namespace Shared {

class PluginInterface {

public:

    enum GlobalState {
        GS_Unlocked, // Edit mode
        GS_Observe, // Observe mode
        GS_Running, // Running mode
        GS_Input,  // User input required
        GS_Pause  // Running paused
    };

    virtual void initialize(const QString & resourcesRootPath) = 0;
    virtual void saveSession() const = 0;
    virtual void restoreSession() = 0;
    virtual void changeCurrentDirectory(const QString & path) = 0;
    virtual void changeGlobalState(GlobalState old, GlobalState current) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::PluginInterface, "kumir2.Plugin")

#endif // PLUGININTERFACE_H
