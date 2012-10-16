#include "pindialog.h"
#include "ui_pindialog.h"
#include "debug.h"

PinDialog::PinDialog(const JConfig &conf, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PinDialog)
{
	ui->setupUi(this);
#ifdef DEBUG
	setCursor(Qt::ArrowCursor);
#endif
	
	m_len = conf["pin"]["len"].toInt(PIN_DEF_LEN);
	
	QString echo = conf["pin"]["echo"].toString(QString(1, PIN_DEF_ECHO));
	if (echo.size() == 0)
		m_echo = PIN_DEF_ECHO;
	else
		m_echo = echo[0];
	
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

PinDialog::~PinDialog()
{
	delete ui;
}

void PinDialog::on_kpCancelPb_clicked()
{
	m_pin.clear();
	pinChanged();
}

void PinDialog::on_kpBsPb_clicked()
{
	int len = m_pin.size();
	if (len == 0)
		return;
	
	m_pin.resize(len - 1);
	pinChanged();
}

void PinDialog::on_okPb_clicked()
{
	setEnabled(false);
	emit gotPin();
}

void PinDialog::num()
{
	if (m_pin.size() >= m_len)
		return;
	
	m_pin += sender()->objectName().mid(2, 1);
	pinChanged();
}

void PinDialog::pinChanged()
{
	ui->pinLb->setText(QString(m_pin.size(), m_echo));
	ui->okPb->setEnabled(m_pin.size() == m_len);
}

void PinDialog::open()
{
	refresh();
	showFullScreen();
}

void PinDialog::refresh()
{
	m_pin.clear();
	pinChanged();
	setEnabled(true);
}
