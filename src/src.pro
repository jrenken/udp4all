TEMPLATE = app
TARGET = udp4all 

QT        += core gui network 

HEADERS   += mainwin.h
SOURCES   += main.cpp \
    mainwin.cpp
FORMS     += mainwin.ui    
RESOURCES = udp4all.qrc
RC_FILE = udp4all.rc
OBJECTS_DIR = ../obj
DESTDIR = ../bin
MOC_DIR = moc
