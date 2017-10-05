/*
 * gaps2gpsprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include <QStringList>
#include "gaps2gpsprocessor.h"

Gaps2GpsProcessor::Gaps2GpsProcessor(const QString& parList)
    : DataProcessor(parList),
      mBeaconId(0),
      mSendGLL(true),
      mSendGGA(true)
{
    mGLL.setRecord(QByteArray("$GPGLL,5301.4970,N,00852.1740,E,114015,A,"));
    mGGA.setRecord(QByteArray("$GPGGA,114035.00,5301.4970,N,00852.1740,E,2,,,-0000.0,M,,,,"));

    QStringList list = parList.split(' ');
    if (list.size() > 0) {
        int id = list.at(0).toInt();
        if (id >= 0 && id < 16)
            mBeaconId = id;
    }
    if (list.contains("-GGA", Qt::CaseInsensitive))
        mSendGGA = false;
    if (list.contains("-GLL", Qt::CaseInsensitive))
        mSendGLL = false;

}

QList<QByteArray> Gaps2GpsProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;

    foreach (QByteArray ba, list) {
       NmeaRecord n(ba);
       if (n.isEmpty()) continue;
       if (n.header() == "$PTSAG") {
           if (n[6].toInt() == mBeaconId) {
               if (mSendGGA) {
                   mGGA[1] = n[2];
                   mGGA[2] = n[7];
                   mGGA[3] = n[8];
                   mGGA[4] = n[9];
                   mGGA[5] = n[10];
                   out.append(mGGA.sentence(true));
               }
               if (mSendGLL) {
                   mGLL[5] = n[2];
                   mGLL[1] = n[7];
                   mGLL[2] = n[8];
                   mGLL[3] = n[9];
                   mGLL[4] = n[10];
                   out.append(mGLL.sentence(true));
               }
           }
       }
   }
   return out;
}

QString Gaps2GpsProcessor::doc()
{
    return "Gaps2Gps: Convert a $PTSAG sentence with the relevant beacon id into GGA and GLL sentences.\n"
           "    Parameter:  numeric beacon id [0..15]\n"
           "                -GGA: inhibit GGA output\n"
           "                -GLL: inhibit GLL output\n\n";
}
