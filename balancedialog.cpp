#include "balancedialog.h"
#include "ui_balancedialog.h"

BalanceDialog::BalanceDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BalanceDialog)
{
	ui->setupUi(this);
	
	m_currency = QString::fromUtf8("руб.");
}

BalanceDialog::~BalanceDialog()
{
	delete ui;
}

void BalanceDialog::on_paymentPb_clicked()
{
	emit payment();
	accept();
}

void BalanceDialog::on_ejectPb_clicked()
{
	emit eject();
	accept();
}

void BalanceDialog::open(double balance)
{
	ui->balanceLb->setText(QString::number(balance) + " " + m_currency);
	if (balance <= 0)
		ui->ejectPb->setEnabled(false);
	else
		ui->ejectPb->setEnabled(true);
	showFullScreen();
}
