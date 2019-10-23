/*
 * gaps2praveprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include "gaps2praveprocessor.h"
#include <QStringList>

Gaps2PraveProcessor::Gaps2PraveProcessor(const QString& parList)
    : DataProcessor(parList)
{
    mPrave.setRecord(QByteArray("$PRAVE,0000,0000,0000.0000,00000.0000,000000,1,8,0,7,11.9,0,-61,0,0,,"));
    QStringList list = parList.split(' ');
    getParameter(parList);

}


QList<QByteArray> Gaps2PraveProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;

    foreach (QByteArray ba, list) {
        if (ba.isEmpty())
            continue;
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PTSAG") {
            mPrave[1] = QString("%1").arg(n[6].toInt(), 4, 10, QChar('0')).toLatin1();
            qreal l = n[7].toDouble();
            if (n[8].toUpper() == "S")
                l *= -1;
            mPrave[3] = QString("%1").arg(l, 9, 'f', 4, QChar('0')).toLatin1();
            l = n[9].toDouble();
            if (n[10].toUpper() == "W")
                l *= -1;
            mPrave[4] = QString("%1").arg(l, 10, 'f', 4, QChar('0')).toLatin1();
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
        } else if (n.header() == "$PMTMGPO") {
            int bid = mBeacons.value(n[1], -1);
            if (bid == -1)
                continue;
            mPrave[1] = QString("%1").arg(bid, 4, 10, QChar('0')).toLatin1();
            mPrave.toDDM(n.toDouble(4), 3, NmeaRecord::Lat, 0, 4);
            mPrave.toDDM(n.toDouble(5), 4, NmeaRecord::Lon, 0, 4);
            mPrave[5] = QString("%1").arg(n.toInt(3), 6, 10, QChar('0')).toLatin1();
            mPrave.setField(8, (int) (n[7].toDouble() * -1));
            mPrave.setField(14, int(n[9].toDouble()));
            out.append(mPrave.sentence(true));
        }

    }
    return out;
}


void Gaps2PraveProcessor::getParameter(const QString& parList)
{
    QStringList list = parList.split(QRegExp(",\\s*"), QString::SkipEmptyParts);
    if (list.size() > 0) {
        if (!list.at(0).contains(QRegExp("[<>:]"))) {
            bool ok;
            int talker = list.at(0).toInt(&ok);
            if (ok) {
                mPrave[2] = QString("%1").arg(talker, 4, 10, QChar('0')).toLatin1();
            }
        }
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

QString Gaps2PraveProcessor::doc()
{
    return "Gaps2Prave:   Convert a $PTSAG or a $PMTMGPO sentence with the relevant beacon id or sender key to $PRAVE sentences.\n"
           "    Parameter:  To ID [0 - 9999] and id mapping in the form <key:id>\n"
           "                e.g. \"10, <SHP:0>, <ROV:1>\"\n"
           "                The parameters need to be quoted as a comma separated list.\n\n";
}
