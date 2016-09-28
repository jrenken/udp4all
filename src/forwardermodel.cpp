/*
 *  file:   forwardermodel.cpp
 *  author: jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: forwardermodel.cpp 13 2014-11-26 12:29:48Z jrenken $
 */

#include <QDebug>
#include <QPixmap>
#include "forwardermodel.h"

ForwarderModel::ForwarderModel(QMap<QString, UdpForwarder*>* data, QObject *parent)
    : QAbstractTableModel(parent)
{
    Q_ASSERT(data != 0);
    mForwarders = data;
    startTimer(2000);
}

QVariant ForwarderModel::data(const QModelIndex &index, int role) const
{
    if ((role != Qt::DisplayRole) && (role != Qt::DecorationRole)
             && (role != Qt::ToolTipRole) && (role < Qt::UserRole))
        return QVariant();

    UdpForwarder *fw = mForwarders->values().at(index.row());
//  qDebug() << index.row() << index.column() << role << fw->statistics();
    if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            QString s = (fw->bound() ? ":/images/images/network-connect.png" :  ":/images/images/network-disconnect.png");
            return QPixmap(s);
        }
    } else if (role == Qt::UserRole + 1) {
        fw->setMonitor(true);
        return true;
    } else if (role == Qt::UserRole + 2) {
        fw->setMonitor(false);
        return false;
    } else if (role == Qt::UserRole + 3) {
        return fw->objectName();
    } else {
        switch (index.column()) {
        case 0:
            return fw->objectName();
        case 1:
            return fw->source();
        case 2:
            return fw->targets();
        case 3:
            return fw->inputs();
        case 4:
            return fw->processor() + " " + fw->processorParameter();
        case 5:
            return fw->statistics().first;
        case 6:
            return fw->statistics().second;
        }
    }
    return QVariant();
}

QVariant ForwarderModel::headerData(int section, Qt::Orientation orientation,
                    int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Source");
        case 2:
            return tr("Targets");
        case 3:
            return tr("Inputs");
        case 4:
            return tr("Processor");
        case 5:
            return tr("Received");
        case 6:
            return tr("Sent");
        default:
            return QVariant();
        }
    }
    return QVariant();
}


int ForwarderModel::rowCount(const QModelIndex &) const
{
    return mForwarders->size();
}

int ForwarderModel::columnCount(const QModelIndex &) const
{
    return 7;
}

void ForwarderModel::timerEvent(QTimerEvent *)
{
    emit dataChanged(index(0, 5), index(rowCount() - 1, 6));
}

