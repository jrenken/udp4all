/*
 *  file:   dataprocessor.h
 *  author: jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: dataprocessor.h 13 2014-11-26 12:29:48Z jrenken $
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

    QString parameter() const {
        return mParameter;
    }

    static QString doc();
protected:
    QString mParameter;
};


#endif /* DATAPROCESSOR_H_ */
