/*
 *  file:   forwardmanager.cpp
 *  author: jrenken
 *
 *  $Rev: 14 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 14:59:56 +0100 (Mi, 26. Nov 2014) $
 *  $Id: forwardmanager.cpp 14 2014-11-26 13:59:56Z jrenken $
 */

#include <QSettings>
#include <QDebug>
#include "forwardmanager.h"
#include "linesplitprocessor.h"
#include "gaps2gpsprocessor.h"
#include "gaps2msfprocessor.h"
#include "gaps2praveprocessor.h"

ForwardManager::ForwardManager(QObject *parent)
    : QObject(parent)
{
    setObjectName("ForwardManager");
    mForwarderModel = new ForwarderModel(&mForwarders, this);
}


bool ForwardManager::loadConfiguration(const QString& fileName)
{
    if (!fileName.isEmpty()) {
        QSettings settings(fileName, QSettings::IniFormat);
        if (settings.value("Type").toString() != "Udp4All") {
            emit newMessage(tr("ForwardManager:loadConfiguration:failed:invalid config file"));
            return false;
        }

        if (settings.status() == QSettings::NoError)
            return loadConfiguration(settings);
    }
    return false;
}

bool ForwardManager::loadConfiguration(QSettings& settings)
{
    emit newMessage(tr("ForwardManager:loadConfiguration:%1").arg(settings.fileName()));
    createForwarders(settings);
    connectForwarders(settings);
    return true;
}

void ForwardManager::createForwarders(QSettings& settings)
{
    int size = settings.beginReadArray("Forwarders");
    for ( int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QHash<QString, QVariant> setngs;
        foreach ( QString key, settings.childKeys()) {
            setngs.insert(key, settings.value(key));
        }
        UdpForwarder *forwarder = createForwarder(setngs);
        mForwarders.insert(forwarder->objectName(), forwarder);
    }
    settings.endArray();
}

UdpForwarder* ForwardManager::createForwarder(const QHash<QString, QVariant>& settings)
{
    QString s = settings.value("Name").toString();
    UdpForwarder *forwarder = new UdpForwarder(s, this);
    connect(forwarder, SIGNAL(newMessage(const QString&)), SIGNAL(newMessage(const QString&)));
    emit newMessage(tr("ForwardManager:createForwarder:%1").arg(forwarder->objectName()));

    connect(forwarder, SIGNAL(newRecMonitorData(const QByteArray&)),
            SIGNAL(newRecMonitorData(const QByteArray&)));
    connect(forwarder, SIGNAL(newSendMonitorData(const QByteArray&)),
            SIGNAL(newSendMonitorData(const QByteArray&)));

    s = settings.value("Source").toString();
    if (!s.isEmpty() || (s != "none") ) {
        forwarder->setSource(s.section(':', 0, 0), s.section(':', -1).toInt());
    }

    QStringList sl = settings.value("Inputs").toString().split(QRegExp("\\,?\\s+"), QString::SkipEmptyParts);
    foreach (QString s, sl) {
        forwarder->addInput(s);
    }

    forwarder->setDataProcessor(
            createDataProcessor(settings.value("Processor").toString(),
                    settings.value("Processor.Parameter").toString()));

    sl = settings.value("Targets").toString().split(QRegExp("\\,?\\s+"), QString::SkipEmptyParts);
    foreach (QString s, sl) {
        forwarder->addTarget(s.section(':', 0, 0), s.section(':', -1).toInt());
    }
    return forwarder;
}

void ForwardManager::connectForwarders(QSettings& settings)
{
    int size = settings.beginReadArray("Forwarders");
    for ( int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QHash<QString, QVariant> setngs;
        foreach ( QString key, settings.childKeys()) {
            setngs.insert(key, settings.value(key));
        }
        connectForwarder(setngs);
    }
    settings.endArray();
}

void ForwardManager::connectForwarder(const QHash<QString, QVariant>& settings)
{
    QString s = settings.value("Name").toString();
    UdpForwarder* forwarder = mForwarders.value(s);
    if (forwarder) {
        QVariantList vl = settings.value("Inputs").toList();
        if (vl.isEmpty())
            vl.append(settings.value("Inputs"));
        foreach (QVariant v, vl) {
            s = v.toString();
            UdpForwarder* sforwarder = mForwarders.value(s);
            if (sforwarder && (sforwarder != forwarder)) {
                connect(sforwarder, SIGNAL(newData(const QByteArray&)),
                        forwarder, SLOT(handleData(const QByteArray&)));
                emit newMessage(tr("ForwardManager:connectForwarder:%1 -> %2")
                        .arg(sforwarder->objectName()).arg(forwarder->objectName()));
            }
        }
    }
}

void ForwardManager::bindAll()
{
    foreach (UdpForwarder* fw, mForwarders.values()) {
        fw->bindSocket();
    }
}

void ForwardManager::releaseAll()
{
    foreach (UdpForwarder* fw, mForwarders.values()) {
        fw->releaseSocket();
    }
}

DataProcessor* ForwardManager::createDataProcessor(const QString& type, const QString& par)
{
    if (type == "Gaps2Msf") {
        return new Gaps2MsfProcessor(par);
    } else if (type == "LineSplit") {
        return new LineSplitProcessor(par);
    } else if (type == "Gaps2Gps") {
        return new Gaps2GpsProcessor(par);
    } else if (type == "Gaps2Prave") {
        return new Gaps2PraveProcessor(par);
    }
    return 0;
}

void ForwardManager::setMonitor(bool mon)
{
    foreach (UdpForwarder* fw, mForwarders.values()) {
        fw->setMonitor(mon);
    }
}

void ForwardManager::setMonitor(const QString& forw, bool mon)
{
    UdpForwarder *forwarder = mForwarders.value(forw);
    if (forwarder) {
        forwarder->setMonitor(mon);
    }
}

bool ForwardManager::monitor(const QString& forw) const
{
    UdpForwarder *forwarder = mForwarders.value(forw);
    if (forwarder)
        return forwarder->monitor();
    return false;
}

void ForwardManager::updateForwarder( const QHash<QString, QVariant>& settings, QSettings &globalSettings )
{
    UdpForwarder *fw;
    QString s = settings.value("Name").toString();
    if (mForwarders.contains(s)) {
        fw = mForwarders.value(s);
        mForwarders.remove(s);
        delete fw;
    }
    fw = createForwarder(settings);
    mForwarders.insert(fw->objectName(), fw);
    saveConfiguration(globalSettings);
    connectForwarders(globalSettings);
    fw->bindSocket();
    mForwarderModel->updateData();
}


void ForwardManager::deleteForwarder( const QString& name, QSettings &globalSettings)
{
    if (mForwarders.contains(name)) {
        UdpForwarder *fw = mForwarders.value(name);
        mForwarders.remove(name);
        delete fw;
        saveConfiguration(globalSettings);
        mForwarderModel->updateData();
    }
}

void ForwardManager::saveConfiguration(QSettings& settings)
{
    settings.beginWriteArray("Forwarders");
    int idx = 0;
    foreach (QString key, mForwarders.keys()) {
        settings.setArrayIndex(idx);
        QHash<QString, QVariant> fwSettings = mForwarders.value(key)->settings();
        foreach (QString sKey, fwSettings.keys()) {
            settings.setValue(sKey, fwSettings.value(sKey));
        }
        idx++;
    }
    settings.endArray();
}
