/*
 * ais2gpsprocessor.h
 *
 *  Created on: 05.05.2016
 *      Author: jrenken
 */

#ifndef AIS2GPSPROCESSOR_H_
#define AIS2GPSPROCESSOR_H_

#include "dataprocessor.h"

class Ais2GpsProcessor: public DataProcessor
{
public:
    Ais2GpsProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "Ais2Gps";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);
    QList<QByteArray> decodePayload(const QByteArray& pl);

private:
    QByteArray  mBuffer;
    QByteArray  mPayload;
    int         mMmsi;
    NmeaRecord  mGGA;
    NmeaRecord  mGLL;
    NmeaRecord  mVTG;
    bool        mSendGLL, mSendGGA, mSendVTG;
};

class BitVector
{
public:
    BitVector(int data = 0, int len = 0);
    void append(int data, int len);
    int size() const;
    QByteArray data() const;
    quint32 toUInt(int from, int len, bool* ok = 0) const;
    double toDouble(int from, int len, bool* ok = 0) const;
private:
    QByteArray  mVector;
};


#endif /* AIS2GPSPROCESSOR_H_ */
