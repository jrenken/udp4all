/*
 *  file:	dataprocessor.cpp
 *  author:	jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#include "dataprocessor.h"
#include <QDebug>

DataProcessor::DataProcessor()
{
}

DataProcessor::~DataProcessor()
{
}

QList<QByteArray> DataProcessor::processData(const QByteArray& data)
{
	QList<QByteArray> list;
	list.append(data);
	return list;
}
Gaps2MsfProcessor::Gaps2MsfProcessor()
    : DataProcessor()
{
    mMsf.setRecord(QByteArray("$MSF,,,SHIP,Meteor,MSF0,,,,,,,,,,"));
    qDebug() << mMsf.sentence(true);
}

QList<QByteArray> Gaps2MsfProcessor::processData(const QByteArray& data)
{
    QList<QByteArray> list = data.split('\n');
    QList<QByteArray> out;
     foreach (QByteArray ba, list) {
        NmeaRecord n(ba);
        qDebug() << n.sentence(true);
        if (n.header() == "$PTSAG") {
            if (n[6].toInt() == 0) {
                mMsf[1] = n[5] + n[4] + n[3];
                mMsf[2] = n[2];
            }
        } else if (n.header() == "$PTSAH") {
            mMsf[10] = n[2];
            out.append(mMsf.sentence(true));
        } else if (n.header() == "$HEHDT") {
            mMsf[10] = n[1];
            out.append(mMsf.sentence(true));
        }
    }
//    qDebug() << out;
    return out;
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
