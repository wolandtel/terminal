#include "paymentdialog.h"
#include "ui_paymentdialog.h"
#include "config.h"
#include "debug.h"

PaymentDialog::PaymentDialog(const JConfig &conf, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PaymentDialog)
{
	ui->setupUi(this);
	
	m_currency = conf["global"]["currency"].toString(DEF_CURRENCY);
	m_helperFile = conf["global"]["helper"].toString(DEF_HELPER);
	m_helperParams << "acceptor"
					<< conf["acceptor"]["device"].toString()
					<< conf["acceptor"]["baudrate"].toString(PD_ACCEPTOR_DEF_BAUDRATE)
					<< conf["acceptor"]["timeout"].toString(PD_ACCEPTOR_DEF_TIMEOUT);
	m_helper = new QProcess();
	m_helperState = HelperCreated;
	
	connect(m_helper, SIGNAL(readyReadStandardOutput()), SLOT(helperRead()));
	connect(m_helper, SIGNAL(started()), SLOT(helperStarted()));
	connect(m_helper, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(helperFinished()));
}

PaymentDialog::~PaymentDialog()
{
	delete m_helper;
	delete ui;
}

void PaymentDialog::open()
{
	ui->closePb->setEnabled(false);
	ui->payPb->setEnabled(false);
	ui->paidLb->setText("0 " + m_currency);
	m_paid = 0;
	m_helper->start(m_helperFile, m_helperParams);
	m_helperState = HelperStarting;
	if (!m_helper->waitForStarted(PD_HELPER_START_TIMEOUT))
	{
		close();
		return;
	}
}

void PaymentDialog::paid(int amount)
{
	if (m_paid == 0)
	{
		ui->closePb->setEnabled(false);
		ui->payPb->setEnabled(true);
	}
	m_paid = amount;
	ui->paidLb->setText(QString::number(m_paid) + " " + m_currency);
}

void PaymentDialog::close()
{
	HelperState state = m_helperState;
	
	if (state == HelperTerminating)
		return;
	
	m_helperState = HelperTerminating;
	
	if (state & (HelperStarting | HelperRunning))
	{
		ui->closePb->setEnabled(false);
		ui->payPb->setEnabled(false);
		
		m_helper->terminate();
		m_helper->waitForFinished(PD_HELPER_STOP_TIMEOUT);
	}
}

void PaymentDialog::helperRead()
{
	QString input, event;
	Json data;
	int code;
	
	while (m_helper->canReadLine())
	{
		input = m_helper->readLine();
		data.parse(input);
		
#ifdef DEBUG
		qDebug() << "\t[ " << input << " ]: " << data.dump() << "(" + QString::number(data.error()) + ")";
#endif
		
		if (data.error() != Json::ErrorNone)
			continue;
		
		if ((code = data["code"].toInt()) == 0)
		{
			if ((event = data["event"].toString()) == "received")
				paid(data["amount"].toInt());
			else if (event == "started")
				showFullScreen();
			else if (event == "finished")
			{
				emit credit(m_paid);
				hide();
				m_helperState = HelperTerminating;
			}
		}
		else
			close();
	}
}

void PaymentDialog::helperStarted()
{
	if (m_helperState == HelperRunning)
		return;
	
	m_helperState = HelperRunning;
	ui->closePb->setEnabled(true);
}

void PaymentDialog::helperFinished()
{
	close();
	m_helperState = HelperFinished;
}

