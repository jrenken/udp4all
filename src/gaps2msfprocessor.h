/*
 * gaps2msfprocessor.h
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#ifndef GAPS2MSFPROCESSOR_H_
#define GAPS2MSFPROCESSOR_H_

#include "dataprocessor.h"

class Gaps2MsfProcessor : public DataProcessor
{
public:
    Gaps2MsfProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "Gaps2Msf";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);

    static QString doc();
private:
    NmeaRecord  mMsf;
    bool        mBug;
};

#endif /* GAPS2MSFPROCESSOR_H_ */
