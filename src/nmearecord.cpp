/*
 *  file:   nmearecord.cpp
 *  author: jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: nmearecord.cpp 13 2014-11-26 12:29:48Z jrenken $
 */

#include <QString>
#include <math.h>
#include "nmearecord.h"

const int NmeaRecord::MaxFields = 64;

NmeaRecord::NmeaRecord(int size)
    : mPrecision(6)
{
    setSize(size);
}

NmeaRecord::NmeaRecord(const char* rec)
    : mPrecision(6)
{
    setRecord(QByteArray(rec));
}

NmeaRecord::NmeaRecord(const QByteArray& rec)
    : mPrecision(6)
{
    setRecord(rec);
}

NmeaRecord::NmeaRecord(const QString& rec)
    : mPrecision(6)
{
    setRecord(rec);
}

QByteArray NmeaRecord::sentence(bool cs)
{
    QByteArray sentence;
    foreach (QByteArray ba, mFields) {
        sentence.append(ba);
        sentence.append(',');
    }
    sentence.chop(1);   // remove last ,
    if (cs) {
        sentence.append(checkSum(sentence));
    }
    return sentence.append("\r\n");
}


QByteArray NmeaRecord::checkSum(const QByteArray &ba)
{
    unsigned int cs = 0;
    for (int i = 1; i < ba.size(); i++) {
        if (ba.at(i) == '*')
            break;
        cs ^= ba.at(i);
    }
    return QString("*%1").arg(cs & 0x00FF, 2, 16, QLatin1Char('0')).toUpper().toAscii();
}

int NmeaRecord::setRecord(const QByteArray& rec)
{
    QByteArray ba = rec.trimmed();
    mValid = true;
    if ((ba.size() > 3) && (ba.at(ba.size() - 3) == '*')) {
        if (checkSum(ba) != ba.right(3))
            mValid = false;
        ba.chop(3);
    }
    mFields.clear();
    mFields.append(ba.split(','));
    return mFields.size();
}

int NmeaRecord::setRecord(const QString& rec)
{
    return setRecord(rec.toAscii());
}


QByteArray& NmeaRecord::field(int i)
{
    int idx = qBound(0, i, MaxFields - 1);
    if (idx >= mFields.size()) {
        setSize(idx);
        mFields.append(QByteArray());
    }
    return mFields[idx];
}

QByteArray& NmeaRecord::operator[](int i)
{
    return field(i);
}

//double NmeaRecord::operator[](int i)
//{
//    if (i < mFields.size()) {
//        return mFields.at(i).toDouble();
//    }
//    return 0;
//}

void NmeaRecord::setField(int i, int val)
{
    field(i) = QByteArray::number(val);
}

void NmeaRecord::setField(int i, double val, int prec)
{
    if (prec > 0)
        field(i) = QByteArray::number(val, 'f', prec);
    else
        field(i) = QByteArray::number(val, 'f', mPrecision);
}

void NmeaRecord::setField(int i, const QByteArray &val)
{
    field(i) = val;
}

int NmeaRecord::numberOfFields()
{
    return mFields.size();
}

void NmeaRecord::setNumberOfFields(int i)
{
    setSize(i);
}

bool NmeaRecord::isValid() const
{
    return mValid;
}

QByteArray NmeaRecord::header() const {
    if (mFields.size() >= 1)
        return mFields.at(0);
    return QByteArray();
}

void NmeaRecord::setSize(int size)
{
    int newSize = qBound(0, size, MaxFields);
    while (newSize < mFields.size()) {
        mFields.removeLast();
    }
    while (newSize > mFields.size()) {
        mFields.append(QByteArray());
    }
}

double NmeaRecord::fromDDM(int vIdx, int hIdx, double defaultVal) const {
    int idx = mFields[vIdx].indexOf('.');
    if (idx < 2) {
        return defaultVal;
    }
    bool ok;
    qreal val = mFields[vIdx].mid(0, idx - 2).toDouble(&ok);
    if (ok) {
        val += mFields[vIdx].mid(idx - 2).toDouble(&ok) / 60.0;
        if (ok) {
            if (hIdx) {
                if ((mFields[hIdx] == "S") || (mFields[hIdx] == "W")) {
                    val = -val;
                }
            }
            return val;
        }
    }
    return defaultVal;
}

void NmeaRecord::toDDM(double val, int vIdx, int latLon, int hIdx, int prec)
{
    double wrapped = fmod(val, 360.0);

    if (wrapped > 180.0) {
        wrapped -= 360.0;
    } else if (wrapped < -180.0) {
        wrapped += 360.0;
    }
    int deg = int(qAbs(wrapped));
    double min = double((qAbs(wrapped) - deg) * 60);

    if (qRound(min * pow(10.0, prec)) > 60 * pow(10.0, prec)) {
        min = qMax(min - 60, 0.0);
        deg++;
    }
    QString hemi;
    QString sign;

    if (hIdx != 0) {
        hemi = val < 0 ? (latLon ? "W" : "S") : (latLon ? "E" : "N");
    } else {
        if (val < 0)
            sign = "-";
    }
    if (deg == 0 && qRound(min * pow(10.0, prec)) == 0) {
        sign = QString();
        hemi = QString();
    }
    if (deg == 180 && qRound(min * pow(10.0, prec)) == 0) {
        hemi = QString();
    }

    QString coord = QString("%1%2%3").arg(sign).arg(deg, latLon ? 3 : 2, 10,
            QChar('0')).arg(min, 3 + prec, 'f', prec, QChar('0'));
    setField(vIdx, coord.toLatin1());
    if (hIdx)
        setField(hIdx, hemi.toLatin1());
}
