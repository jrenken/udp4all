/*
 *  file:       main.cpp
 *  author:     jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: main.cpp 13 2014-11-26 12:29:48Z jrenken $
 */

#ifdef DAEMON
    #include <QCoreApplication>
    #include <QSettings>
    #include <QFile>
    #include <signal.h>
    #include <unistd.h>
    #include <iostream>
    #include "forwardmanager.h"
    #include "logger.h"
    #include "svnversion.h"
#else
    #include "mainwin.h"
    #include <QtGui>
    #include <QApplication>
#endif

#ifdef DAEMON
void writePidFile()
{
    int pid = getpid();
    QFile   pidRun("/var/run/udp4alld.pid");
    if (pidRun.open(QIODevice::WriteOnly)) {
        QTextStream out(&pidRun);
        out << pid;
        pidRun.close();
    }
}

void signalHandler(int nr)
{
    if (nr == SIGTERM) {
        Logger::logMessage(QCoreApplication::tr("Received SIGTERM: Exiting"));
        QFile::remove("/var/run/udp4alld.pid");
        QCoreApplication::quit();
    } else if (nr == SIGHUP) {
        Logger::logMessage(QCoreApplication::tr("Reload configuration not implemented yet"));
    } else if (nr == SIGUSR1) {
        Logger::logMessage(QCoreApplication::tr("Received SIGUSR1: Nothing yet"));
    }
}

#endif

int main(int argc, char *argv[])
{
#ifndef DAEMON
    QApplication a(argc, argv);
#else
    bool    daemonize   = true;
    int     opt;
    QString configFile;

    while ((opt = getopt(argc, argv, "dvc:")) != -1) {
        switch (opt) {
        case 'd':
            daemonize = false;
            break;
        case 'c':
            configFile = optarg;
            break;
        case 'v':
            std::cout << "udp4alld" << std::endl
                      << "Rev: "SVNVERSION << std::endl
                      << "Date: "SVNDATE << std::endl
                      << "Author: Jens Renken renken@marum.de" << std::endl
                      << "(C) Copyright 2011, University of Bremen, Marum" << std::endl;
            exit(0);
       }
    }
    QCoreApplication a(argc, argv);
#endif
    a.setOrganizationName("Marum");
    a.setApplicationName("udp4all");

#ifdef DAEMON
    Logger  *logger = new Logger(&a);
    QObject::connect(ForwardManager::instance(), SIGNAL(newMessage(const QString&)), logger, SLOT(logMessage(const QString&)));
    QSettings *settings;
    if (!configFile.isEmpty()) {
        settings = new QSettings(configFile, QSettings::IniFormat );
    } else {
        settings = new QSettings();
    }
    ForwardManager::instance()->loadConfiguration(*settings);
    ForwardManager::instance()->bindAll();
    logger->logMessage(QCoreApplication::tr("Started udp4all"));
    if (daemonize) {
        if (daemon(0, 0) == -1) {
            exit(EXIT_FAILURE);
        }
        writePidFile();
    }
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGUSR1, signalHandler);

#else
    MainWin w;
    w.show();
#endif
    return a.exec();
}
