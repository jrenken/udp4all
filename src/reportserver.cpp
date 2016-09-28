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
      mServer(new QTcpServer(this)),
      mHtml(true)
{
    mServer->setMaxPendingConnections(1);
}

bool ReportServer::listen(quint16 port, bool html)
{
    if (mServer->listen(QHostAddress::Any, port)) {
        connect(mServer, SIGNAL(newConnection()), this, SLOT(sendReport()));
        mHtml = html;
        return true;
    }
    return false;
}

void ReportServer::sendReport()
{
    QTcpSocket *clientConnection = mServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    QByteArray ba;
    clientConnection->waitForReadyRead(5);
    ba = clientConnection->readAll();

    clientConnection->write(ForwardManager::instance()->report(mHtml).toLatin1());
    clientConnection->disconnectFromHost();

}
