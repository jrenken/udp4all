/*
 *  file:	forwardmanager.cpp
 *  author:	jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#include <QSettings>
#include <QDebug>
#include "forwardmanager.h"

ForwardManager::ForwardManager(QObject *parent)
	: QObject(parent)
{
	setObjectName("ForwardManager");
}


bool ForwardManager::loadConfiguration(const QString& fileName)
{
	if (!fileName.isEmpty()) {
		QSettings settings(fileName, QSettings::IniFormat);
		if (settings.status() == QSettings::NoError)
			return loadConfiguration(settings);
	}
	return false;
}

bool ForwardManager::loadConfiguration(QSettings& settings)
{
	if (settings.value("Type").toString() != "Udp4All") {
		emit newMessage(tr("ForwardManager:loadConfiguration:failed:invalid config file"));
		return false;
	}
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
		QString s = settings.value("Name").toString();
		UdpForwarder *forwarder = new UdpForwarder(s, this);
		connect(forwarder, SIGNAL(newMessage(const QString&)), SIGNAL(newMessage(const QString&)));
		emit newMessage(tr("ForwardManager:createForwarder:%1").arg(forwarder->objectName()));

		s = settings.value("Source").toString();
		if (!s.isEmpty()) {
			forwarder->setSource(s.section(':', 0, 0), s.section(':', -1).toInt());
		}

		forwarder->setDataProcessor(createDataProcessor(settings.value("Processor").toString()));

		QVariantList vl = settings.value("Targets").toList();
		if (vl.isEmpty()) {
			QVariant v = settings.value("Targets");
			if (!v.isNull())
				vl.append(v);
		}
		foreach (QVariant v, vl) {
			s = v.toString();
			forwarder->addTarget(s.section(':', 0, 0), s.section(':', -1).toInt());
		}
		mForwarders.insert(forwarder->objectName(), forwarder);
	}
	settings.endArray();
}


void ForwardManager::connectForwarders(QSettings& settings)
{
	int size = settings.beginReadArray("Forwarders");
	for ( int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
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
	settings.endArray();
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

DataProcessor* ForwardManager::createDataProcessor(const QString& type)
{
	if (type == "Gaps2Msf") {
		return new Gaps2MsfProcessor();
	}
	return 0;
}

void ForwardManager::setMonitor(const QString& forw, bool mon)
{
	UdpForwarder *forwarder = mForwarders.value(forw);
	if (forwarder) {
		if (mon) {
			connect(forwarder, SIGNAL(newRecMonitorData(const QByteArray&)),
					SIGNAL(newRecMonitorData(const QByteArray&)));
			connect(forwarder, SIGNAL(newSendMonitorData(const QByteArray&)),
					SIGNAL(newSendMonitorData(const QByteArray&)));
		} else {
			disconnect(forwarder, SIGNAL(newRecMonitorData(const QByteArray&)), 0, 0);
			disconnect(forwarder, SIGNAL(newSendMonitorData(const QByteArray&)), 0, 0);

		}
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
