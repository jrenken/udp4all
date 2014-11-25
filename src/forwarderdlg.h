#ifndef FORWARDERDLG_H
#define FORWARDERDLG_H

#include <QtGui/QDialog>
#include "udpforwarder.h"
#include "ui_forwarderdlg.h"

class ForwarderDlg : public QDialog
{
    Q_OBJECT

public:
    ForwarderDlg(QWidget *parent = 0, QHash<QString,QVariant> settings = QHash<QString,QVariant>() );

    QHash<QString,QVariant> settings() const;

private:
    Ui::ForwarderDlgClass ui;
    UdpForwarder          mForwarder;
};

#endif // FORWARDERDLG_H
