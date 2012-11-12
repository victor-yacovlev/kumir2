#ifndef BYTECODE_DATA_H
#define BYTECODE_DATA_H

#include "vm_tableelem.h"

#include <deque>


namespace Bytecode {

struct Data {
    std::deque <TableElem> d;
    uint8_t versionMaj;
    uint8_t versionMin;
    uint8_t versionRel;
    std::string characterEncoding;
    unsigned long lastModified;
};

inline void bytecodeToDataStream(std::ostream & ds, const Data & data)
{
    ds << "#!/bin/env kumir2-run\n";
    valueToDataStream(ds, data.versionMaj);
    valueToDataStream(ds, data.versionMin);
    valueToDataStream(ds, data.versionRel);
    stringToDataStream(ds, data.characterEncoding);
    valueToDataStream(ds, uint32_t(data.d.size()));
    for (size_t i=0; i<data.d.size(); i++) {
        tableElemToBinaryStream(ds, data.d.at(i));
    }
}

inline void bytecodeFromDataStream(std::istream & ds, Data & data)
{
    std::string dummy;
    std::getline(ds, dummy);
    valueFromDataStream(ds, data.versionMaj);
    valueFromDataStream(ds, data.versionMin);
    valueFromDataStream(ds, data.versionRel);
    stringFromDataStream(ds, data.characterEncoding);
    uint32_t u32_size;
    valueFromDataStream(ds, u32_size);
    size_t size = size_t(u32_size);
    data.d.resize(size);
    for (size_t i=0; i<size; i++) {
        tableElemFromBinaryStream(ds, data.d.at(i));
    }
}

inline void bytecodeToTextStream(std::ostream & ts, const Data & data)
{
    ts << "#!/bin/env kumir2-run -S\n";
    ts << "#version " << data.versionMaj << " " << data.versionMin << " " << data.versionRel << "\n";
    ts << "#encoding " << data.characterEncoding << "\n\n";
    for (size_t i=0; i<data.d.size(); i++) {
        tableElemToTextStream(ts, data.d.at(i));
        ts << "\n";
    }
}

inline void bytecodeFromTextStream(std::istream & ts, Data & data)
{
    std::string line;
    data.versionMaj = 1;
    data.versionMin = 99;
    data.versionRel = 0;
    data.characterEncoding = "UTF-8";
    while (!ts.eof()) {
        getline(ts, line);
        if (line.length()==0)
            break;
        if (line.at(0)!='#')
            throw std::string("Header elements must start with '#' symbol");
        std::istringstream is(line);
        std::string lexem;
        is >> lexem;
        if (lexem=="#version") {
            is >> data.versionMaj >> data.versionMin >> data.versionRel;
            if (is.fail())
                throw std::string("Wrong version number in header");
        }
        else if (lexem=="#encoding") {
            is >> data.characterEncoding;
            if (is.fail())
                throw std::string("Wrong character encoding in header");
        }
    }
    data.d.resize(20);
    size_t realCount = 0;
    while (!ts.eof()) {
        TableElem e;
        tableElemFromTextStream(ts, e);
        if (realCount>=data.d.size()) {
            data.d.resize(data.d.size()+20);
        }
        data.d.at(realCount) = e;
        realCount ++;
    }
    data.d.resize(realCount);
}

} // namespace Bytecode

#endif // BYTECODE_DATA_H
