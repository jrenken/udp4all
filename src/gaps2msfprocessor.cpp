/*
 * gaps2msfprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include <QStringList>
#include <QtCore/qmath.h>
#include "gaps2msfprocessor.h"

// $MSF,date,time,type,name,source,lat,lon,depth,altitude,heading,roll,pitch,Vx,Vy,Vz,

Gaps2MsfProcessor::Gaps2MsfProcessor(const QString& parList)
    : DataProcessor(parList),
      mBug(false)
{
    QStringList list = parList.split(' ');
    mMsf.setRecord(QByteArray("$MSF,,,SHIP,MyBoat,MSF0,,,0,0,0,0,0,0,0,0,"));
    if (list.size() > 0 && !list.at(0).isEmpty())
        mMsf[3] = list.at(0).toLatin1();
    if (list.size() > 1 && !list.at(1).isEmpty())
        mMsf[4] = list.at(1).toLatin1();
    if (list.size() > 2 && !list.at(2).isEmpty())
        mMsf[5] = list.at(2).toLatin1();
    if (list.indexOf("mbug") != -1) {
        mBug = true;
    }
}

QList<QByteArray> Gaps2MsfProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;
     foreach (QByteArray ba, list) {
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PTSAG") {
            if (n[6].toInt() == 0) {
                if (mBug) {
                    mMsf[1] = "2012" + n[4] + n[3];
                } else {
                    mMsf[1] = n[5] + n[4] + n[3];
                }
                mMsf[2] = n[2];
                qreal l = n[7].toDouble();
                int lD = qFloor(l / 100);
                l = (l - lD * 100) / 60.0 + lD;
                if (n[8].toUpper() == "S")
                    l *= -1;
                mMsf.setField(6, l);
                l = n[9].toDouble();
                lD = qFloor(l / 100);
                l = (l - lD * 100) / 60.0 + lD;
                if (n[10].toUpper() == "W")
                    l *= -1;
                mMsf.setField(7, l);
            }
        } else if (n.header() == "$PTSAH") {
            mMsf[10] = n[2];
            out.append(mMsf.sentence(true));
        } else if (n.header() == "$HEHDT") {
            mMsf[10] = n[1];
            out.append(mMsf.sentence(true));
        }
    }
    return out;
}

