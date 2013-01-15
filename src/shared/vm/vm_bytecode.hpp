#ifndef BYTECODE_DATA_H
#define BYTECODE_DATA_H

#include "vm_tableelem.hpp"

#include <deque>
#include <stdint.h>


namespace Bytecode {

struct Data {
    std::deque <TableElem> d;
    uint8_t versionMaj;
    uint8_t versionMin;
    uint8_t versionRel;
    unsigned long lastModified;
};

inline void bytecodeToDataStream(std::list<char> & ds, const Data & data)
{
    static const char * header = "#!/usr/bin/env kumir2-run\n";
    for (size_t i=0; i<strlen(header); i++) {
        ds.push_back(header[i]);
    }
    valueToDataStream(ds, data.versionMaj);
    valueToDataStream(ds, data.versionMin);
    valueToDataStream(ds, data.versionRel);
    uint32_t u32size = uint32_t(data.d.size());
    valueToDataStream(ds, u32size);
    for (size_t i=0; i<data.d.size(); i++) {
        tableElemToBinaryStream(ds, data.d.at(i));
    }
}

inline void bytecodeToDataStream(std::ostream & ds, const Data & data)
{
    std::list<char> bytes;
    bytecodeToDataStream(bytes, data);
    char * buffer = reinterpret_cast<char*>(calloc(bytes.size(), sizeof(char)));
    size_t i = 0;
    for (std::list<char>::const_iterator it=bytes.begin(); it!=bytes.end(); ++it) {
        buffer[i] = *it;
        i++;
    }
    ds.write(buffer, bytes.size()*sizeof(char));
    free(buffer);
}

inline void bytecodeFromDataStream(std::list<char> & ds, Data & data)
{
    if (ds.size()>0 && ds.front()=='#') {
        char cur;
        while(1) {
            cur = ds.front();
            ds.pop_front();
            if (cur=='\n')
                break;
        }
    }
    if (ds.size()>0)
        valueFromDataStream(ds, data.versionMaj);
    if (ds.size()>0)
        valueFromDataStream(ds, data.versionMin);
    if (ds.size()>0)
        valueFromDataStream(ds, data.versionRel);
    uint32_t u32_size = 0;
    if (ds.size()>=4)
        valueFromDataStream(ds, u32_size);
    size_t size = size_t(u32_size);
    data.d.resize(size);
    for (size_t i=0; i<size; i++) {
        tableElemFromBinaryStream(ds, data.d.at(i));
    }
}

inline void bytecodeFromDataStream(std::istream & is, Data & data)
{
    std::list<char> bytes;
    while (!is.eof()) {
        char buffer;
        is.read(&buffer, 1);
        bytes.push_back(buffer);
    }
    bytecodeFromDataStream(bytes, data);
}


inline void bytecodeToTextStream(std::ostream & ts, const Data & data)
{
    ts << "#!/usr/bin/env kumir2-run\n";
    ts << "#version " << int(data.versionMaj) << " " << int(data.versionMin) << " " << int(data.versionRel) << "\n\n";
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
