#include <math.h>
#include "balancedialog.h"
#include "ui_balancedialog.h"

BalanceDialog::BalanceDialog(const QString &currency, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BalanceDialog)
{
	ui->setupUi(this);
	
	m_currency = currency;
	connect(ui->paymentPb, SIGNAL(clicked()), SIGNAL(payment()));
}

BalanceDialog::~BalanceDialog()
{
	delete ui;
}

void BalanceDialog::setBalance(double balance)
{
	m_balance = balance;
	ui->balanceLb->setText(QString::number(balance, 'f', 2) + " " + m_currency);
	if (balance <= 0)
		ui->ejectPb->setEnabled(false);
	else
		ui->ejectPb->setEnabled(true);
}

void BalanceDialog::open(double balance)
{
	setBalance(balance);
	showFullScreen();
}

void BalanceDialog::on_ejectPb_clicked()
{
	emit eject((int)floor(m_balance));
}
