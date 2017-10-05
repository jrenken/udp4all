/*
 * linesplitprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include <QBuffer>
#include "linesplitprocessor.h"

LineSplitProcessor::LineSplitProcessor(const QString& parList)
    : DataProcessor(parList)
{

}

QList<QByteArray> LineSplitProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list;
    mBuffer.append(data);
    QBuffer buffer(&mBuffer);
    buffer.open(QIODevice::ReadOnly);
    while (buffer.canReadLine()) {
        QByteArray ba = buffer.readLine(256);
        list.append(ba);
    }
    mBuffer.remove(0, buffer.pos());
    return list;
}

QString LineSplitProcessor::doc()
{
    return  "LineSplitProcessor: Split one multiline UDP datagram into several datagrams each containing only one line.\n"
            "   Parameter: None\n\n";
}
