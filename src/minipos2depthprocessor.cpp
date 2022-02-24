/*
 * minipos2depth.cpp
 *
 *  Created on: Feb 2, 2022
 *      Author: jrenken
 */

#include "minipos2depthprocessor.h"


#define ENUM_OR(e_type, e, flag)   static_cast<e_type>(static_cast<int>(e) | static_cast<int>(flag))

MiniPos2DepthProcessor::MiniPos2DepthProcessor(const QString& parList)
    : DataProcessor(parList),
      mSentenceType(S_NONE),
      mUseDepth(false)
{
    mPKEL.setRecord(QByteArray("$PKEL,007,165809,LF,00.00,HF,02.55"));
    mDBT.setRecord(QByteArray("$GPDBT,87.3,f,26.6,M,14.5,F"));
    mDPT.setRecord(QByteArray("$GPDPT,0.0,0.0"));

    QStringList list = parList.split(' ');

    if (list.contains("DBT", Qt::CaseInsensitive))
        mSentenceType = ENUM_OR(SentenceType, mSentenceType, S_DBT);
    if (list.contains("DBS", Qt::CaseInsensitive))
        mSentenceType = ENUM_OR(SentenceType, mSentenceType, S_DBS);
    if (list.contains("DBK", Qt::CaseInsensitive))
        mSentenceType = ENUM_OR(SentenceType, mSentenceType, S_DBK);
    if (list.contains("DPT", Qt::CaseInsensitive))
        mSentenceType = ENUM_OR(SentenceType, mSentenceType, S_DPT);
    if (list.contains("PKEL", Qt::CaseInsensitive))
        mSentenceType = ENUM_OR(SentenceType, mSentenceType, S_PKEL);
    if (list.contains("depth"))
        mUseDepth = true;

}

QList<QByteArray> MiniPos2DepthProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;


    foreach (QByteArray ba, list) {
        double depth;
        NmeaRecord n(ba);
        if (n.isEmpty()) continue;
        if (n.header() == "$PSAAS") {
            if (mUseDepth) {
                depth = n.toDouble(6);
            } else {
                depth = n.toDouble(7);
            }
            if (mSentenceType & S_PKEL) {
                mPKEL[2] = n[1].left(6);
                mPKEL.setField(4, depth, 1);
                mPKEL[6] = mPKEL[4];
                out.append(mPKEL.sentence(true));
            }
            if (mSentenceType & S_DPT) {
                mDPT.setField(1, depth, 1);
                out.append(mDPT.sentence(true));
            }
            if (mSentenceType & (S_DBT | S_DBS | S_DBK)) {
                mDBT.setField(3, depth, 1);
                mDBT.setField(1, depth / 0.3048, 1);
                mDBT.setField(5, depth * 1.8288, 1);
                if (mSentenceType & S_DBT) {
                    mDBT.setHeader("GPDBT");
                    out.append(mDBT.sentence(true));
                }
                if (mSentenceType & S_DBS) {
                    mDBT.setHeader("GPDBS");
                    out.append(mDBT.sentence(true));
                }
                if (mSentenceType & S_DBK) {
                    mDBT.setHeader("GPDBK");
                    out.append(mDBT.sentence(true));
                }
            }
        } else
            continue;
    }
    return out;
}


QString MiniPos2DepthProcessor::doc()
{
    return "MiniPos2Depth: Convert a $PSAAS sentence depth or altitude to Echosounder protocol\n"
           "    Parameter:  DPT: send $GPDPT depth of water sentence.\n"
           "                DBT: send $GPDBT depth below transducer.\n"
           "                DPK: send $GPDBK depth below keel sentence.\n"
           "                DPS: send $GPDBTS depth below surface sentence.\n"
           "                PKEL: send $PKEL sentence.\n"
           "                depth: use depth, otherwise use altitude.\n\n";
}
