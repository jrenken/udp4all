/*
 * gaps2gpsprocessor.h
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#ifndef GAPS2GPSPROCESSOR_H_
#define GAPS2GPSPROCESSOR_H_

#include "dataprocessor.h"
#include <QMap>

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
    NmeaRecord  mRMC;
    int         mBeaconId;
    bool        mSendGLL, mSendGGA, mSendRMC;
    QMap<int, QString>  mTalker;

    void getParameter(const QString& parList);

};


#endif /* GAPS2GPSPROCESSOR_H_ */
