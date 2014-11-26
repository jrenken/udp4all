/*
 *  file:   dataprocessor.cpp
 *  author: jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: dataprocessor.cpp 13 2014-11-26 12:29:48Z jrenken $
 */

#include <QStringList>

#include "dataprocessor.h"

DataProcessor::DataProcessor(const QString& parList)
    : mParameter(parList)
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

