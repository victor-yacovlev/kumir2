#ifndef GDBMI_STACKFRAME_H
#define GDBMI_STACKFRAME_H

#include <QCoreApplication>
#include <QMetaType>

QT_BEGIN_NAMESPACE
class QDebug;
QT_END_NAMESPACE

namespace GdbMi {

class StackFrame
{
public:
    StackFrame();
    void clear();
    bool isUsable() const;
//    QString toToolTip() const;
    QString toString() const;

public:
    qint32 level;
    QString function;
    QString file;  // We try to put an absolute file name in there.
    QString from;  // Sometimes something like "/usr/lib/libstdc++.so.6"
    QString to;    // Used in ScriptEngine only.
    qint32 line;
    quint64 address;
    bool usable;
};

QDebug operator<<(QDebug d, const StackFrame &frame);

typedef QList<StackFrame> StackFrames;

} // namespace GdbMi

Q_DECLARE_METATYPE(GdbMi::StackFrame)

#endif // GDBMI_STACKFRAME_H
