/*
 * minipos2gpsprocessor.cpp
 *
 *  Created on: Feb 2, 2022
 *      Author: jrenken
 */

#include <QDate>
#include <minipos2gpsprocessor.h>

MiniPos2GpsProcessor::MiniPos2GpsProcessor(const QString& parList)
    : DataProcessor(parList),
      mSendGLL(true),
      mSendGGA(true),
      mSendRMC(true),
      mSendVTG(true),
      mUseDepth(false)
{
    mGLL.setRecord(QByteArray("$GPGLL,5301.4970,N,00852.1740,E,114015,A,"));
    mGGA.setRecord(QByteArray("$GPGGA,114035.00,5301.4970,N,00852.1740,E,2,8,,-0000.0,M,,,,"));
    mRMC.setRecord(QByteArray("$GPRMC,145801.00,A,0709.70959,N,02906.68151,W,0.0,0.0,210318,,"));
    mVTG.setRecord(QByteArray("$GPVTG,360.0,T,,M,000.0,N,0.0,K"));
    QStringList list = parList.split(' ');
    if (list.contains("-GGA", Qt::CaseInsensitive))
        mSendGGA = false;
    if (list.contains("-GLL", Qt::CaseInsensitive))
        mSendGLL = false;
    if (list.contains("-RMC", Qt::CaseInsensitive))
        mSendRMC = false;
    if (list.contains("-VTG", Qt::CaseInsensitive))
        mSendVTG = false;
    if (list.contains("depth"))
        mUseDepth = true;

}

/*
 * $PSAAS,101301.06,5832.74,N,01458.52,E,176.3,206.3,4.8,1.20,-1.00,-0.30'
 * $PSAAS,<time>,<latitude>,<hemisphere>,<longitude>,<direction>,<depth>,<altitude>,<heading><velforw>,<velport>,<velup>*hh<CR><LF>
 *
 */
QList<QByteArray> MiniPos2GpsProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;


    foreach (QByteArray ba, list) {
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PSAAS") {
            if (mSendGGA) {
                mGGA[1] = n[1];
                mGGA[2] = n[2];
                mGGA[3] = n[3];
                mGGA[4] = n[4];
                mGGA[5] = n[5];
                if (mUseDepth) {
                    mGGA.setField(9, n.toDouble(6) * -1.0, 1);
                } else {
                    mGGA[9] = n[7];
                }
                out.append(mGGA.sentence(true));
            }
            if (mSendGLL) {
                mGLL[1] = n[2];
                mGLL[2] = n[3];
                mGLL[3] = n[4];
                mGLL[4] = n[5];
                mGLL[5] = n[1];
                out.append(mGLL.sentence(true));
            }
            if (mSendRMC) {
                mRMC[1] = n[1];
                mRMC[3] = n[2];
                mRMC[4] = n[3];
                mRMC[5] = n[4];
                mRMC[6] = n[5];
                mRMC.setField(7, n.toDouble(9) * 1.943844, 1);
                mRMC[8] = n[8];
                mRMC[9] = QDate::currentDate().toString("ddMMyy").toLatin1();

                out.append(mRMC.sentence(true));
            }
            if (mSendVTG) {
                mVTG[1] = n[8];
                mVTG.setField(7, n.toDouble(9) * 1.943844, 2);
                mVTG.setField(9, n.toDouble(9) * 3.6, 2);
                out.append(mVTG.sentence(true));
            }
        }
    }
    return out;
}

QString MiniPos2GpsProcessor::doc()
{
    return "MiniPos2Gps: Convert a $PSAAS sentence to RMC, GGA, GLL and VTG sentences\n"
           "    Parameter:  -GGA: inhibit GGA output.\n"
           "                -GLL: inhibit GLL output.\n"
           "                -RMC: inhibit RMC output.\n"
           "                depth: use inverted depth for altitude field, otherwise use altitude.\n\n";
}

