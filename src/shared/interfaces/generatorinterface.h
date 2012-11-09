#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "dataformats/ast.h"

namespace Shared {


#define MIME_BYTECODE_BINARY    QString::fromAscii("application/vnd.niisi.kumir2.bytecode.binary")
#define MIME_BYTECODE_TEXT      QString::fromAscii("application/vnd.niisi.kumir2.bytecode.text")
#define MIME_NATIVE_EXECUABLE   QString::fromAscii("application/octet-stream")

class GeneratorInterface {
public:
    enum DebugLevel {
        NoDebug = 0,
        LinesOnly = 1,
        LinesAndVariables = 2
    };

    /** Generates execuable by AST
      * @param tree IN:  abstract syntax tree
      * @param out  OUT: output buffer to write (if QFile - not opened state)
      * @returns pair of string: first is error (or empty), second is mimetype
      */
    virtual QPair<QString,QString> generateExecuable(
            const AST::Data * tree
            , QIODevice * out, DebugLevel debugLevel = LinesOnly) = 0;
    virtual void setVerbose(bool v) = 0;
    virtual void setTemporaryDir(const QString & path, bool autoclean) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::GeneratorInterface, "kumir2.generator")

#endif
