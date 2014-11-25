#include "forwarderdlg.h"

ForwarderDlg::ForwarderDlg(QWidget *parent, QHash<QString,QVariant> settings )
    : QDialog(parent)
{
	ui.setupUi(this);
	QStringList procs;
	procs << "" << "Gaps2Msf" << "Gaps2Gps" << "Gaps2Prave" << "LineSplit";
	ui.comboBoxProcessors->addItems(procs);

	if (!settings.isEmpty()) {
	    ui.lineEditName->setText(settings.value("Name").toString());
	    ui.lineEditSource->setText(settings.value("Source").toString());
	    int i = ui.comboBoxProcessors->findText(settings.value("Processor").toString());
	    if ( i != -1)
	        ui.comboBoxProcessors->setCurrentIndex(i);
	    ui.lineEditProcessorParameter->setText(settings.value("Processor.Parameter").toString());
	    ui.plainTextEditInputs->setPlainText(settings.value("Inputs").toString().split(" ").join("\n"));
	    ui.plainTextEditTargets->setPlainText(settings.value("Targets").toString().split(" ").join("\n"));
	}
}


QHash<QString,QVariant> ForwarderDlg::settings() const
{
    QHash<QString, QVariant> settings;
    settings.insert("Name", ui.lineEditName->text());
    settings.insert("Inputs", ui.plainTextEditInputs->toPlainText().trimmed().replace("\n", " "));
    settings.insert("Source", ui.lineEditSource->text());
    settings.insert("Processor", ui.comboBoxProcessors->currentText());
    settings.insert("Processor.Parameter", ui.lineEditProcessorParameter->text());
    settings.insert("Targets", ui.plainTextEditTargets->toPlainText().trimmed().replace("\n", " "));
    return settings;
}
