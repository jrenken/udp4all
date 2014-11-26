/*
 * gaps2praveprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include "gaps2praveprocessor.h"
#include <QStringList>
#include <QDebug>

Gaps2PraveProcessor::Gaps2PraveProcessor(const QString& parList)
    : DataProcessor(parList)
{
    mPrave.setRecord(QByteArray("$PRAVE,0000,0000,0000.0000,00000.0000,000000,1,8,0,24,11.6,0,0,0,0,,"));
    QStringList list = parList.split(' ');
    if (list.size() > 0 && !list.at(0).isEmpty())
        mPrave[2] = QString("%1").arg(list.at(0).toInt(), 4, 16, QChar('0')).toAscii();
}


QList<QByteArray> Gaps2PraveProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;

    foreach (QByteArray ba, list) {
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PTSAG") {
            mPrave[1] = QString("%1").arg(n[6].toInt(), 4, 10, QChar('0')).toAscii();
            qreal l = n[7].toDouble();
            if (n[8].toUpper() == "S")
                l *= -1;
            mPrave[3] = QString("%1").arg(l, 9, 'f', 4, QChar('0')).toAscii();
            l = n[9].toDouble();
            if (n[10].toUpper() == "W")
                l *= -1;
            mPrave[4] = QString("%1").arg(l, 10, 'f', 4, QChar('0')).toAscii();
            mPrave[5] = n[2];
            mPrave.setField(8, (int) (n[12].toDouble() * -1));
            if (n[6].toInt() != 0)
                out.append(mPrave.sentence(true));
        } else if (n.header() == "$PTSAH") {
            mPrave[14] = n[2];
            out.append(mPrave.sentence(true));
        } else if (n.header() == "$HEHDT") {
            mPrave[14] = n[1];
            out.append(mPrave.sentence(true));
        }

    }
    return out;
}
