#ifndef MACRO_H
#define MACRO_H

#include <QtCore>
#include <QtGui>
#include <QtScript>

#include "keycommand.h"

namespace Editor {

struct Macro {
    QString title;
    QChar key;
    QList<KeyCommand> commands;
    QAction * action;
};

extern QString dumpMacro(Macro & m);
extern bool loadMacro(const QScriptValue & v, Macro & m);
extern QList<Macro> loadFromFile(const QString &fileName);
extern bool saveToFile(const QString &fileName, const QList<Macro> & macros);

}

#endif
