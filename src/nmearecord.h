/*
 *  file:   nmearecord.h
 *  author: jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: nmearecord.h 13 2014-11-26 12:29:48Z jrenken $
 */

#ifndef NMEARECORD_H_
#define NMEARECORD_H_

#include <QList>
#include <QByteArray>


class NmeaRecord
{
public:
    enum { Lat, Lon };

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
    void setField(int i, double val, int prec = -1);
    void setField(int i, const QByteArray &val);

    double fromDDM(int vIdx, int hIdx = 0, double defaultVal = 0.0) const;
    void toDDM(double val, int vIdx, int latLon, int hIdx = 0, int prec = 6);


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
