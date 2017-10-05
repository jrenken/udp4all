/*
 * gaps2gpsprocessor.h
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#ifndef GAPS2GPSPROCESSOR_H_
#define GAPS2GPSPROCESSOR_H_

#include "dataprocessor.h"

class Gaps2GpsProcessor : public DataProcessor
{
public:
    Gaps2GpsProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "Gaps2Gps";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);

    static QString doc();
private:
    NmeaRecord  mGGA;
    NmeaRecord  mGLL;
    int         mBeaconId;
    bool        mSendGLL, mSendGGA;
};


#endif /* GAPS2GPSPROCESSOR_H_ */
