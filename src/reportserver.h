/*
 * reportserver.h
 *
 *  Created on: 09.05.2016
 *      Author: jrenken
 */

#ifndef SRC_REPORTSERVER_H_
#define SRC_REPORTSERVER_H_


#include <QObject>
#include <QTcpServer>

class ReportServer : public QObject
{
    Q_OBJECT

public:
    ReportServer(QObject* parent = 0);

    bool listen(quint16 port);

private:
    QTcpServer*  mServer;

private slots:
    void sendReport();
};

#endif /* SRC_REPORTSERVER_H_ */
