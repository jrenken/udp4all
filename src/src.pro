TEMPLATE = app
TARGET = udp4all
QT += core \
    gui \
    network
HEADERS += forwardmanager.h \
    dataprocessor.h \
    udpforwarder.h \
    mainwin.h \
    svnversion.h
SOURCES += forwardmanager.cpp \
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
svnversion.target = svnver
svnversion.commands = ../mksvnver
QMAKE_EXTRA_TARGETS += svnversion
PRE_TARGETDEPS += svnver

target.path = /usr/local/bin
INSTALLS += target
