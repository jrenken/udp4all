/*
 *  file:       main.cpp
 *  author:     jrenken
 *
 *  $Rev: 17 $
 *  $Author: jrenken $
 *  $Date: 2016-06-03 10:16:49 +0200 (Fr, 03. Jun 2016) $
 *  $Id: main.cpp 17 2016-06-03 08:16:49Z jrenken $
 */

#ifdef DAEMON
    #include <QCoreApplication>
    #include <QSettings>
    #include <QFile>
    #include <QByteArray>
    #include <signal.h>
    #include <unistd.h>
    #include <iostream>
    #include "forwardmanager.h"
    #include "logger.h"
    #include "reportserver.h"
    #include "svnversion.h"
#else
    #include "mainwin.h"
    #include <QtGui>
    #include <QApplication>
#endif

#define STR(x)   #x
#define XSTR(x)  STR(x)

#ifdef GITTAG
#define __GITTAG__ XSTR(GITTAG)
#else
#define __GITTAG__ "V0.16.0"
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
        ForwardManager *m = ForwardManager::instance();
        if (!m)
            Logger::logMessage(QCoreApplication::tr("Received SIGUSR1: Nothing yet"));
        else
            m->reportForwarders();
    }
}

#endif

int main(int argc, char *argv[])
{
#ifndef DAEMON
    QApplication a(argc, argv);
#else
    bool    daemonize   = true;
    bool    rawReport = false;
    quint16 reportPort = 8181;
    int     opt;
    QString configFile;

    while ((opt = getopt(argc, argv, "dvrc:p:")) != -1) {
        switch (opt) {
        case 'd':
            daemonize = false;
            break;
        case 'c':
            configFile = optarg;
            break;
        case 'p':
            quint16 port;
            bool ok;
            port = QByteArray(optarg).toUInt(&ok);
            if (ok) {
                reportPort = port;
            }
            break;
        case 'r':
            rawReport = true;
            break;
        case 'v':
            std::cout << "udp4alld" << std::endl
                      << "Rev: "SVNVERSION << "  " << __GITTAG__ << std::endl
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
    if (daemonize) {
        if (daemon(0, 0) == -1) {
            exit(EXIT_FAILURE);
        }
        writePidFile();
    }
    if (reportPort != 0) {
        ReportServer *reportServer = new ReportServer(&a);
        if (reportServer->listen(reportPort, !rawReport)) {
            logger->logMessage(QCoreApplication::tr("Started report server on port %1").arg(reportPort));
        }
    }
    logger->logMessage(QCoreApplication::tr("Started udp4all with pid %1").arg(getpid()));
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGUSR1, signalHandler);

#else
    MainWin w;
    w.show();
#endif
    return a.exec();
}
