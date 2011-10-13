/*
 *  file:	udpforwarder.h
 *  author:	jrenken
 *
 *  $Rev:$
 *  $Author:$
 *  $Date:$
 *  $Id:$
 */

#ifndef UDPFORWARDER_H_
#define UDPFORWARDER_H_

#include <QObject>
#include <QList>
#include <QPair>
#include <QHostAddress>
#include <QUdpSocket>
#include "dataprocessor.h"


class UdpForwarder: public QObject
{
	Q_OBJECT

public:
	UdpForwarder(const QString& name = "UdpForwarder", QObject *parent = 0);
	virtual ~UdpForwarder();

	void setDataProcessor(DataProcessor* proc);
	DataProcessor* dataProcessor() const {
		return mProcessor;
	}

	void setSource(const QString& addr, quint16 port);
	QString sourceAddress() const {
		return mSource.first.toString();
	}
	quint16 port() const {
		return mSource.second;
	}

	void addTarget(const QString& addr, quint16 port);

public slots:
	void handleData(const QByteArray& data);
	bool bindSocket();
	void releaseSocket();

signals:
	void newData(const QByteArray& data);
	void newMessage(const QString& msg);

private slots:
	void readPendingDatagrams();

private:
    QUdpSocket			mSocket;
    int					mRecCount;
    int					mSendCount;

    QPair<QHostAddress, quint16>			mSource;
    QList< QPair<QHostAddress, quint16> >	mTargets;
    DataProcessor*							mProcessor;
};

#endif /* UDPFORWARDER_H_ */
