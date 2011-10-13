/*
 *  file:	dataprocessor.cpp
 *  author:	jrenken
 *
 *  $Rev:$
 *  $Author:$
 *  $Date:$
 *  $Id:$
 */

#include "dataprocessor.h"

DataProcessor::DataProcessor()
{
}

DataProcessor::~DataProcessor()
{
}

QByteArray DataProcessor::processData(const QByteArray& data)
{
	return data;
}

QByteArray Gaps2MsfProcessor::processData(const QByteArray& data)
{
	return DataProcessor::processData(data);
}
