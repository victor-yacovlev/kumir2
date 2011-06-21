#ifndef GUIRUNNER_H
#define GUIRUNNER_H

#include <QtCore>
#include <QtGui>

#ifdef STDLIB_LIBRARY
#define STDLIB_EXPORT Q_DECL_EXPORT
#else
#define STDLIB_EXPORT Q_DECL_IMPORT
#endif

extern "C" STDLIB_EXPORT void __main_gui__( int argc, char *argv[],
                              void(*creator_functs)(),
                              void(*main_funct)() );


extern STDLIB_EXPORT void __create_window_for__(QWidget * widget, const QList<QAction*> & menus);


#endif // GUIRUNNER_H
