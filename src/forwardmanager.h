/*
 *  file:   forwardmanager.h
 *  author: jrenken
 *
 *  $Rev: 17 $
 *  $Author: jrenken $
 *  $Date: 2016-06-03 10:16:49 +0200 (Fr, 03. Jun 2016) $
 *  $Id: forwardmanager.h 17 2016-06-03 08:16:49Z jrenken $
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
    QString report(bool html = true) const;

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
