#include "stackframe.h"

#include <QTextStream>
#include <QDebug>

static QString tr(const char * s) { return QString::fromUtf8(s); }

namespace GdbMi {

////////////////////////////////////////////////////////////////////////
//
// StackFrame
//
////////////////////////////////////////////////////////////////////////

StackFrame::StackFrame()
  : level(-1), line(-1), address(0), usable(false)
{}

void StackFrame::clear()
{
    line = level = -1;
    function.clear();
    file.clear();
    from.clear();
    to.clear();
    address = 0;
}

bool StackFrame::isUsable() const
{
    return usable;
}

QString StackFrame::toString() const
{
    QString res;
    QTextStream str(&res);
    str << tr("Address:") << ' ';
    str.setIntegerBase(16);
    str << address;
    str.setIntegerBase(10);
    str << ' '
        << tr("Function:") << ' ' << function << ' '
        << tr("File:") << ' ' << file << ' '
        << tr("Line:") << ' ' << line << ' '
        << tr("From:") << ' ' << from << ' '
        << tr("To:") << ' ' << to;
    return res;
}

//QString StackFrame::toToolTip() const
//{
//    const QString filePath = QDir::toNativeSeparators(file);
//    QString res;
//    QTextStream str(&res);
//    str << "<html><body><table>";
//    if (address)
//        str << "<tr><td>" << tr("Address:") << "</td><td>"
//            << formatToolTipAddress(address) << "</td></tr>";
//    if (!function.isEmpty())
//        str << "<tr><td>" << tr("Function:") << "</td><td>" << function << "</td></tr>";
//    if (!file.isEmpty())
//        str << "<tr><td>" << tr("File:") << "</td><td>" << filePath << "</td></tr>";
//    if (line != -1)
//        str << "<tr><td>" << tr("Line:") << "</td><td>" << line << "</td></tr>";
//    if (!from.isEmpty())
//        str << "<tr><td>" << tr("From:") << "</td><td>" << from << "</td></tr>";
//    if (!to.isEmpty())
//        str << "<tr><td>" << tr("To:") << "</td><td>" << to << "</td></tr>";
//    str << "</table>";

//    str <<"<br> <br><i>" << tr("Note:") << " </i> ";
//    bool showDistributionNote = false;
//    if (isUsable()) {
//        str << tr("Sources for this frame are available.<br>Double-click on "
//            "the file name to open an editor.");
//    } else if (line <= 0) {
//        str << tr("Binary debug information is not accessible for this "
//            "frame. This either means the core was not compiled "
//            "with debug information, or the debug information is not "
//            "accessible.");
//        showDistributionNote = true;
//    } else {
//        str << tr("Binary debug information is accessible for this "
//            "frame. However, matching sources have not been found.");
//        showDistributionNote = true;
//    }
//    if (!Utils::HostOsInfo::isWindowsHost() && showDistributionNote) {
//        str << QLatin1String(" ") <<
//               tr("Note that most distributions ship debug information "
//                  "in separate packages.");
//    }

//    str << "</body></html>";
//    return res;
//}

QDebug operator<<(QDebug d, const  StackFrame &f)
{
    QString res;
    QTextStream str(&res);
    str << "level=" << f.level << " address=" << f.address;
    if (!f.function.isEmpty())
        str << ' ' << f.function;
    if (!f.file.isEmpty())
        str << ' ' << f.file << ':' << f.line;
    if (!f.from.isEmpty())
        str << " from=" << f.from;
    if (!f.to.isEmpty())
        str << " to=" << f.to;
    d.nospace() << res;
    return d;
}
} // namespace GdbMi
