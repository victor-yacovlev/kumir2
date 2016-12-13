#ifndef MACRO_H
#define MACRO_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtXml>

#include "keycommand.h"

namespace Editor {

struct Macro {
    QString title;
    QChar key;
    Qt::Key extKey;
    QList<KeyCommand> commands;
    QAction * action;
    bool showInLastBlock;
    inline explicit Macro() { action = nullptr; extKey = Qt::Key(0u); showInLastBlock = false; }
};

extern QDomElement dumpMacro(QSharedPointer<Macro> m, QDomDocument &document, QDomElement &root);
extern bool loadMacro(const QDomElement & v, QSharedPointer<Macro> m);
extern QList<QSharedPointer<Macro>> loadFromFile(const QString &fileName);
extern bool saveToFile(const QString &fileName, const QList<QSharedPointer<Macro>> & macros);

}

#endif
