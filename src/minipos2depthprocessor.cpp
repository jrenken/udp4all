/*
 * minipos2depth.cpp
 *
 *  Created on: Feb 2, 2022
 *      Author: jrenken
 */

#include "minipos2depthprocessor.h"

MiniPos2DepthProcessor::MiniPos2DepthProcessor(const QString& parList)
    : DataProcessor(parList),
      mUseDepth(false)
{
    mPKEL.setRecord(QByteArray("$PKEL,007,165809,LF,00.00,HF,02.55"));
}

QList<QByteArray> MiniPos2DepthProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;


    foreach (QByteArray ba, list) {
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PSAAS") {
            mPKEL[2] = n[1].left(6);
            if (mUseDepth) {
                mPKEL[4] = n[6];
                mPKEL[6] = n[6];
            } else {
                mPKEL[4] = n[7];
                mPKEL[6] = n[7];
            }
            out.append(mPKEL.sentence(true));
        } else
            continue;
    }
    return out;
}


QString MiniPos2DepthProcessor::doc()
{
    return "MiniPos2Depth: Convert a $PSAAS sentence depth or altitude to Echosounder protocol\n"
           "    Parameter:      depth: use depth, otherwise use altitude.\n\n";
}
