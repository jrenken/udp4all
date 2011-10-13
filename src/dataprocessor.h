/*
 *  file:	dataprocessor.h
 *  author:	jrenken
 *
 *  $Rev:$
 *  $Author:$
 *  $Date:$
 *  $Id:$
 */

#ifndef DATAPROCESSOR_H_
#define DATAPROCESSOR_H_

#include <QByteArray>
#include <QString>

class DataProcessor
{
public:
	DataProcessor();
	virtual ~DataProcessor();
	virtual QString	typeName() const {
		return "Raw";
	}
	virtual QByteArray processData(const QByteArray& data);
};


class Gaps2MsfProcessor : public DataProcessor
{
public:
	virtual QString	typeName() const {
		return "Gaps2Msf";
	}
	virtual QByteArray processData(const QByteArray& data);
};

#endif /* DATAPROCESSOR_H_ */
