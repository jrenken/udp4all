/*
 *  file:	dataprocessor.cpp
 *  author:	jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#include "dataprocessor.h"
#include <QDebug>
DataProcessor::DataProcessor()
{
}

DataProcessor::~DataProcessor()
{
}

QList<QByteArray> DataProcessor::processData(const QByteArray& data)
{
	QList<QByteArray> list;
	list.append(data);
	return list;
}

QList<QByteArray> Gaps2MsfProcessor::processData(const QByteArray& data)
{
	return DataProcessor::processData(data);
}

QList<QByteArray> LineSplitProcessor::processData(const QByteArray& data)
{
	QList<QByteArray> list;

	int from = 0, to = 0;
	while (from < data.size() && to != -1) {
		to  = data.indexOf('\n', from);
		list.append(data.mid(from, qMax(-1, to - from + 1)));
		from = to + 1;
	}
	return list;
}
