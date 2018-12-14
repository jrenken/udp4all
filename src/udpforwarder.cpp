/*
 *  file:   udpforwarder.cpp
 *  author: jrenken
 *
 *  $Rev: 17 $
 *  $Author: jrenken $
 *  $Date: 2016-06-03 10:16:49 +0200 (Fr, 03. Jun 2016) $
 *  $Id: udpforwarder.cpp 17 2016-06-03 08:16:49Z jrenken $
 */

#include <QTimer>
#include "udpforwarder.h"

UdpForwarder::UdpForwarder(const QString& name, QObject *parent)
    : QObject(parent),
      mSourceType(UDP),
      mRecCount(0),
      mSendCount(0),
      mMonitor(false),
      mDelay(0),
      mLastError(QAbstractSocket::UnknownSocketError),
      mProcessor(0)
{
    setObjectName(name);
    connect(&mSocket, SIGNAL(readyRead()), SLOT(readPendingDatagrams()));
    connect(&mTcpSocket, SIGNAL(readyRead()), SLOT(readSocketData()));
    connect(&mTcpSocket, SIGNAL(connected()), SLOT(tcpSocketConnected()));
    connect(&mTcpSocket, SIGNAL(disconnected()), SLOT(tcpSocketDisconnected()));
    connect(&mTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(tcpSocketError( QAbstractSocket::SocketError)));
}

UdpForwarder::~UdpForwarder()
{
    if ( bound() )
        releaseSocket();
    if (mProcessor)
        delete mProcessor;
    emit newMessage(tr("Forwarder %1:delete myself").arg(objectName()));
}


QString UdpForwarder::source() const
{
    if (mSource.first.isNull())
        return "none";
    QString s;
    if (mSourceType == TCP)
        s = "tcp://";
    return s + QString("%1:%2").arg(mSource.first.toString()).arg(mSource.second);
}

QString UdpForwarder::targets() const
{
    if (mTargets.isEmpty())
        return "none";
    QString s;
    QPair<QHostAddress, quint16> target;

    foreach (target, mTargets) {
        if (target.first.isNull())
            return "none";
        s.append(QString("%1:%2 ").arg(target.first.toString()).arg(target.second));
    }
    return s.simplified();
}

QString UdpForwarder::inputs() const
{
    if (mInputs.isEmpty())
        return "none";

    return mInputs.join(" ").simplified();
}

QString UdpForwarder::processor() const
{
    if (mProcessor == 0)
        return "none";

    return mProcessor->typeName();
}

QString UdpForwarder::processorParameter() const
{
    if (mProcessor == 0)
        return "";
    return mProcessor->parameter();
}

void UdpForwarder::setSoureType(SourceType type)
{
    if (mSourceType != type) {
        releaseSocket();
    }
    mSourceType = type;
}

void UdpForwarder::setSource(const QString& addr, quint16 port)
{
    mSource.first.setAddress(addr);
    mSource.second = port;
    if (mSourceType == TCP) {
        if (mTcpSocket.state() == QAbstractSocket::ConnectedState)
            bindSocket();
        emit newMessage(tr("Forwarder %1:setSource:tcp://%2:%3").arg(objectName())
                .arg(mSource.first.toString()).arg(mSource.second));
   } else {
        if (mSocket.state() == QAbstractSocket::BoundState)
            bindSocket();
        emit newMessage(tr("Forwarder %1:setSource:%2:%3").arg(objectName())
                .arg(mSource.first.toString()).arg(mSource.second));
   }
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

int UdpForwarder::delay() const
{
    return mDelay;
}

void UdpForwarder::setDelay(int delay)
{
    mDelay = delay;
}



bool UdpForwarder::bindSocket()
{
    if (mSourceType == TCP) {
        mTcpSocket.disconnectFromHost();
        mTcpSocket.connectToHost(mSource.first, mSource.second, QIODevice::ReadOnly);
    } else {
        mSocket.disconnectFromHost();
        if (!mSocket.bind(mSource.first, mSource.second)) {
            QTimer::singleShot(2000, this, SLOT(bindSocket()));
            return false;
        }
        mSocket.flush();
        emit newMessage(tr("Forwarder %1:bindSocket:%2:%3").arg(objectName())
                .arg(mSource.first.toString()).arg(mSource.second));
    }
    mSendCount = 0;
    mRecCount = 0;
    return true;
}

void UdpForwarder::releaseSocket()
{
    if (mSourceType == TCP) {
        mTcpSocket.disconnectFromHost();
    } else {
        mSocket.disconnectFromHost();
        emit newMessage(tr("Forwarder %1:releaseSocket:%2:%3").arg(objectName())
                .arg(mSource.first.toString()).arg(mSource.second));
    }
}

bool UdpForwarder::bound() const
{
    if ( source() == "none" )
        return false;
    if (mSourceType == TCP)
        return (mTcpSocket.state() == QAbstractSocket::ConnectedState);

    return (mSocket.state() == QAbstractSocket::BoundState);
}

void UdpForwarder::handleData(const QByteArray& data)
{
    QList<QByteArray> procData;

    mRecCount++;
    if (mMonitor) {
        emit newRecMonitorData(data);
    }
    if (mProcessor) {
        procData = mProcessor->processData(data);
    } else {
        procData.append(data);
    }
    if (!procData.isEmpty()) {
        if (mDelay > 0) {
            bool trig = mOutputQueue.isEmpty();
            mOutputQueue.append(procData);
            if (trig)
                sendQueuedData();
        } else {
            foreach (QByteArray ba, procData) {
                sendData(ba);
            }
        }
    }
}

void UdpForwarder::sendData(const QByteArray& ba)
{
    mSendCount++;
    emit newData(ba);
    if (mMonitor) {
        emit newSendMonitorData(ba);
    }
    QPair<QHostAddress, quint16> target;
    foreach (target, mTargets) {
        mSocket.writeDatagram(ba, target.first, target.second);
    }
}

void UdpForwarder::sendQueuedData()
{
    if (mOutputQueue.isEmpty())
        return;

    QByteArray ba = mOutputQueue.dequeue();
    sendData(ba);
    if (!mOutputQueue.isEmpty()) {
        QTimer::singleShot(mDelay, this, SLOT(sendQueuedData()));
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

QHash<QString,QVariant> UdpForwarder::settings() const
{
    QHash<QString, QVariant> settings;
    settings.insert("Name", objectName());
    settings.insert("Inputs", inputs());
    settings.insert("Source", source());
    settings.insert("Delay", mDelay);
    settings.insert("Processor", processor());
    if (mProcessor)
        settings.insert("Processor.Parameter", mProcessor->parameter());
    settings.insert("Targets", targets());
    return settings;
}

QString UdpForwarder::report(bool html) const
{
    QString fmt;
    if (html)
        fmt = "<tr>\n"
                "<td>%1</td>\n"
                "<td>%2</td>\n"
                "<td>%3</td>\n"
                "<td>%4 %5</td>\n"
                "<td>%6</td>\n"
                "<td>%7</td>\n"
                "<td>%8</td>\n"
                "<td>%9</td>\n"
                ;
    else
        fmt = "%1; Source: %2; Inputs: %3; Processor: %4 %5; "
                "Targets: %6; Delay: %7; Received: %8; Send: %9";

    QString rep = fmt.arg(objectName())
                           .arg(source())
                           .arg(inputs())
                           .arg(processor())
                           .arg(processorParameter())
                           .arg(targets())
                           .arg(mDelay)
                           .arg(mRecCount)
                           .arg(mSendCount);
    return rep;
}

void UdpForwarder::readSocketData()
{
    while (mTcpSocket.bytesAvailable()) {
        QByteArray data;
        data = mTcpSocket.read(4096);
        handleData(data);
    }
}

void UdpForwarder::tcpSocketConnected()
{
    mTcpSocket.flush();
    emit newMessage(tr("Forwarder %1:connectSocket:tcp://%2:%3").arg(objectName())
            .arg(mSource.first.toString()).arg(mSource.second));
}

void UdpForwarder::tcpSocketDisconnected()
{
    emit newMessage(tr("Forwarder %1:releaseSocket:tcp://%2:%3").arg(objectName())
            .arg(mSource.first.toString()).arg(mSource.second));
}

void UdpForwarder::tcpSocketError(QAbstractSocket::SocketError error)
{
    if (error != mLastError) {
        mLastError = error;
        emit newMessage(tr("Forwarder %1:tcpSocketError: %2: %3").arg(objectName()).arg(error).arg(mTcpSocket.errorString()));
    }
    QTimer::singleShot(2000, this, SLOT(bindSocket()));
}
