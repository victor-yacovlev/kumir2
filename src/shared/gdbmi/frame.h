#ifndef FRAME_H
#define FRAME_H

#include <QtGlobal>
#include <QString>

namespace GdbMi {

struct Frame {
    quint32 level;
    quintptr address;
    QString function;
    QString file;
    quint32 line; // starting from 0
    quint16 threadId;

    Frame* next;
};

}

#endif // FRAME_H
