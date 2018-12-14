#include "forwarderdlg.h"

ForwarderDlg::ForwarderDlg(QWidget *parent, QHash<QString,QVariant> settings )
    : QDialog(parent)
{
	ui.setupUi(this);
	QStringList procs;
	procs << "" << "Gaps2Msf" << "Gaps2Gps"
	      << "Gaps2Prave" << "LineSplit"
	      << "Ais2Gps" << "Ranger2Gaps";
	ui.comboBoxProcessors->addItems(procs);

	if (!settings.isEmpty()) {
	    ui.lineEditName->setText(settings.value("Name").toString());
	    ui.lineEditSource->setText(settings.value("Source").toString());
	    int i = ui.comboBoxProcessors->findText(settings.value("Processor").toString());
	    if ( i != -1)
	        ui.comboBoxProcessors->setCurrentIndex(i);
	    ui.lineEditProcessorParameter->setText(settings.value("Processor.Parameter").toString());
	    ui.plainTextEditInputs->setPlainText(settings.value("Inputs").toString()
	            .split(QRegExp("\\,?\\s+"), QString::SkipEmptyParts).join("\n"));
	    ui.plainTextEditTargets->setPlainText(settings.value("Targets").toString()
	            .split(QRegExp("\\,?\\s+"), QString::SkipEmptyParts).join("\n"));
	    ui.spinBoxDelay->setValue(settings.value("Delay", 0).toInt());
	}
}


QHash<QString,QVariant> ForwarderDlg::settings() const
{
    QHash<QString, QVariant> settings;
    settings.insert("Name", ui.lineEditName->text());
    settings.insert("Inputs", ui.plainTextEditInputs->toPlainText().replace("\n", " ").simplified());
    settings.insert("Source", ui.lineEditSource->text());
    settings.insert("Processor", ui.comboBoxProcessors->currentText());
    settings.insert("Processor.Parameter", ui.lineEditProcessorParameter->text());
    settings.insert("Targets", ui.plainTextEditTargets->toPlainText().replace("\n", " ").simplified());
    settings.insert("Delay", ui.spinBoxDelay->value());
    return settings;
}
