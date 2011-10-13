/*
 *  file:	forwardmanager.h
 *  author:	jrenken
 *
 *  $Rev:$
 *  $Author:$
 *  $Date:$
 *  $Id:$
 */

#ifndef FORWARDMANAGER_H_
#define FORWARDMANAGER_H_

#include <QObject>
#include <QStringList>
#include <QMap>
#include "udpforwarder.h"
#include "dataprocessor.h"

class QSettings;

class ForwardManager: public QObject
{
	Q_OBJECT
public:
	ForwardManager(QObject *parent = 0);

	bool loadConfiguration(const QString& fileName = QString());
	bool loadConfiguration(QSettings& settings);

	QStringList	forwarders() const {
		return mForwarders.keys();
	}

public slots:
	void bindAll();
	void releaseAll();

signals:
	void newMessage(const QString& msg);

private:
	QMap<QString, UdpForwarder*>	mForwarders;

	void createForwarders(QSettings& settings);
	void connectForwarders(QSettings& settings);
	void bindForwarders();
	DataProcessor* createDataProcessor(const QString& type);
};

#endif /* FORWARDMANAGER_H_ */
