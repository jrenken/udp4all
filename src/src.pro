TEMPLATE = app
TARGET = udp4all
#CONFIG += console
CONFIG += debug_and_release

QT += core \
    widgets \
    gui \
    network
console { 
    QT -= gui widgets
    TARGET = udp4alld
    DEFINES += DAEMON
    HEADERS += reportserver.h \
        logger.h
    SOURCES += reportserver.cpp \
        logger.cpp
    message("Build daemon udp4alld")
}
else { 
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
HEADERS += ais2gpsprocessor.h \
    gaps2praveprocessor.h \
    gaps2gpsprocessor.h \
    minipos2gpsprocessor.h \
    minipos2depthprocessor.h \
    linesplitprocessor.h \
    gaps2msfprocessor.h \
    ranger2gapsprocessor.h \
    nmearecord.h \
    forwardmanager.h \
    dataprocessor.h \
    udpforwarder.h
SOURCES += ais2gpsprocessor.cpp \
    gaps2praveprocessor.cpp \
    gaps2gpsprocessor.cpp \
    minipos2gpsprocessor.cpp \
    minipos2depthprocessor.cpp \
    linesplitprocessor.cpp \
    gaps2msfprocessor.cpp \
    ranger2gapsprocessor.cpp \
    nmearecord.cpp \
    forwardmanager.cpp \
    dataprocessor.cpp \
    udpforwarder.cpp \
    main.cpp
OBJECTS_DIR = ../obj
DESTDIR = ../bin
MOC_DIR = moc
GITHASH = $$system(git log -1 --pretty=format:"%h")
GITTAG = $$system(git describe --abbrev=0 --tags)
GITDATE = $$system(git log -1 --date=iso --pretty=format:"%cd")
DEFINES += GITHASH=$$GITHASH \
    GITTAG=$$GITTAG \
    GITDATE="\'$$GITDATE\'"
unix { 
    target.path = /usr/local/bin
    icon.path = /usr/local/share/icons
    icon.files = images/udp4all.png
    desktop.path = /usr/local/share/applications
    desktop.files = ../etc/udp4all.desktop
    INSTALLS += target icon desktop
}
    
