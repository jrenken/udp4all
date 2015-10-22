/*
 *  file:   udpforwarder.h
 *  author: jrenken
 *
 *  $Rev: 14 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 14:59:56 +0100 (Mi, 26. Nov 2014) $
 *  $Id: udpforwarder.h 14 2014-11-26 13:59:56Z jrenken $
 */

#ifndef UDPFORWARDER_H_
#define UDPFORWARDER_H_

#include <QObject>
#include <QList>
#include <QQueue>
#include <QPair>
#include <QVariant>
#include <QHash>
#include <QHostAddress>
#include <QUdpSocket>
#include <QStringList>
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
    QString source() const;
    QString targets() const;
    QString inputs() const;
    QString processor() const;

    int     delay() const;
    void    setDelay(int delay);

    bool bound() const;
    void addTarget(const QString& addr, quint16 port);

    void addInput(const QString& input) {
        mInputs.append(input);
    }

    QPair<int, int> statistics() const {
        return qMakePair(mRecCount, mSendCount);
    }

    bool monitor() const {
        return mMonitor;
    }
    void setMonitor(bool mon) {
        mMonitor = mon;
    }

    QHash<QString,QVariant> settings() const;

    void sendData(const QByteArray& ba);

public slots:
    void handleData(const QByteArray& data);
    bool bindSocket();
    void releaseSocket();
    void sendQueuedData();

signals:
    void newData(const QByteArray& data);
    void newMessage(const QString& msg);
    void newRecMonitorData(const QByteArray& data);
    void newSendMonitorData(const QByteArray& data);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket          mSocket;
    int                 mRecCount;
    int                 mSendCount;
    bool                mMonitor;
    int                 mDelay;

    QPair<QHostAddress, quint16>            mSource;
    QList< QPair<QHostAddress, quint16> >   mTargets;
    QStringList                             mInputs;
    DataProcessor*                          mProcessor;

    QQueue<QByteArray>   mOutputQueue;

};

#endif /* UDPFORWARDER_H_ */
