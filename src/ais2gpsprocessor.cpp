/*
 * ais2gpsprocessor.cpp
 *
 *  Created on: 05.05.2016
 *      Author: jrenken
 */

#include <QBuffer>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include "ais2gpsprocessor.h"

Ais2GpsProcessor::Ais2GpsProcessor(const QString& parList)
    : DataProcessor(parList),
      mMmsi(0),
      mSendGLL(true),
      mSendGGA(true),
      mSendVTG(true),
      mSendMSF(false),
      mMsfBug(false)
{
    bool ok;
    mGLL.setRecord(QByteArray("$GPGLL,5301.4970,N,00852.1740,E,114015,A,"));
    mGGA.setRecord(QByteArray("$GPGGA,114035.00,5301.4970,N,00852.1740,E,2,,,-0000.0,M,,,,"));
    mVTG.setRecord(QByteArray("$GPVTG,360.0,T,,,000.0,N,,"));
    mMSF.setRecord(QByteArray("$MSF,,,SHIP,MyBoat,MSF0,,,0,0,0,0,0,0,0,0,"));
    QStringList list = parList.split(' ');
    if (list.size() > 0) {
        int id = list.at(0).toInt(&ok);
        if (ok)
            mMmsi = id;
    }
    if (list.contains("-GGA", Qt::CaseInsensitive))
        mSendGGA = false;
    if (list.contains("-GLL", Qt::CaseInsensitive))
        mSendGLL = false;
    if (list.contains("-VTG", Qt::CaseInsensitive))
        mSendVTG = false;
    int msf = list.indexOf("+MSF");
    if (msf != -1) {
        msf++;
        mSendMSF = true;

        if (list.size() > msf && !list.at(msf).isEmpty())
            mMSF[3] = list.at(msf).toAscii();
        if (list.size() > (msf + 1) && !list.at(msf + 1).isEmpty())
            mMSF[4] = list.at(msf + 1).toAscii();
        if (list.size() > (msf + 2) && !list.at(msf + 2).isEmpty())
            mMSF[5] = list.at(msf + 2).toAscii();
        if (list.indexOf("mbug") != -1) {
            mMsfBug = true;
        }
    }

}

QList<QByteArray> Ais2GpsProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list;
    mBuffer.append(data);
    QBuffer buffer(&mBuffer);
    buffer.open(QIODevice::ReadOnly);
    while (buffer.canReadLine()) {
        QByteArray ba = buffer.readLine(256);
        NmeaRecord n(ba);
        if (n.isEmpty())
            continue;
        if (n.header() == "!AIVDM") {
            int fcnt = n[1].toInt();
            int frag = n[2].toInt();
            if (frag == 1) {
                mPayload = n[5];
            } else {
                mPayload.append(n[5]);
            }
            if (frag == fcnt) {
                list.append(decodePayload(mPayload));
            }
        }
    }
    mBuffer.remove(0, buffer.pos());
    return list;
}

QList<QByteArray> Ais2GpsProcessor::decodePayload(const QByteArray& pl)
{
    QList<QByteArray> list;

    BitVector vec;
    foreach (char c, pl) {
        int v = c;
        v -= 48;
        if (v > 40)
            v -= 8;
        vec.append(v, 6);
    }
    int offs = 0;
    quint32 type = vec.toUInt(0, 6);
    if (type == 18 || type == 19) {
        offs = -4;
    } else if ((type < 1) || (type > 3))
        return list;

    quint32 mmsi = vec.toUInt(8, 30);
    if (int(mmsi) == mMmsi) {
        double lat = int(vec.toUInt(89 + offs, 27)) / 600000.0;
        double lon = int(vec.toUInt(61 + offs, 28)) / 600000.0;
        quint32 sec = vec.toUInt(137 + offs, 6);
        double cog = vec.toUInt(116 + offs, 12) * 0.1;
        double speed = vec.toUInt(50 + offs, 10) * 0.1;
        QDateTime dt = QDateTime::currentDateTimeUtc();
        dt.setTime(QTime(dt.time().hour(), dt.time().minute(), sec));
        if (mSendGGA) {
            mGGA.setField(1, QString("%1%2%3.00").arg(dt.time().hour(), 2, 10, QLatin1Char('0'))
                    .arg(dt.time().minute(), 2, 10, QLatin1Char('0'))
                    .arg(dt.time().second(), 2, 10, QLatin1Char('0')).toLatin1());
            mGGA.toDDM(lat, 2, NmeaRecord::Lat, 3, 5);
            mGGA.toDDM(lon, 4, NmeaRecord::Lon, 5, 5);

            list.append(mGGA.sentence(true));
        }
        if (mSendGLL) {
            mGLL.setField(5, QString("%1%2%3").arg(dt.time().hour(), 2, 10, QLatin1Char('0'))
                    .arg(dt.time().minute(), 2, 10, QLatin1Char('0'))
                    .arg(dt.time().second(), 2, 10, QLatin1Char('0')).toLatin1());
            mGLL.toDDM(lat, 1, NmeaRecord::Lat, 2, 5);
            mGLL.toDDM(lon, 3, NmeaRecord::Lon, 4, 5);
            list.append(mGLL.sentence(true));
        }
        if (mSendVTG) {
            mVTG.setField(1, QString("%1").arg(cog, 5, 'f', 1, QLatin1Char('0')).toLatin1());
            mVTG.setField(5, QString("%1").arg(speed, 5, 'f', 1, QLatin1Char('0')).toLatin1());
            list.append(mVTG.sentence(true));
        }
        if (mSendMSF) {
            if (mMsfBug) {
                mMSF.setField(1, dt.toString("yyyyMMdd").toLatin1());
            } else {
                mMSF.setField(1, ("2012" + dt.toString("MMdd")).toLatin1());
            }
            mMSF.setField(2, dt.toString("hhmmss").toLatin1());
            mMSF.setField(6, lat);
            mMSF.setField(7, lon);
            mMSF.setField(10, cog, 1);
            list.append(mMSF.sentence(true));
        }
    }
    return list;
}


BitVector::BitVector(int data, int len)
{
    if (len) {
        append(data, len);
    }
}

void BitVector::append(int data, int len)
{
    if (len > 32)
        return;

    QByteArray ba = QString("%1").arg(data, len, 2, QLatin1Char('0')).toLatin1();
    mVector.append(ba);
}

int BitVector::size() const
{
    return mVector.size();
}

QByteArray BitVector::data() const
{
    return mVector;
}

quint32 BitVector::toUInt(int from, int len, bool* ok) const
{
    bool ok1;

    if (len > 32) {
        if (ok) *ok = false;
        return 0;
    }
    QByteArray ba = mVector.mid(from, len);
    quint32 res = ba.toUInt(&ok1, 2);
    if (ok)
        *ok = ok1;
    return res;
}

double BitVector::toDouble(int from, int len, bool* ok) const
{
    bool ok1;

    if (len > 64) {
        if (ok) *ok = false;
        return 0.0;
    }
    QByteArray ba = mVector.mid(from, len);
    if (len > 32) {
        quint64 ul = ba.toULongLong(&ok1, 2);
        if (ok)
            *ok = ok1;
        if (ok1) {
            double res = *(reinterpret_cast<double*>(&ul));
            return res;
        }
    } else {
        quint32 ui = ba.toUInt(&ok1, 2);
        if (ok)
            *ok = ok1;
        if (ok1) {
            float res = *(reinterpret_cast<float*>(&ui));
            return res;
        }
    }
    if (ok)
        *ok = false;
    return 0.0;
}
