/*
 *  file:		main.cpp
 *  author:		jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#include "mainwin.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Marum");
    a.setApplicationName("udp4all");

    MainWin w;
    w.show();
    return a.exec();
}
