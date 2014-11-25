TEMPLATE = app
TARGET = udp4all
QT += core \
    gui \
    network
HEADERS += gaps2praveprocessor.h \
    gaps2gpsprocessor.h \
    linesplitprocessor.h \
    gaps2msfprocessor.h \
    nmearecord.h \
    forwardermodel.h \
    forwardmanager.h \
    dataprocessor.h \
    udpforwarder.h \
    mainwin.h
SOURCES += gaps2praveprocessor.cpp \
    gaps2gpsprocessor.cpp \
    linesplitprocessor.cpp \
    gaps2msfprocessor.cpp \
    nmearecord.cpp \
    forwardermodel.cpp \
    forwardmanager.cpp \
    dataprocessor.cpp \
    udpforwarder.cpp \
    main.cpp \
    mainwin.cpp
FORMS += mainwin.ui
RESOURCES = udp4all.qrc
RC_FILE = udp4all.rc
OBJECTS_DIR = ../obj
DESTDIR = ../bin
MOC_DIR = moc
unix { 
    svnversion.target = svnver
    svnversion.commands = ../mksvnver
    QMAKE_EXTRA_TARGETS += svnversion
    PRE_TARGETDEPS += svnver
    target.path = /usr/local/bin
    INSTALLS += target
}
