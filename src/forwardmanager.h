/*
 *  file:   forwardmanager.h
 *  author: jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#ifndef FORWARDMANAGER_H_
#define FORWARDMANAGER_H_

#include <QObject>
#include <QStringList>
#include <QMap>
#include "udpforwarder.h"
#include "dataprocessor.h"
#include "forwardermodel.h"

class QSettings;

class ForwardManager: public QObject
{
    Q_OBJECT
public:
    ForwardManager(QObject *parent = 0);

    bool loadConfiguration(const QString& fileName = QString());
    bool loadConfiguration(QSettings& settings);

    QStringList forwarders() const {
        return mForwarders.keys();
    }

    UdpForwarder *forwarder(const QString& name) const {
        return mForwarders.value(name);
    }

    void setMonitor(const QString& forw, bool mon);
    void setMonitor(bool mon);
    bool monitor(const QString& forw) const;

    QAbstractItemModel* model() const {
        return mForwarderModel;
    }

    void updateForwarder( const QHash<QString, QVariant>& settings );
    void deleteForwarder( const QString& name);

public slots:
    void bindAll();
    void releaseAll();

signals:
    void newMessage(const QString& msg);
    void newRecMonitorData(const QByteArray& data);
    void newSendMonitorData(const QByteArray& data);

private:
    QMap<QString, UdpForwarder*>    mForwarders;
    ForwarderModel*                 mForwarderModel;
    void createForwarders(QSettings& settings);
    void connectForwarders(QSettings& settings);
    void bindForwarders();
    UdpForwarder* createForwarder(const QHash<QString, QVariant>& settings);
    void connectForwarder(const QHash<QString, QVariant>& settings);

    DataProcessor* createDataProcessor(const QString& type, const QString& par = QString());
};

#endif /* FORWARDMANAGER_H_ */
