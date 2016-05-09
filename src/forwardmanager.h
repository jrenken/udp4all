/*
 *  file:   forwardmanager.h
 *  author: jrenken
 *
 *  $Rev: 16 $
 *  $Author: jrenken $
 *  $Date: 2015-10-22 12:49:56 +0200 (Do, 22. Okt 2015) $
 *  $Id: forwardmanager.h 16 2015-10-22 10:49:56Z jrenken $
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

#ifdef DAEMON
    static ForwardManager *instance();
#endif

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

    void reportForwarders();
    QString report() const;

#ifndef DAEMON
    QAbstractItemModel* model() const {
        return mForwarderModel;
    }
#endif

    void updateForwarder( const QHash<QString, QVariant>& settings, QSettings &globalSettings);
    void deleteForwarder( const QString& name, QSettings &globalSettings);


public slots:
    void bindAll();
    void releaseAll();

signals:
    void newMessage(const QString& msg);
    void newRecMonitorData(const QByteArray& data);
    void newSendMonitorData(const QByteArray& data);

private:
    QMap<QString, UdpForwarder*>    mForwarders;
#ifndef DAEMON
    ForwarderModel*                 mForwarderModel;
#else
    static  ForwardManager          *mInstance;
#endif
    void createForwarders(QSettings& settings);
    void connectForwarders(QSettings& settings);
    void bindForwarders();
    UdpForwarder* createForwarder(const QHash<QString, QVariant>& settings);
    void connectForwarder(const QHash<QString, QVariant>& settings);

    DataProcessor* createDataProcessor(const QString& type, const QString& par = QString());
    void saveConfiguration(QSettings& settings);
};

#endif /* FORWARDMANAGER_H_ */
