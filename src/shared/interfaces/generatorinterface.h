#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "dataformats/ast.h"

namespace Shared {

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
      * @param mimeType OUT: MIME type of result
      * @param fileSuffix OUT: suggested filename suffix for result
      * @returns pair of string: first is error (or empty), second is mimetype
      */
    virtual void setDebugLevel(DebugLevel debugLevel) = 0;
    virtual void generateExecuable(
            const AST::DataPtr tree,
            QByteArray & out,
            QString & mimeType,
            QString & fileSuffix
            ) = 0;
    virtual void setOutputToText(bool flag) = 0;
    virtual void setVerbose(bool v) = 0;
    virtual void setTemporaryDir(const QString & path, bool autoclean) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::GeneratorInterface, "kumir2.generator")

#endif
