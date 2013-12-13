#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QString>

namespace Shared {

class PluginInterface {

public:

    /** The GlobalState enum defines components behaviour:
     *  GS_Unlocked -   in this state everything is available to user interact
     *  GS_Observe  -   the same as GS_Unlocked, but initially keeps last
     *                  state
     *  GS_Running  -   in this state everyhing is locked to protect
     *                  user interaction modifications while program running
     *  GS_Input    -   The same as GS_Running, but user is able to enter
     *                  input data
     *  GS_Pause    -   The same as GS_Running, but program execution is
     *                  waiting for user to allow continue
     */
    enum GlobalState {
        GS_Unlocked,    /** Edit mode */
        GS_Observe,     /** Observe mode */
        GS_Running,     /** Running mode */
        GS_Input,       /** User input required */
        GS_Pause        /** Running paused */
    };


    /** Initialized plugin before use.
     * Required to call before any plugin's method call in case of using
     * outside of Kumir's Extension System
     * @param resourcesRootPath the absolute path to plugin's resources dir
     */
    virtual void initialize(const QString & resourcesRootPath) = 0;

    /** Notifies plugin to save it's internal state before exit or
     *  settings file change location */
    virtual void saveSession() const = 0;

    /** Notifies plugin to restore it's internal state */
    virtual void restoreSession() = 0;

    /** Changes plugin's default working directory */
    virtual void changeCurrentDirectory(const QString & path) = 0;

    /** Notifies plugin on global state change (see above)
     * @param old       previous state
     * @param current   the new state
     */
    virtual void changeGlobalState(GlobalState old, GlobalState current) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::PluginInterface, "kumir2.Plugin")

#endif // PLUGININTERFACE_H
