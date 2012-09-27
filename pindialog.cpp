#include "pindialog.h"
#include "ui_pindialog.h"

PinDialog::PinDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PinDialog)
{
	ui->setupUi(this);
#ifdef DEBUG
	setCursor(Qt::ArrowCursor);
#else
	ui->pinLe->setCursor(Qt::BlankCursor);
#endif
}

PinDialog::~PinDialog()
{
	delete ui;
}

void PinDialog::on_kpBsPb_clicked()
{
		ui->pinLe->backspace();
}

void PinDialog::on_kp0Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "0");
}

void PinDialog::on_kp1Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "1");
}

void PinDialog::on_kp2Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "2");
}

void PinDialog::on_kp3Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "3");
}

void PinDialog::on_kp4Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "4");
}

void PinDialog::on_kp5Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "5");
}

void PinDialog::on_kp6Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "6");
}

void PinDialog::on_kp7Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "7");
}

void PinDialog::on_kp8Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "8");
}

void PinDialog::on_kp9Pb_clicked()
{
	ui->pinLe->setText(ui->pinLe->text() + "9");
}

void PinDialog::open()
{
	ui->pinLe->clear();
	showFullScreen();
}

void PinDialog::on_pinLe_textChanged(const QString &arg1)
{
	if (arg1.size() == ui->pinLe->maxLength())
		ui->OkPb->setEnabled(true);
	else
		ui->OkPb->setEnabled(false);
}
