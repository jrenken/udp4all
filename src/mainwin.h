/*
 *  file:       mainwin.h
 *  author:     jrenken
 *
 *  $Rev: 13 $
 *  $Author: jrenken $
 *  $Date: 2014-11-26 13:29:48 +0100 (Mi, 26. Nov 2014) $
 *  $Id: mainwin.h 13 2014-11-26 12:29:48Z jrenken $
 */

#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QPair>
#include <QList>
#include <QHostAddress>
#include <QSystemTrayIcon>
#include "forwardmanager.h"
#include "ui_mainwin.h"

class QSettings;

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = 0);
    ~MainWin();


signals:
    void datagramCountChanged(int);

public slots:
    void on_actionAbout_triggered();
    void on_actionMonitor_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionNewForwarder_triggered();
    void on_actionEditForwarder_triggered();
    void on_actionDeleteForwarder_triggered();
    void on_actionConfigFileExample_triggered();
    void on_actionDataProcessorInfo_triggered();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void displayRecMonitorData(const QByteArray& data);
    void displaySendMonitorData(const QByteArray& data);
    void onCurrentChanged(const QModelIndex & current, const QModelIndex &  );

private:
    Ui::MainWinClass    ui;
    QSystemTrayIcon *   trayIcon;
    QMenu *             trayIconMenu;
    ForwardManager*     mManager;
    void closeEvent(QCloseEvent *event);
    void setupForwarders(QSettings& settings);
    void copySettings(QSettings& from, QSettings& to);
};

#endif // MAINWIN_H
