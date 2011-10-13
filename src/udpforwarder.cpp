/*
 *  file:	udpforwarder.cpp
 *  author:	jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#include <QTimer>
#include <QDebug>
#include "udpforwarder.h"


UdpForwarder::UdpForwarder(const QString& name, QObject *parent)
	: QObject(parent),
	  mRecCount(0),
	  mSendCount(0),
	  mMonitor(false),
	  mProcessor(0)
{
	setObjectName(name);
	connect(&mSocket, SIGNAL(readyRead()), SLOT(readPendingDatagrams()));
}

UdpForwarder::~UdpForwarder()
{
	if (mProcessor)
		delete mProcessor;
	emit newMessage(tr("Forwarder %1:delete myself").arg(objectName()));
}


void UdpForwarder::setSource(const QString& addr, quint16 port)
{
	mSource.first.setAddress(addr);
	mSource.second = port;
	if (mSocket.state() == QAbstractSocket::BoundState)
		bindSocket();
	emit newMessage(tr("Forwarder %1:setSource:%2:%3").arg(objectName())
			.arg(mSource.first.toString()).arg(mSource.second));
}

void UdpForwarder::addTarget(const QString& addr, quint16 port)
{
	QPair<QHostAddress, quint16> target;
	target.first.setAddress(addr);
	target.second = port;
	mTargets.append(target);
	emit newMessage(tr("Forwarder %1:addTarget:%2:%3").arg(objectName())
			.arg(target.first.toString()).arg(target.second));
}

void UdpForwarder::setDataProcessor(DataProcessor* proc)
{
	mProcessor = proc;
	if (mProcessor)
		emit newMessage(tr("Forwarder %1:setDataProcessor:%2").arg(objectName()).arg(mProcessor->typeName()));
	else
		emit newMessage(tr("Forwarder %1:setDataProcessor:Null").arg(objectName()));
}

bool UdpForwarder::bindSocket()
{
	mSocket.disconnectFromHost();
	if (!mSocket.bind(mSource.first, mSource.second)) {
		QTimer::singleShot(2000, this, SLOT(bindSocket()));
		return false;
	}
	mSendCount = 0;
	mRecCount = 0;
	mSocket.flush();
	emit newMessage(tr("Forwarder %1:bindSocket:%2:%3").arg(objectName())
			.arg(mSource.first.toString()).arg(mSource.second));
	return true;
}

void UdpForwarder::releaseSocket()
{
	mSocket.disconnectFromHost();
	emit newMessage(tr("Forwarder %1:releaseSocket:%2:%3").arg(objectName())
			.arg(mSource.first.toString()).arg(mSource.second));
}

void UdpForwarder::handleData(const QByteArray& data)
{
	QByteArray procData;

	mRecCount++;
	if (mMonitor) {
		emit newRecMonitorData(data);
	}
	if (mProcessor) {
		procData = mProcessor->processData(data);
	} else {
		procData = data;
	}
	if (!procData.isEmpty()) {
		mSendCount++;
		emit newData(procData);
		if (mMonitor) {
			emit newSendMonitorData(data);
		}
		QPair<QHostAddress, quint16> target;
		foreach (target, mTargets) {
			mSocket.writeDatagram(procData, target.first, target.second);
		}
	}
}


void UdpForwarder::readPendingDatagrams()
{
	while (mSocket.hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(mSocket.pendingDatagramSize());
		mSocket.readDatagram(datagram.data(), datagram.size());
		handleData(datagram);
	}
}

