#ifndef GUIRUNNER_H
#define GUIRUNNER_H

#include <QtCore>
#include <QtGui>

extern "C" void __main_gui__( int argc, char *argv[],
                              void(*creator_functs)(),
                              void(*main_funct)() );


extern void __create_window_for__(ExtensionSystem::VisualComponent * component);


#endif // GUIRUNNER_H
