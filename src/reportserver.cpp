/*
 * reportserver.cpp
 *
 *  Created on: 09.05.2016
 *      Author: jrenken
 */

#include <QTcpSocket>
#include "forwardmanager.h"
#include "reportserver.h"

ReportServer::ReportServer(QObject* parent)
    : QObject(parent),
      mServer(new QTcpServer(this))
{
    mServer->setMaxPendingConnections(1);
}

bool ReportServer::listen(quint16 port)
{
    if (mServer->listen(QHostAddress::LocalHost, port)) {
        connect(mServer, SIGNAL(newConnection()), this, SLOT(sendReport()));
        return true;
    }
    return false;
}

void ReportServer::sendReport()
{
    QTcpSocket *clientConnection = mServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    clientConnection->write(ForwardManager::instance()->report().toLatin1());
    clientConnection->disconnectFromHost();

}
