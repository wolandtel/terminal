#include "ejectdialog.h"
#include "ui_ejectdialog.h"
#include "config.h"
#include "debug.h"

EjectDialog::EjectDialog(const JConfig &conf, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EjectDialog)
{
	ui->setupUi(this);
	ui->currencyLb->setText(conf["global"]["currency"].toString(DEF_CURRENCY));
	m_min = conf["dispenser"]["min"].toInt(EJECT_DEF_MIN);
	m_max = conf["dispenser"]["max"].toInt(EJECT_DEF_MAX);
	m_len = conf["dispenser"]["len"].toInt(EJECT_DEF_LEN);
	m_helperFile = conf["global"]["helper"].toPath(DEF_HELPER);
	m_helperParams << "dispenser"
					<< conf["dispenser"]["model"].toString()
					<< conf["dispenser"]["device"].toString();
	m_helperCassettes = conf["dispenser"]["cassettes"];
	m_helper = new QProcess();
///*DEBUG*/	m_helper->setProcessChannelMode(QProcess::ForwardedChannels);
	
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
	connect(m_helper, SIGNAL(readyReadStandardOutput()), SLOT(helperRead()));
}

EjectDialog::~EjectDialog()
{
	delete m_helper;
	delete ui;
}

void EjectDialog::amountChanged()
{
	ui->amountLb->setText(m_amount);
	
	int amount = m_amount.toInt();
//	ui->ejectPb->setEnabled((amount >= m_min) && (amount <= m_balance) && (amount % m_min == 0));
	ui->ejectPb->setEnabled((amount >= m_min) && (amount <= m_balance));
}

void EjectDialog::open(int balance)
{
	m_balance = (balance > m_max ? m_max : balance);
	refresh(m_balance);
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

void EjectDialog::refresh(int amount)
{
	m_amount = QString::number(amount);
	amountChanged();
	setEnabled(true);
}

void EjectDialog::on_ejectPb_clicked()
{
	if (m_amount.toInt() > m_balance) // дополнительная проверка запрашиваемой суммы перед выдачей
		return;
	
	setEnabled(false);
	QStringList params = m_helperParams;
	params << m_helperCassettes.encode()
			<< m_amount;
	m_helper->start(m_helperFile, params);
	if (!m_helper->waitForStarted())
		dbg << "! Ошибка выдачи наличных."; // FIX: заменить на запись в лог
	m_helper->waitForFinished();
	hide();
}

void EjectDialog::helperRead()
{
	QString input, event;
	Json data;
	int code;
	
	while (m_helper->canReadLine())
	{
		input = m_helper->readLine();
		data.parse(input);
		
#ifdef DEBUG
		dbg << "\t[ " << input << " ]: " << data.dump() << "(" + QString::number(data.error()) + ")";
#endif
		
		if (data.error() != Json::ErrorNone)
			continue;
		
		if ((code = data["code"].toInt()) == 0)
		{
			m_helperCassettes = data["cassettes"];
			emit eject(-1 * data["dispensed"].toInt());
		}
	}
}
