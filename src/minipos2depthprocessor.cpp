/*
 * minipos2depth.cpp
 *
 *  Created on: Feb 2, 2022
 *      Author: jrenken
 */

#include "minipos2depthprocessor.h"
#include <QDebug>
MiniPos2DepthProcessor::MiniPos2DepthProcessor(const QString& parList)
    : DataProcessor(parList),
      mUseDepth(false)
{
    mPKEL.setRecord(QByteArray("$PKEL,007,165809,LF,00.00,HF,02.55"));
    mDBT.setRecord(QByteArray("$GPDBT,87.3,f,26.6,M,14.5,F"));
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
                mDBT[3] = n[6];
                mDBT.setField(1, n.toDouble(6) / 0.3048, 1);
                mDBT.setField(5, n.toDouble(6) * 1.8288, 1);
            } else {
                mPKEL[4] = n[7];
                mPKEL[6] = n[7];
                mDBT[3] = n[7];
                mDBT.setField(1, n.toDouble(7) / 0.3048, 1);
                mDBT.setField(5, n.toDouble(7) * 1.8288, 1);
            }
            out.append(mPKEL.sentence(true));
            out.append(mDBT.sentence(true));
            qDebug() << mDBT.sentence(true);
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
