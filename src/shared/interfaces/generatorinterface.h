#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "dataformats/ast.h"

typedef QPair<QString,QString> StringPair;

namespace Shared {


#define MIME_BYTECODE_BINARY    "application/vnd.niisi.kumir2.bytecode.binary"
#define MIME_BYTECODE_TEXT      "application/vnd.niisi.kumir2.bytecode.text"
#define MIME_NATIVE_EXECUABLE   "application/octet-stream"

class GeneratorInterface {
public:
    /** Generates execuable by AST
      * @param tree IN:  abstract syntax tree
      * @param out  OUT: output buffer to write (if QFile - not opened state)
      * @returns pair of string: first is error (or empty), second is mimetype
      */
    virtual StringPair generateExecuable(
            const AST::Data * tree
            , QIODevice * out) = 0;
    virtual void setVerbose(bool v) = 0;
    virtual void setTemporaryDir(const QString & path, bool autoclean) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::GeneratorInterface, "kumir2.generator")

#endif
