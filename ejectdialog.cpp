#include "ejectdialog.h"
#include "ui_ejectdialog.h"
#include "config.h"

EjectDialog::EjectDialog(const JConfig &conf, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EjectDialog)
{
	ui->setupUi(this);
	ui->currencyLb->setText(conf["global"]["currency"].toString(DEF_CURRENCY));
	m_min = conf["eject"]["min"].toInt(EJECT_DEF_MIN);
	m_max = conf["eject"]["max"].toInt(EJECT_DEF_MAX);
	m_len = conf["eject"]["len"].toInt(EJECT_DEF_LEN);
	
	connect(ui->kp0Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp1Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp2Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp3Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp4Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp5Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp6Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp7Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp8Pb, SIGNAL(clicked()), SLOT(num()));
	connect(ui->kp9Pb, SIGNAL(clicked()), SLOT(num()));
}

EjectDialog::~EjectDialog()
{
	delete ui;
}

void EjectDialog::amountChanged()
{
	ui->amountLb->setText(m_amount);
	
	int amount = m_amount.toInt();
	ui->ejectPb->setEnabled((amount > m_min) && (amount < m_max) && (amount % m_min == 0));
}

void EjectDialog::open()
{
	refresh();
	showFullScreen();
}

void EjectDialog::on_kpCancelPb_clicked()
{
	m_amount = "0";
	amountChanged();
}

void EjectDialog::on_kpBsPb_clicked()
{
	int len = m_amount.size();
	if (len == 1)
		m_amount = "0";
	else
		m_amount.resize(len - 1);
	amountChanged();
}

void EjectDialog::num()
{
	if (m_amount.size() >= m_len)
		return;
	
	QString num = sender()->objectName().mid(2, 1);
	if (m_amount == "0")
		m_amount = num;
	else
		m_amount += num;
	amountChanged();
}

void EjectDialog::refresh()
{
	m_amount = "0";
	amountChanged();
	setEnabled(true);
}

void EjectDialog::on_ejectPb_clicked()
{
	setEnabled(false);
	emit eject(m_amount.toInt());
}
