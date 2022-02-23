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
    MiniPos2DepthProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "MiniPos2Depth";
    }

    virtual QList<QByteArray> processData(const QByteArray& data);
    static QString doc();

private:
    NmeaRecord  mPKEL;
    NmeaRecord  mDBT;

    bool        mUseDepth;
};

#endif /* MINIPOS2DEPTHPROCESSOR_H_ */
