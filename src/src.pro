TEMPLATE = app
TARGET = udp4all
QT += core \
    gui \
    network

console { 
    QT -= gui
    TARGET = udp4alld
    DEFINES += DAEMON
    HEADERS += logger.h
    SOURCES += logger.cpp
    message("Build daemon udp4alld")
} else { 
    message("Build GUI version udp4all")
    HEADERS += forwarderdlg.h \
        forwardermodel.h \
        mainwin.h
    SOURCES += forwarderdlg.cpp \
        forwardermodel.cpp \
        mainwin.cpp
    FORMS += forwarderdlg.ui \
        mainwin.ui
    RESOURCES = udp4all.qrc
    RC_FILE = udp4all.rc
}
HEADERS += gaps2praveprocessor.h \
    gaps2gpsprocessor.h \
    linesplitprocessor.h \
    gaps2msfprocessor.h \
    nmearecord.h \
    forwardmanager.h \
    dataprocessor.h \
    udpforwarder.h
SOURCES += gaps2praveprocessor.cpp \
    gaps2gpsprocessor.cpp \
    linesplitprocessor.cpp \
    gaps2msfprocessor.cpp \
    nmearecord.cpp \
    forwardmanager.cpp \
    dataprocessor.cpp \
    udpforwarder.cpp \
    main.cpp
OBJECTS_DIR = ../obj
DESTDIR = ../bin
MOC_DIR = moc
unix { 
    target.path = /usr/local/bin
    INSTALLS += target
    GITHASH = $$system(git log -1 --pretty=format:"%h")
    GITTAG = $$system(git describe --abbrev=0 --tags)
    GITDATE = $$system(git log -1 --date=iso --pretty=format:"%cd")
    DEFINES += GITHASH=$$GITHASH \
        GITTAG=$$GITTAG \
        GITDATE="\'$$GITDATE\'"
        
}
#unix { 
#    svnversion.target = svnver
#    svnversion.commands = ../mksvnver
#    QMAKE_EXTRA_TARGETS += svnversion
#    PRE_TARGETDEPS += svnver
#    target.path = /usr/local/bin
#    INSTALLS += target
#    GITTAG = $$system(git describe --abbrev=0 --tags)
#}
