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
      mBeaconId(-1),
      mSendGLL(true),
      mSendGGA(true),
      mSendRMC(true)
{
    mGLL.setRecord(QByteArray("$GPGLL,5301.4970,N,00852.1740,E,114015,A,"));
    mGGA.setRecord(QByteArray("$GPGGA,114035.00,5301.4970,N,00852.1740,E,2,,,-0000.0,M,,,,"));
    mRMC.setRecord(QByteArray("$GPRMC,145801.00,A,0709.70959,N,02906.68151,W,,,210318,,,"));
    getParameter(parList);
}

void Gaps2GpsProcessor::getParameter(const QString& parList)
{
    QStringList list = parList.split(' ');
    if (list.size() > 0) {
        bool ok;
        int id = list.at(0).toInt(&ok);
        if (ok && id >= 0 && id < 256)
            mBeaconId = id;
        QStringList mlist = list.at(0).split(',');
        foreach (QString p, mlist) {
            if (p.contains(':')) {
                int bId = p.section(':', 0, 0).toInt(&ok);
                mTalker[bId] = p.section(':', 1, 1);
            }
        }
    }
    if (list.contains("-GGA", Qt::CaseInsensitive))
        mSendGGA = false;
    if (list.contains("-GLL", Qt::CaseInsensitive))
        mSendGLL = false;
    if (list.contains("-RMC", Qt::CaseInsensitive))
        mSendRMC = false;

}

QList<QByteArray> Gaps2GpsProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;

    foreach (QByteArray ba, list) {
       NmeaRecord n(ba);
       if (n.isEmpty()) continue;
       if (n.header() == "$PTSAG") {
           int bId = n[6].toInt();
           if ((bId == mBeaconId) || mTalker.contains(bId)) {
               if (mSendGGA) {
                   mGGA[1] = n[2];
                   mGGA[2] = n[7];
                   mGGA[3] = n[8];
                   mGGA[4] = n[9];
                   mGGA[5] = n[10];
                   if (mBeaconId == -1) {
                       mGGA.setTalker(mTalker.value(bId));
                   }
                   out.append(mGGA.sentence(true));
               }
               if (mSendGLL) {
                   mGLL[5] = n[2];
                   mGLL[1] = n[7];
                   mGLL[2] = n[8];
                   mGLL[3] = n[9];
                   mGLL[4] = n[10];
                   if (mBeaconId == -1) {
                       mGLL.setTalker(mTalker.value(bId));
                   }
                   out.append(mGLL.sentence(true));
               }
               if (mSendRMC) {
                   mRMC[1] = n[2];
                   mRMC[3] = n[7];
                   mRMC[4] = n[8];
                   mRMC[5] = n[9];
                   mRMC[6] = n[10];
                   if (mBeaconId == -1) {
                       mRMC.setTalker(mTalker.value(bId));
                   }
                   out.append(mRMC.sentence(true));
               }
           }
       }
   }
   return out;
}

QString Gaps2GpsProcessor::doc()
{
    return "Gaps2Gps: Convert a $PTSAG sentence with the relevant beacon id into GGA and GLL sentences,\n"
           "          or assign talker id for each defined beacon id\n"
           "    Parameter:  numeric beacon id [0..15]\n"
           "             or id:talker,id:talker,...\n"
           "                -GGA: inhibit GGA output\n"
           "                -GLL: inhibit GLL output\n\n";
}
