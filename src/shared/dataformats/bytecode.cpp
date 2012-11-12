#include "bytecode.h"

namespace Bytecode {

extern QTextStream& operator<<(QTextStream& ts, const Data& d)
{
    ts.setCodec("UTF-8");
    ts << "# VERSION " << d.versionMaj << "." << d.versionMin << "." << d.versionRel << "\n\n";
    for (int i=0; i<d.d.size(); i++) {
        ts << d.d[i];
    }
    return ts;
}

extern QTextStream& operator>>(QTextStream& ts, Data& d)
{
    ts.setCodec("UTF-8");
    d.versionMaj = 2;
    d.versionMin = 0;
    d.versionRel = 0;
    while (!ts.atEnd()) {
        TableElem te;
        ts >> te;
        d.d.append(te);
    }
    return ts;
}

extern QDataStream& operator<<(QDataStream& ds, const Data& d)
{
    static const char * bashRun = "#!/usr/bin/env kumir2-run\n";
    ds.writeRawData(bashRun,strlen(bashRun)*sizeof(char));
    ds << d.versionMaj << d.versionMin << d.versionRel;
    for (int i=0; i<d.d.size(); i++) {
        ds << d.d[i];
    }
    return ds;
}

extern std::ostream& operator<<(std::ostream& ds, const Data& d)
{
    static const char * bashRun = "#!/usr/bin/env kumir2-run\n";
    ds << bashRun;
    ds << d.versionMaj << d.versionMin << d.versionRel;
    for (int i=0; i<d.d.size(); i++) {
        ds << d.d[i];
    }
    return ds;
}

extern QDataStream& operator>>(QDataStream& ds, Data& d)
{
    char skipBuffer[1];
    while (!ds.atEnd()) {
        ds.readRawData(skipBuffer, 1);
        if (skipBuffer[0]=='\n')
            break;
    }
    ds >> d.versionMaj;
    ds >> d.versionMin;
    ds >> d.versionRel;
    while (!ds.atEnd()) {
        TableElem te;
        ds >> te;
        d.d.append(te);
    }
    return ds;
}

extern std::istream& operator>>(std::istream &ds, Data &d)
{
    char skipBuffer;
    while (!ds.eof()) {
        ds.get(skipBuffer);
        if (skipBuffer=='\n')
            break;
    }
    ds >> d.versionMaj;
    ds >> d.versionMin;
    ds >> d.versionRel;
    while (!ds.eof()) {
        TableElem te;
        ds >> te;
        d.d.append(te);
    }
    return ds;
}



} // namespace Bytecode
