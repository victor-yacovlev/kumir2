#ifndef MACRO_H
#define MACRO_H

#include <QtCore>
#include <QtGui>
#include <QtXml>

#include "keycommand.h"

namespace Editor {

struct Macro {
    QString title;
    QChar key;
    Qt::Key extKey;
    QList<KeyCommand> commands;
    QAction * action;
    inline explicit Macro() { action = nullptr; extKey = Qt::Key(0u); }
};

extern QDomElement dumpMacro(const Macro & m, QDomDocument &document, QDomElement &root);
extern bool loadMacro(const QDomElement & v, Macro & m);
extern QList<Macro> loadFromFile(const QString &fileName);
extern bool saveToFile(const QString &fileName, const QList<Macro> & macros);

}

#endif
