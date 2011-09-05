#ifndef GUIRUNNER_CONNECTOR_H
#define GUIRUNNER_CONNECTOR_H

#include <QtCore>

#ifdef STDLIB_LIBRARY
#define STDLIB_EXPORT Q_DECL_EXPORT
#else
#define STDLIB_EXPORT Q_DECL_IMPORT
#endif


namespace StdLib {


class STDLIB_EXPORT Connector
        : public QObject
{
    Q_OBJECT
public:
    explicit Connector();
    static Connector* instance();
    static bool connectedToKumir;

public slots:

    void connectToKumir(int argc, char* *argv);
    void listenFor(QProcess * process);

    void sendReply(const QVariantList & message);

    void waitForEmptyAndStop();
    void stopListen();

    void output(const QString & text);
    QVariantList input(const QString & format);
    void error(const QString & message, int lineNo = -1);
    void resetActor(const QString & pluginName);

    // Returns : [0] - error, [1] - result, [2..N-1] - opt results
    QVariantList evaluateActorCommand(const QString & pluginName,
                                      const QString & command,
                                      const QVariantList & arguments);
signals:
    void outputTextReceived(const QString & text);
    void errorMessageReceived(const QString & message);
    void inputFormatReceived(const QString & format);
    void errorReceived(int lineNo);
    void resetActorReceived(const QString & name);
    void actorCommandReceived(const QString & actorName,
                              const QString & command,
                              const QVariantList & arguments);

private:
    static Connector * m_instance;
#ifdef SHM_METHOD
    class Connector_SHM * d;
#endif
#ifdef PIPE_METHOD
    class Connector_PIPE * d;
#endif
};


} // namespace GuiRunner

struct ActorResponse { QString error; QVariant result; QVariantList res; };

extern "C" STDLIB_EXPORT unsigned char __connected_to_kumir__();
extern "C" STDLIB_EXPORT void __try_connect_to_kumir__(int argc, char* *argv);
extern "C" STDLIB_EXPORT void __wait_for_output_queue_flushed__();
extern STDLIB_EXPORT void __connect_to_kumir__(int argc, char* *argv);
extern STDLIB_EXPORT void __reset_actor__(const QString & moduleName);
extern STDLIB_EXPORT ActorResponse __run_actor_command__(const QString &actor, const QString & command, const QVariantList & arguments);


#endif // GUIRUNNER_CONNECTOR_H
