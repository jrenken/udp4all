/*
 *  file:   udpforwarder.cpp
 *  author: jrenken
 *
 *  $Rev: 16 $
 *  $Author: jrenken $
 *  $Date: 2015-10-22 12:49:56 +0200 (Do, 22. Okt 2015) $
 *  $Id: udpforwarder.cpp 16 2015-10-22 10:49:56Z jrenken $
 */

#include <QTimer>
#include <QDebug>
#include "udpforwarder.h"


UdpForwarder::UdpForwarder(const QString& name, QObject *parent)
    : QObject(parent),
      mRecCount(0),
      mSendCount(0),
      mMonitor(false),
      mDelay(0),
      mProcessor(0)
{
    setObjectName(name);
    connect(&mSocket, SIGNAL(readyRead()), SLOT(readPendingDatagrams()));
}

UdpForwarder::~UdpForwarder()
{
    if ( bound() )
        mSocket.disconnectFromHost();
    if (mProcessor)
        delete mProcessor;
    emit newMessage(tr("Forwarder %1:delete myself").arg(objectName()));
}


QString UdpForwarder::source() const
{
    if (mSource.first.isNull())
        return "none";
    return QString("%1:%2").arg(mSource.first.toString()).arg(mSource.second);
}

QString UdpForwarder::targets() const
{
    if (mTargets.isEmpty())
        return "none";
    QString s;
    QPair<QHostAddress, quint16> target;

    foreach (target, mTargets) {
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

bool UdpForwarder::bound() const
{
    if ( source() == "none" )
        return false;
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

QString UdpForwarder::report() const
{
    QString rep = QString("%1; Inputs: %2; Source: %3; Processor: %4 %5; "
                          "Targets: %6; Delay: %7; Received: %8; Send: %9")
                           .arg(objectName())
                           .arg(inputs())
                           .arg(source())
                           .arg(processor())
                           .arg(processorParameter())
                           .arg(targets())
                           .arg(mDelay)
                           .arg(mRecCount)
                           .arg(mSendCount);
    return rep;
}
