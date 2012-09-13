#include "pindialog.h"
#include "ui_pindialog.h"

PinDialog::PinDialog(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PinDialog)
{
	ui->setupUi(this);
}

PinDialog::~PinDialog()
{
	delete ui;
}
