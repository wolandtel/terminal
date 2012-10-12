#include "balancedialog.h"
#include "ui_balancedialog.h"

BalanceDialog::BalanceDialog(const QString &currency, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BalanceDialog)
{
	ui->setupUi(this);
	
	m_currency = currency;
	connect(ui->paymentPb, SIGNAL(clicked()), SIGNAL(payment()));
	connect(ui->ejectPb, SIGNAL(clicked()), SIGNAL(eject()));
}

BalanceDialog::~BalanceDialog()
{
	delete ui;
}

void BalanceDialog::setBalance(double balance)
{
	ui->balanceLb->setText(QString::number(balance) + " " + m_currency);
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
