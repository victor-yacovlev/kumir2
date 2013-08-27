#ifndef PYTHON3LANGUAGE_ANALIZERINSTANCE_H
#define PYTHON3LANGUAGE_ANALIZERINSTANCE_H

#include "interfaces/analizer_instanceinterface.h"

#include <QObject>

namespace Python3Language {

class AnalizerInstance
        : public QObject
        , public Shared::Analizer::InstanceInterface
{
    Q_OBJECT
public:
    explicit AnalizerInstance(QObject *parent = 0);   
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_ANALIZERINSTANCE_H
