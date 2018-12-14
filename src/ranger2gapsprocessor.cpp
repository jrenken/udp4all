/*
 * ranger2gapsprocessor.cpp
 *
 *  Created on: Dec 14, 2018
 *      Author: jrenken
 */

#include <QDate>
#include "ranger2gapsprocessor.h"

/*
 *
 * $PSONLLD,133707.59,Ship 1,A,51.3306281,-0.83499295,0.000,0.08,0.08,0.00,0.01,,,,*1D
 * $PSONLLD,133707.52,2401,A,51.3315406,-0.83563975,999.916,5.33,5.33,0.00,0.62,,,,*2A
 *
 * $PSONALL,Ship 1,CRP,134446.175,650879.00,5688874.16,0.00,180.00,,G,0.00,0.00,,0.109,0.001*51
 * $PSONALL,2401,Transducer,134446.178,650778.88,5688917.56,1000.09,,,G,,,,7.521,0.606*3F
 *
 *
 *
 *  $PTSAG,#2374110,055648.831,05,04,2018,0,2718.19917,N,01747.61373,W,F,0005.8,1,9999.0*39
 *  $PTSAH,0,359.671,0.000*4D
 *  $PTSAX,#2383172,071216.870,05,04,2018,4,-412.45,526.85,F,0631.8,1,9999.0*23
 *  $PTSAY,#2383173,071216.870,05,04,2018,4,-651.98,150.33,F,0631.8,1,9999.0*2E
 *  $PTSAQ,071216.870,05,04,2018,4,06.42,04.90,192.98,05.03*77
 */


Ranger2GapsProcessor::Ranger2GapsProcessor(const QString& parList)
    : DataProcessor(parList),
      mShipsHeading(-9999.9),
      mFrameId(1)
{
    mPTSAG.setRecord(QByteArray("$PTSAG,#2374110,055648.831,05,04,2018,0,2718.19917,N,01747.61373,W,F,0005.8,1,9999.0"));
    mPTSAH.setRecord(QByteArray("$PTSAH,0,359.671,0.000"));
    mBeacons["2401"] = 1;
    mBeacons["Ship 1"] = 0;
    getParameter(parList);
}

QList<QByteArray> Ranger2GapsProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;

    foreach (QByteArray ba, list) {
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PSONLLD") {
            QString sid = n[2];
            int bid = mBeacons.value(sid, -1);
            if (bid == -1)
                continue;
            bool ok;
            double lat = n[4].toDouble(&ok);
            if (!ok) continue;
            double lon = n[5].toDouble(&ok);
            if (!ok) continue;
            mPTSAG.toDDM(lat, 7, NmeaRecord::Lat, 8);
            mPTSAG.toDDM(lon, 9, NmeaRecord::Lon, 10);
            mPTSAG[1] = QString("#%1").arg(++mFrameId).toLatin1();
            mPTSAG[2] = n[1];
            QDate date = QDate::currentDate();
            mPTSAG[3] = QString("%1").arg(date.day(), 2).toLatin1();
            mPTSAG[4] = QString("%1").arg(date.month(), 2).toLatin1();
            mPTSAG.setField(5, date.year());
            mPTSAG.setField(6, bid);
            mPTSAG[12] = n[6];
            out << mPTSAG.sentence(true);
        } else if (n.header() == "$PSONALL") {
            if (mBeacons.value(n[1], -1) != 0)
                continue;
            mPTSAH[2] = n[7];
            out << mPTSAH.sentence(true);
        }
    }
    return out;
}

QString Ranger2GapsProcessor::doc()
{
    return "Ranger2Gaps: Convert $PSONLLD and $PSONALL sentences into Posidonia/Gaps $PTSAG, $PTSAH and $HEHDT sentences\n"
           "    Parameter:  beacon id assignment in the form <ranger id:beacon id>,\n"
           "                e.g \"<ship 1:0>, <2401:1>\"\n\n";
}

void Ranger2GapsProcessor::getParameter(const QString& parList)
{
    QStringList list = parList.split(QRegExp(",\\s+"), QString::SkipEmptyParts);
    if (list.size() > 0) {
        foreach (QString s, list) {
            QStringList l = s.split(QRegExp("[<>:]"), QString::SkipEmptyParts);
            if (l.size() != 2)
                continue;
            bool ok;
            int id = l.at(1).toInt(&ok);
            if (ok) {
                mBeacons[l.at(0)] = id;
            }
        }
    }
}
