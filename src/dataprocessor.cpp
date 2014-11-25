/*
 *  file:   dataprocessor.cpp
 *  author: jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
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

