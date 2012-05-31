#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "dataformats/ast.h"
#include <vector>
#include <string>
#include <list>

namespace Shared {


#define MIME_BYTECODE_BINARY    QString::fromAscii("application/vnd.niisi.kumir2.bytecode.binary")
#define MIME_BYTECODE_TEXT      QString::fromAscii("application/vnd.niisi.kumir2.bytecode.text")
#define MIME_LLVM_MODULE_BINARY QString::fromAscii("application/x-object")
#define MIME_NATIVE_EXECUABLE   QString::fromAscii("application/octet-stream")
#define MIME_MULTIPART          QString::fromAscii("multipart")

struct MultipartEntry {
    std::string mimeType;
    std::vector<unsigned char> content;
    inline QByteArray toByteArray() const {
        QByteArray res;
        QDataStream stream(&res, QIODevice::WriteOnly);
        stream << QString::fromStdString(mimeType);
        stream << QVector<unsigned char>::fromStdVector(content);
        return res;
    }
    inline void fromByteArray(const QByteArray &data) {
        QDataStream stream(data);
        QString qs;
        QVector<unsigned char> qv;
        stream >> qs;
        stream >> qv;
        mimeType = qs.toStdString();
        content = qv.toStdVector();
    }
};

typedef std::list<MultipartEntry> MultipartData;

class GeneratorInterface {
public:
    /** Generates execuable by AST
      * @param tree IN:  abstract syntax tree
      * @param out  OUT: output buffer to write (if QFile - not opened state)
      * @returns pair of string: first is error (or empty), second is mimetype
      */
    virtual QPair<QString,QString> generateExecuable(
            const AST::Data * tree
            , QIODevice * out, QStringList * usedDlls = 0) = 0;
    virtual void setVerbose(bool v) {}
    virtual void setTemporaryDir(const QString & path, bool autoclean) {}
};

}

Q_DECLARE_INTERFACE(Shared::GeneratorInterface, "kumir2.generator")

#endif
