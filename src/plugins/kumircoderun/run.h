#ifndef KUMIRCODERUN_RUN_H
#define KUMIRCODERUN_RUN_H

#include <QtCore>

namespace KumirCodeRun {

class Run : public QThread
{
    Q_OBJECT
public:
    explicit Run(QObject *parent = 0);

public slots:
    void stop();
    void runOneStep();
    void runToEnd();

protected :
    void run();

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_RUN_H
