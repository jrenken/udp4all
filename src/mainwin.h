/*
 *  file:		mainwin.h
 *  author:		jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#ifndef MAINWIN_H
#define MAINWIN_H

#include <QtGui/QMainWindow>
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
	void on_actionReconnect_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();

private slots:
	void readPendingDatagrams();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWinClass 	ui;
    QUdpSocket			mSocket;
    int					mDgramCount;
    QPair<QHostAddress, quint16>			source;
    QList< QPair<QHostAddress, qint16> >	targets;
    QSystemTrayIcon *	trayIcon;
    QMenu *				trayIconMenu;
    ForwardManager*		mManager;
    void closeEvent(QCloseEvent *event);
    void loadConfiguration(QSettings& settings);
    void saveConfiguration(QSettings& settings);
    void bindSocket();
};

#endif // MAINWIN_H
