/*
 *  file:       main.cpp
 *  author:     jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: main.cpp 13 2014-11-26 12:29:48Z jrenken $
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
