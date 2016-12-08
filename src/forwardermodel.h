/*
 *  file:   forwardermodel.h
 *  author: jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: forwardermodel.h 13 2014-11-26 12:29:48Z jrenken $
 */

#ifndef FORWARDERMODEL_H_
#define FORWARDERMODEL_H_

#include <QAbstractItemModel>
#include <QTimerEvent>
#include "udpforwarder.h"

class ForwarderModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    ForwarderModel(QMap<QString, UdpForwarder*>* data, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void updateData() {
        beginResetModel();
        endResetModel();
        //reset();
    }

protected:
    void timerEvent(QTimerEvent *event);

private:
    QMap<QString, UdpForwarder*>    *mForwarders;
};

#endif /* FORWARDERMODEL_H_ */
