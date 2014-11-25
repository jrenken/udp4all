/*
 *  file:   forwardermodel.h
 *  author: jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
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

protected:
    void timerEvent(QTimerEvent *event);

private:
    QMap<QString, UdpForwarder*>    *mForwarders;
};

#endif /* FORWARDERMODEL_H_ */
