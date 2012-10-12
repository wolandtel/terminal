#include "paymentdialog.h"
#include "ui_paymentdialog.h"

PaymentDialog::PaymentDialog(const QString &currency, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PaymentDialog)
{
	ui->setupUi(this);
	m_currency = currency;
}

PaymentDialog::~PaymentDialog()
{
	delete ui;
}

void PaymentDialog::open()
{
	ui->closePb->setEnabled(true);
	ui->payPb->setEnabled(false);
	ui->paidLb->setText("0 " + m_currency);
	m_paid = 0;
	showFullScreen();
}

void PaymentDialog::paid(int amount)
{
	if (m_paid == 0)
	{
		ui->closePb->setEnabled(false);
		ui->payPb->setEnabled(true);
	}
	m_paid += amount;
	ui->paidLb->setText(QString::number(m_paid) + " " + m_currency);
}

void PaymentDialog::accept()
{
	emit credit(m_paid);
}
