#include "sch_program.h"
#include "fp.h"

namespace Schema {

extern bool parceJSON(const QScriptValue &value, Program &program)
{
    if (value.isArray()) {
        int len = value.property("length").toInteger();
        for (int i=0 ; i<len; i++) {
            QScriptValue jsAlg = value.property(i);
            Algorhitm alg;
            if (parceJSON(jsAlg, alg)) {
                program << alg;
            }
        }

    }
    return true;
}



extern QString addLeadingTab(const QString &t);

extern QString generateJSON(const Program &program)
{
    QStringList r;
    for (int i=0; i<program.size(); i++) {
        QStringList lines = generateJSON(program[i]).split("\n");
        for (int j=0; j<lines.size(); j++) {
            lines[j] = ""+lines[j];
        }
        r << lines.join("\n").trimmed();
    }
    QString result = "[ ";
    result += r.join(", ");
    result += " ]";
    return result;
}

extern bool isEqual(const Program &a, const Program &b)
{
    if (a.size()==b.size()) {
        bool difference = false;
        for (int i=0; i<a.size(); i++) {
            difference |= ! isEqual(a[i], b[i]);
        }
        return !difference;
    }
    else {
        return false;
    }
}

} // namespace Schema
