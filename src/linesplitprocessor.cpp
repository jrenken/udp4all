/*
 * linesplitprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include <QBuffer>
#include "linesplitprocessor.h"

LineSplitProcessor::LineSplitProcessor(const QString& parList)
    : DataProcessor(parList),
      mNoFiltering(true),
      mInhibitAll(false)
{
    getParameter(parList);
}

void LineSplitProcessor::getParameter(const QString& parList)
{
    foreach (QString s, parList.split(' ')) {
        if (s.startsWith('+')) {
            mSelectivePatterns << s.midRef(1).toLatin1().toUpper();
            mInhibitAll = true;
            mNoFiltering = false;
        } else if (s.startsWith('-')) {
            mInhibitPatterns << s.midRef(1).toLatin1().toUpper();
            mNoFiltering = false;
        }
    }
}


QList<QByteArray> LineSplitProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list;
    mBuffer.append(data);
    QBuffer buffer(&mBuffer);
    buffer.open(QIODevice::ReadOnly);
    while (buffer.canReadLine()) {
        QByteArray ba = buffer.readLine(256);
        if (mNoFiltering) {
            list.append(ba);
        } else {
            int headerEnd = ba.indexOf(',');
            if (mInhibitAll) {
                foreach (QByteArray tba, mSelectivePatterns) {
                    if (ba.lastIndexOf(tba, headerEnd) > -1) {
                        list.append(ba);
                        break;
                    }
                }
            } else {
                bool send = true;
                foreach (QByteArray tba, mInhibitPatterns) {
                    if (ba.lastIndexOf(tba, headerEnd) > 0) {
                        send = false;
                    }
                }
                if (send)
                    list.append(ba);
            }
        }
    }
    mBuffer.remove(0, buffer.pos());
    return list;
}

QString LineSplitProcessor::doc()
{
    return  "LineSplitProcessor: Split one multiline UDP datagram into several datagrams each containing only one line\n"
            "                    and optional filter the sentences by header. The filter could be selective or inhibiting.\n"
            "    Parameter:  -pattern: inhibit sending sentences whith pattern in their header.\n"
            "                +pattern: inhibit all sentences except those containing pattern in their header.\n"
            "                eg: +HEHDT inhibits all sentences except those starting with $HEHDT.\n"
            "                    -GGA inhibits sentences containing GGA in their header, eg. $GPGGA or $GNGGA.\n\n";
}
