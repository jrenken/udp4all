/*
 *  file:	nmearecord.cpp
 *  author:	jrenken
 *
 *  $Rev:$
 *  $Author:$
 *  $Date:$
 *  $Id:$
 */

#include <QString>
#include "nmearecord.h"

const int NmeaRecord::MaxFields = 64;

NmeaRecord::NmeaRecord(int size)
    : mPrecision(4)
{
    setSize(size);
}

NmeaRecord::NmeaRecord(const char* rec)
    : mPrecision(4)
{
    setRecord(QByteArray(rec));
}

NmeaRecord::NmeaRecord(const QByteArray& rec)
    : mPrecision(4)
{
    setRecord(rec);
}

NmeaRecord::NmeaRecord(const QString& rec)
    : mPrecision(4)
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

void NmeaRecord::setField(int i, double val)
{
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
