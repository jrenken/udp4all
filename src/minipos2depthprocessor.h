/*
 * minipos2depth.h
 *
 *  Created on: Feb 2, 2022
 *      Author: jrenken
 */

#ifndef MINIPOS2DEPTHPROCESSOR_H_
#define MINIPOS2DEPTHPROCESSOR_H_

#include <QStringList>
#include "dataprocessor.h"

class MiniPos2DepthProcessor : public DataProcessor
{
public:
    enum SentenceType { S_NONE = 0,
                        S_DBT = 1,
                        S_DBS = 2,
                        S_DBK = 4,
                        S_DPT = 8,
                        S_PKEL = 16 };

    MiniPos2DepthProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "MiniPos2Depth";
    }

    virtual QList<QByteArray> processData(const QByteArray& data);
    static QString doc();

private:
    NmeaRecord  mPKEL;
    NmeaRecord  mDBT;
    NmeaRecord  mDPT;

    SentenceType    mSentenceType;
    bool        mUseDepth;
};

#endif /* MINIPOS2DEPTHPROCESSOR_H_ */
