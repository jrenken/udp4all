/*
 *  file:   udpforwarder.h
 *  author: jrenken
 *
 *  $Rev: 17 $
 *  $Author: jrenken $
 *  $Date: 2016-06-03 10:16:49 +0200 (Fr, 03. Jun 2016) $
 *  $Id: udpforwarder.h 17 2016-06-03 08:16:49Z jrenken $
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
    QString processorParameter() const;

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

    QString report(bool html = true) const;

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
