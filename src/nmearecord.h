/*
 *  file:   nmearecord.h
 *  author: jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#ifndef NMEARECORD_H_
#define NMEARECORD_H_

#include <QList>
#include <QByteArray>


class NmeaRecord
{
public:
    static const int MaxFields;

    NmeaRecord(int size = 1);
    NmeaRecord(const char* rec);
    NmeaRecord(const QByteArray& rec);
    NmeaRecord(const QString &rec);
    QByteArray sentence(bool cs = true);
    QByteArray checkSum(const QByteArray &ba);

    int setRecord(const QByteArray& rec);
    int setRecord(const QString& rec);

    QByteArray& operator[](int i);
//    double operator[](int i);

    void setField(int i, int val);
    void setField(int i, double val);
    void setField(int i, const QByteArray &val);


    int numberOfFields();
    void setNumberOfFields(int i);

    int precision() const {
        return mPrecision;
    }
    void setPrecision(int pr) {
        mPrecision = pr;
    }

    bool isValid() const;
    QByteArray header() const;

    bool isEmpty() const {
        return (mFields.size() == 0);
    }

private:
    QList<QByteArray>   mFields;
    bool                mValid;
    int                 mPrecision;
    void setSize(int size);
    QByteArray& field(int i);
};

#endif /* NMEARECORD_H_ */
