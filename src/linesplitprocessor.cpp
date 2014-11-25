/*
 * linesplitprocessor.cpp
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#include "linesplitprocessor.h"

LineSplitProcessor::LineSplitProcessor(const QString& parList)
    : DataProcessor(parList)
{

}

QList<QByteArray> LineSplitProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list;

    int from = 0, to = 0;
    while (from < data.size() && to != -1) {
        to  = data.indexOf('\n', from);
        list.append(data.mid(from, qMax(-1, to - from + 1)));
        from = to + 1;
    }
    return list;
}
