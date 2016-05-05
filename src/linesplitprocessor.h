/*
 * linesplitprocessor.h
 *
 *  Created on: 25.11.2014
 *      Author: jrenken
 */

#ifndef LINESPLITPROCESSOR_H_
#define LINESPLITPROCESSOR_H_

#include "dataprocessor.h"

class LineSplitProcessor : public DataProcessor
{
public:
    LineSplitProcessor(const QString& parList = QString());
    virtual QString typeName() const {
        return "LineSplit";
    }
    virtual QList<QByteArray> processData(const QByteArray& data);
private:
    QByteArray  mBuffer;
};


#endif /* LINESPLITPROCESSOR_H_ */
