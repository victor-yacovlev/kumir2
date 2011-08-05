TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = \
    shared \
    app/kumir2-ide.pro \
    app/kumir2-cc.pro \
    app/kumir2-bc.pro \
    tools \
    plugins \
    actors
    
!macx:SUBDIRS+=app/kumir2-classic.pro
