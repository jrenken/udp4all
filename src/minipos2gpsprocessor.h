/*
 * minipos2gpsprocessor.h
 *
 *  Created on: Feb 2, 2022
 *      Author: jrenken
 */

#ifndef MINIPOS2GPSPROCESSOR_H_
#define MINIPOS2GPSPROCESSOR_H_

#include <QStringList>
#include "dataprocessor.h"

class MiniPos2GpsProcessor : public DataProcessor
{
public:
    MiniPos2GpsProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "MiniPos2Gps";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);
    static QString doc();

private:
    NmeaRecord  mGGA;
    NmeaRecord  mGLL;
    NmeaRecord  mVTG;
    NmeaRecord  mRMC;
    bool        mSendGLL, mSendGGA, mSendRMC, mSendVTG;
    bool        mUseDepth;

};


#endif /* MINIPOS2GPSPROCESSOR_H_ */
