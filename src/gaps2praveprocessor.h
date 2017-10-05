/*
 * gaps2praveprocessor.h
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#ifndef GAPS2PRAVEPROCESSOR_H_
#define GAPS2PRAVEPROCESSOR_H_

#include "dataprocessor.h"

class Gaps2PraveProcessor : public DataProcessor
{
public:
    Gaps2PraveProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "Gaps2Prave";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);

    static QString doc();
private:
    NmeaRecord  mPrave;

};

#endif /* GAPS2PRAVEPROCESSOR_H_ */
