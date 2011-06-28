#ifndef BYTECODE_DATA_H
#define BYTECODE_DATA_H

#include "tableelem.h"

#ifdef BYTECODE_LIBRARY
#   define BYTECODE_EXPORT Q_DECL_EXPORT
#else
#   define BYTECODE_EXPORT Q_DECL_IMPORT
#endif


namespace Bytecode {

struct Data {
    QList<TableElem> d;
    quint8 versionMaj;
    quint8 versionMin;
    quint8 versionRel;
    QDateTime lastModified;
};

extern BYTECODE_EXPORT QTextStream& operator<<(QTextStream&, const Data&);
extern BYTECODE_EXPORT QTextStream& operator>>(QTextStream&, Data&);

extern BYTECODE_EXPORT QDataStream& operator<<(QDataStream&, const Data&);
extern BYTECODE_EXPORT QDataStream& operator>>(QDataStream&, Data&);

} // namespace Bytecode

#endif // BYTECODE_DATA_H
