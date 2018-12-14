/*
 * ranger2gapsprocessor.h
 *
 *  Created on: Dec 14, 2018
 *      Author: jrenken
 */

#ifndef RANGER2GAPSPROCESSOR_H_
#define RANGER2GAPSPROCESSOR_H_

#include "dataprocessor.h"
#include <QMap>

class Ranger2GapsProcessor: public DataProcessor
{
public:
    Ranger2GapsProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "Ranger2Gaps";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);

    static QString doc();

private:
    NmeaRecord  mPTSAG;
    NmeaRecord  mPTSAH;
    NmeaRecord  mHEHDT;

    QMap<QString, int>  mBeacons;

    qreal       mShipsHeading;
    int         mFrameId;

    void getParameter(const QString& parList);

};

#endif /* RANGER2GAPSPROCESSOR_H_ */
