/*
 *  file:   dataprocessor.h
 *  author: jrenken
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
    virtual QString typeName() const {
        return "Raw";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);
};


#endif /* DATAPROCESSOR_H_ */
