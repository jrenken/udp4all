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
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDebug>



MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent),
    mDgramCount(0),
    source(QHostAddress::Any, 2000)
{
	ui.setupUi(this);
	QSettings	settings;

	move(settings.value("pos", QPoint(200, 200)).toPoint());
	resize(settings.value("size", QSize(800, 600)).toSize());
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
	connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(this, SIGNAL(datagramCountChanged(int)),
			ui.labelDatagrams, SLOT(setNum(int)));
	loadConfiguration(settings);
}

MainWin::~MainWin()
{
}

void MainWin::readPendingDatagrams()
{
    while (mSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mSocket.pendingDatagramSize());

        mSocket.readDatagram(datagram.data(), datagram.size());
        if (ui.actionMonitor->isChecked()) {
        	ui.textEdit->append(datagram);
        }
        for (int i = 0; i < targets.size(); i++) {
        	mSocket.writeDatagram(datagram.data(),
        			targets[i].first, targets[i].second);
        }
        emit datagramCountChanged(++mDgramCount);
    }

}

void MainWin::loadConfiguration(QSettings& settings)
{
	QString	s;
	bool ok;
	s = settings.value("Sockets/Listener", "0.0.0.0:2000" ).toString();
	source.first = QHostAddress(s.section(':', 0, 0));
	source.second = s.section(':', 1).toInt(&ok);
	if (ok) {
		bindSocket();
	}
	targets.clear();
	int size = settings.beginReadArray("Sockets/Targets");
	qDebug() << size;
	for (int i = 0; i< size; i++) {
		settings.setArrayIndex(i);
		s = settings.value("Target" ).toString();
		quint16 port = s.section(':', 1).toInt(&ok);
		if (ok) {
			ui.textEdit->append("<b>Add Target: " + s + "</b>");
			targets.append(QPair<QHostAddress, qint16>(QHostAddress(s.section(':', 0, 0)), port));
		}
	}
	settings.endArray();
}

void MainWin::bindSocket()
{
	QString s = source.first.toString()
			+ ":" + QString::number(source.second);
	mSocket.disconnectFromHost();
	if (mSocket.bind(source.first, source.second)) {
		ui.textEdit->append("<b><FONT COLOR=\"#00F000\">Socket bound to " + s + "</FONT></b>");
	} else {
		ui.textEdit->append("<b><FONT COLOR=\"#FF0000\">Failed to bind socket to " + s + "</FONT></b>");
	}
}


void MainWin::saveConfiguration(QSettings& settings)
{
	settings.setValue("Sockets/Listener", source.first.toString()
			+ ":" + QString::number(source.second));
	settings.beginWriteArray("Sockets/Targets");
	for (int i = 0; i < targets.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("Target", targets[i].first.toString()
				+ ":" + QString::number(targets[i].second));
	}
	settings.endArray();
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
		ui.textEdit->append("<b><FONT COLOR=\"#00F0F0\">Load configuration from  " + file + "</b>");
		loadConfiguration(settings);
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
		ui.textEdit->append("<b><FONT COLOR=\"#F0F000\">Save configuration to  " + file + "</b>");
		saveConfiguration(settings);
	}
}



void MainWin::on_actionAbout_triggered()
{
    QString versionInfo = 	"<B>udp4all</B><p>"
                            "$Rev$<p>"
                            "$Date$<p>"
                            "$Author$   <a href=mailto:renken@marum.de>renken@marum.de</a><p>"
							"(C) <i>Copyright 2010</i>, <b>University of Bremen, Marum</b>";
    QMessageBox::about ( this,
                    "About posiview2", versionInfo.replace('$', ' '));


}

void MainWin::on_actionMonitor_triggered()
{
	if (ui.actionMonitor->isChecked()) {
	} else {
	}
}

void MainWin::closeEvent(QCloseEvent *event)
{
	QSettings	settings;
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	saveConfiguration(settings);
	event->accept();

}

