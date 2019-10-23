/*
 *  file:       mainwin.cpp
 *  author:     jrenken
 *
 *  $Rev: 17 $
 *  $Author: jrenken $
 *  $Date: 2016-06-03 10:16:49 +0200 (Fr, 03. Jun 2016) $
 *  $Id: mainwin.cpp 17 2016-06-03 08:16:49Z jrenken $
 */

#include "mainwin.h"
#include <QMessageBox>
#include <QTextEdit>
#include <QInputDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QFont>
#include <QPushButton>
#include "nmearecord.h"
#include "forwarderdlg.h"

#define STR(x)   #x
#define XSTR(x)  STR(x)

#ifdef GITTAG
#define __GITTAG__ XSTR(GITTAG)
#else
#define __GITTAG__ "V0.16.0"
#endif


class HelpDialog : public QDialog
{
public:
    HelpDialog(const QString& title, const QString& helpText, QWidget *parent = 0)
        : QDialog(parent)
    {
        QTextEdit* textEdit = new QTextEdit(this);
        textEdit->setReadOnly(true);
        textEdit->setLineWrapMode(QTextEdit::NoWrap);
        textEdit->setText(helpText);
        QFont font("Courier10");
        font.setFixedPitch(true);
        textEdit->setFont(font);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);



        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(textEdit);
        layout->addWidget(buttonBox);
        setLayout(layout);

        setWindowTitle(title);
        if (parent) {
            resize(parent->size() * 0.8);
        } else {
            resize(800, 600);
        }
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

};

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.toolButtonMonitor->setDefaultAction(ui.actionMonitor);
    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    ui.tabWidget->removeTab(2);
    QSettings   settings;

    move(settings.value("pos", QPoint(200, 200)).toPoint());
    resize(settings.value("size", QSize(800, 600)).toSize());
    ui.splitter->restoreState(settings.value("splitter").toByteArray());

    setupForwarders(settings);

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


void MainWin::setupForwarders(QSettings& settings)
{
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
    ui.tableView->resizeColumnsToContents();

    QItemSelectionModel *selectionModel = new QItemSelectionModel(mManager->model());
    ui.tableView->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(currentChanged ( const QModelIndex &, const QModelIndex &) ),
            this, SLOT(onCurrentChanged(const QModelIndex&, const QModelIndex &)));
}


void MainWin::displayRecMonitorData(const QByteArray& data)
{
    QString s = "<- " + QString(data).trimmed();
    ui.textEditMonitor->append(s);
}

void MainWin::displaySendMonitorData(const QByteArray& data)
{
    QString s = QString("<b><FONT COLOR=\"#0000F0\">-> %1</FONT></b>")
            .arg(QString(data).trimmed().replace("&", "&amp;").replace("<", "&lt;"));
    ui.textEditMonitor->append(s);
}

void MainWin::onCurrentChanged(const QModelIndex & current, const QModelIndex & prev )
{
    prev.data(Qt::UserRole + 2);
    if (ui.actionMonitor->isChecked())
        current.data(Qt::UserRole + 1);
}


void MainWin::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName( this,
            tr("Select config file"), QDir::homePath(), tr("config files (*.conf *.ini)"));
    if (!file.isEmpty()) {
        QSettings settings(file, QSettings::IniFormat);
        ui.textEditLog->append("<b><FONT COLOR=\"#00F0F0\">Load configuration from  " + file + "</b>");
        delete mManager;
        setupForwarders(settings);
        QSettings registry;
        copySettings(settings, registry);
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
        QSettings from;
        copySettings(from, settings);
        ui.textEditLog->append("<b><FONT COLOR=\"#F0F000\">Save configuration to  " + file + "</b>");
    }
}

void MainWin::on_actionConfigFileExample_triggered()
{
    QFile f(":/config/udp4all.conf");
    if (f.open(QIODevice::ReadOnly)) {
        QString s = f.readAll();
        s.prepend("<FONT COLOR=\"#0020F0\">");
        s.replace("\n", "<br>");
        HelpDialog dlg(tr("ConfigFile Example"), s, this);
        dlg.exec();
    }
}

void MainWin::on_actionDataProcessorInfo_triggered()
{
    HelpDialog dlg(tr("DataProcessor Info"), ForwardManager::doc(), this);
    dlg.exec();
}

void MainWin::on_actionAbout_triggered()
{
    QString versionInfo =   "<B>udp4all " __GITTAG__ "</B><p>"
#ifdef GITDATE
                            "Date: " XSTR(GITDATE) "<p>"
#endif
                            "Author: jrenken   <a href=mailto:renken@marum.de>renken@marum.de</a><p>"
                            "(C) <i>Copyright 2011</i>, <b>University of Bremen, Marum</b>";
#ifdef GITHASH
    versionInfo += "<p>Commit: <b>" XSTR(GITHASH) "</b>";
#endif
    QMessageBox::about ( this,
                    "About udp4all", versionInfo);
}

void MainWin::on_actionMonitor_triggered()
{
    if (!ui.actionMonitor->isChecked()) {
        mManager->setMonitor(false);
    }
}

void MainWin::closeEvent(QCloseEvent *event)
{
    QSettings   settings;
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
        break;
    }
}

void MainWin::copySettings(QSettings& from, QSettings& to)
{
    QStringList keys = from.allKeys();

    foreach (QString key, keys) {
        to.setValue(key, from.value(key));
    }
}

void MainWin::on_actionNewForwarder_triggered()
{
    ForwarderDlg *dlg = new ForwarderDlg(this);
    if (dlg->exec() == QDialog::Accepted) {
        QSettings globalSettings;
        mManager->updateForwarder(dlg->settings(), globalSettings);
    }
    delete dlg;
}

void MainWin::on_actionEditForwarder_triggered()
{
    QModelIndex idx = ui.tableView->currentIndex();
    if ( idx.isValid()) {
        QString name = idx.data(Qt::UserRole + 3).toString();
        UdpForwarder *fw = mManager->forwarder(name);
        if ( !fw )
            return;

        ForwarderDlg *dlg = new ForwarderDlg(this, fw->settings());
        if (dlg->exec() == QDialog::Accepted) {
            QSettings globalSettings;
            mManager->updateForwarder(dlg->settings(), globalSettings);
        }
        delete dlg;

    }

}

void MainWin::on_actionDeleteForwarder_triggered()
{
    QModelIndex idx = ui.tableView->currentIndex();
    if ( idx.isValid()) {
        QString name = idx.data(Qt::UserRole + 3).toString();
        QSettings globalSettings;
        mManager->deleteForwarder(name, globalSettings);
    }
}
