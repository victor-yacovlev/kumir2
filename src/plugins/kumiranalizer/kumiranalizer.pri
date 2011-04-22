include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/interfaces/analizer.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)
LIBS *= -l$$qtLibraryTarget(KumirAnalizer)
