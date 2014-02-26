#ifndef GDBMI_LOCATION_H
#define GDBMI_LOCATION_H

#include <QObject>

#include "stackframe.h"

QT_BEGIN_NAMESPACE
class QDebug;
class QPoint;
class QMessageBox;
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GdbMi {

class Location
{
public:
    Location() { init(); }
    Location(quint64 address) { init(); m_address = address; }
    Location(const QString &file) { init(); m_fileName = file; }
    Location(const QString &file, int line, bool marker = true)
        { init(); m_lineNumber = line; m_fileName = file; m_needsMarker = marker; }
    Location(const StackFrame &frame, bool marker = true);
    QString fileName() const { return m_fileName; }
    QString functionName() const { return m_functionName; }
    QString from() const { return m_from; }
    int lineNumber() const { return m_lineNumber; }
    void setNeedsRaise(bool on) { m_needsRaise = on; }
    void setNeedsMarker(bool on) { m_needsMarker = on; }
    void setFileName(const QString &fileName) { m_fileName = fileName; }
    bool needsRaise() const { return m_needsRaise; }
    bool needsMarker() const { return m_needsMarker; }
    bool hasDebugInfo() const { return m_hasDebugInfo; }
    quint64 address() const { return m_address; }

private:
    void init() { m_needsMarker = false; m_needsRaise = true; m_lineNumber = -1;
        m_address = 0; m_hasDebugInfo = true; }
    bool m_needsMarker;
    bool m_needsRaise;
    bool m_hasDebugInfo;
    int m_lineNumber;
    QString m_fileName;
    QString m_functionName;
    QString m_from;
    quint64 m_address;
};

} // namespace GdbMi

#endif // GDBMI_LOCATION_H
