/*
 *  file:		mainwin.cpp
 *  author:		jrenken
 *
 *  $Rev$
 *  $Author$
 *  $Date$
 *  $Id$
 */

#include "mainwin.h"
#include "svnversion.h"
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QDebug>



MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent),
    mDgramCount(0),
    source(QHostAddress::Any, 2000)
{
	ui.setupUi(this);
	ui.toolButtonMonitor->setDefaultAction(ui.actionMonitor);
	QSettings	settings;

	move(settings.value("pos", QPoint(200, 200)).toPoint());
	resize(settings.value("size", QSize(800, 600)).toSize());
	ui.splitter->restoreState(settings.value("splitter").toByteArray());

	mManager = new ForwardManager(this);
	connect(mManager, SIGNAL(newMessage(const QString&)),
			ui.textEditLog, SLOT(append(const QString&)));
	connect(mManager, SIGNAL(newRecMonitorData(const QByteArray&)),
			SLOT(displayRecMonitorData(const QByteArray&)));
	connect(mManager, SIGNAL(newSendMonitorData(const QByteArray&)),
			SLOT(displaySendMonitorData(const QByteArray&)));
	mManager->loadConfiguration(settings);
	mManager->bindAll();
	ui.tableView->setModel(mManager->model());

    QItemSelectionModel *selectionModel = new QItemSelectionModel(mManager->model());
    ui.tableView->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(currentChanged ( const QModelIndex &, const QModelIndex &) ),
	        this, SLOT(onCurrentChanged(const QModelIndex&, const QModelIndex &)));

	trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui.actionExit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->setIcon(QIcon(":/images/images/udp4all.png"));
	trayIcon->show();
}

MainWin::~MainWin()
{
}





void MainWin::bindSocket()
{
	QString s = source.first.toString()
			+ ":" + QString::number(source.second);
	mSocket.disconnectFromHost();
	if (mSocket.bind(source.first, source.second)) {
		ui.textEditLog->append("<b><FONT COLOR=\"#00F000\">Socket bound to " + s + "</FONT></b>");
	} else {
		ui.textEditLog->append("<b><FONT COLOR=\"#FF0000\">Failed to bind socket to " + s + "</FONT></b>");
	}
}


void MainWin::displayRecMonitorData(const QByteArray& data)
{
	QString s = "<- " + QString(data).trimmed();
	ui.textEditMonitor->append(s);
}

void MainWin::displaySendMonitorData(const QByteArray& data)
{
	QString s = QString("<b><FONT COLOR=\"#0000F0\">-> %1</FONT></b>").arg(QString(data).trimmed());
	ui.textEditMonitor->append(s);
}

void MainWin::onCurrentChanged(const QModelIndex & current, const QModelIndex & prev )
{
	prev.data(Qt::UserRole + 2);
	if (ui.actionMonitor->isChecked())
		current.data(Qt::UserRole + 1);
}


void MainWin::on_actionReconnect_triggered()
{
	bindSocket();
}

void MainWin::on_actionOpen_triggered()
{
	QString	file = QFileDialog::getOpenFileName( this,
			tr("Select config file"), QDir::homePath(), tr("config files (*.conf *.ini)"));
	if (!file.isEmpty()) {
		QSettings settings(file, QSettings::IniFormat);
		ui.textEditLog->append("<b><FONT COLOR=\"#00F0F0\">Load configuration from  " + file + "</b>");
	}

}

void MainWin::on_actionSave_triggered()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Select config file"),
			QDir::homePath(), tr("config files (*.conf *.ini)"));
	if (!file.isEmpty()) {
		QFileInfo fi(file);
		if (fi.suffix().isEmpty())
			file += ".conf";
		QSettings settings(file, QSettings::IniFormat);
		ui.textEditLog->append("<b><FONT COLOR=\"#F0F000\">Save configuration to  " + file + "</b>");
	}
}



void MainWin::on_actionAbout_triggered()
{
    QString versionInfo =   "<B>udp4all</B><p>"
                            "Rev: "SVNVERSION" $<p>"
                            "Date: "SVNDATE" $<p>"
                            "$Author$   <a href=mailto:renken@marum.de>renken@marum.de</a><p>"
                            "(C) <i>Copyright 2011</i>, <b>University of Bremen, Marum</b>";
    QMessageBox::about ( this,
                    "About udp4all", versionInfo.replace('$', ' '));
}

void MainWin::on_actionMonitor_triggered()
{
	if (!ui.actionMonitor->isChecked()) {
		mManager->setMonitor(false);
	}
}

void MainWin::closeEvent(QCloseEvent *event)
{
	QSettings	settings;
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("splitter", ui.splitter->saveState());
	event->accept();

}

void MainWin::iconActivated(QSystemTrayIcon::ActivationReason reason)
 {
     switch (reason) {
     case QSystemTrayIcon::Trigger:
     case QSystemTrayIcon::DoubleClick:
    	 setVisible(!isVisible());
    	 break;
     case QSystemTrayIcon::MiddleClick:
         break;
     default:
         ;
     }
 }
