/*
 *  file:	dataprocessor.h
 *  author:	jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#ifndef DATAPROCESSOR_H_
#define DATAPROCESSOR_H_

#include <QByteArray>
#include <QString>
#include <QList>
#include "nmearecord.h"

class DataProcessor
{
public:
	DataProcessor(const QString& parList = QString() );
	virtual ~DataProcessor();
	virtual QString	typeName() const {
		return "Raw";
	}
	virtual QList<QByteArray> processData(const QByteArray& data);
};


class Gaps2MsfProcessor : public DataProcessor
{
public:
    Gaps2MsfProcessor(const QString& parList = QString());
	virtual QString	typeName() const {
		return "Gaps2Msf";
	}
	virtual QList<QByteArray> processData(const QByteArray& data);
private:
	NmeaRecord  mMsf;
};

class LineSplitProcessor : public DataProcessor
{
public:
	LineSplitProcessor(const QString& parList = QString());
	virtual QString	typeName() const {
		return "LineSplit";
	}
	virtual QList<QByteArray> processData(const QByteArray& data);
};

class Gaps2GpsProcessor : public DataProcessor
{
public:
	Gaps2GpsProcessor(const QString& parList = QString());
	virtual QString typeName() const {
		return "Gaps2Gps";
	}
	virtual QList<QByteArray> processData(const QByteArray& data);
private:
	NmeaRecord	mGGA;
	NmeaRecord	mGLL;
	int			mBeaconId;
	bool		mSendGLL, mSendGGA;
};

#endif /* DATAPROCESSOR_H_ */
