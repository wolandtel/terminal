#include <QApplication>
#include <math.h>
#include "printer.h"
#include "terminal.h"
#include "debug.h"
#include "config.h"

Printer::Printer(const JConfig &conf, Terminal *terminal)
{
	m_device = new QFile(conf["printer"]["device"].toString());
	if (m_device->exists() && m_device->open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Unbuffered))
		m_device->flush();
	else
	{
		delete m_device;
		m_device = NULL;
		return;
	}
	
	m_formatDate = conf["global"]["format"]["date"].toString(DEF_FORMAT_DATE);
	m_formatTime = conf["global"]["format"]["time"].toString(DEF_FORMAT_TIME);
	tplLoad(conf["printer"]["tpl"]["eject"].toPath(PRN_DEF_TPL_EJECT), m_tplEject);
	tplLoad(conf["printer"]["tpl"]["payment"].toPath(PRN_DEF_TPL_PAYMENT), m_tplPayment);
	
	m_terminal = terminal;
	m_codec = QTextCodec::codecForName("IBM 866");
}

Printer::~Printer()
{
	if (m_device)
	{
		m_device->close();
		delete m_device;
	}
}

void Printer::ejected(int amount)
{
	print(m_tplEject, amount * -1);
}

void Printer::payment(int amount)
{
	print(m_tplPayment, amount);
}

void Printer::print(QString receipt, int amount)
{
	if (!m_device)
		return;
	
	QString cardnumber = m_terminal->cardreader()->cardnum();
	QDateTime current = QDateTime::currentDateTime();
	
	receipt.replace("__DATE__", current.toString(m_formatDate))
			.replace("__TIME__", current.toString(m_formatTime))
			.replace("__TERMINAL_ID__", m_terminal->id())
			.replace("__CARDNUMBER__", cardnumber.replace(4, cardnumber.size() - 8, " [...] "))
			.replace("__AMOUNT__", QString::number(amount))
			.replace("__BALANCE__", QString::number((int)floor(m_terminal->balance())));
	
#ifdef DEBUG
	dbg << "\n" << receipt;
#endif
	QTextCodec::setCodecForCStrings(m_codec);
	m_device->write(receipt.toAscii());
	m_device->write("\f\n");
	m_device->flush();
}

void Printer::tplLoad(const QString &file, QString &tpl)
{
	QFile tplFile(file);
	if (!tplFile.exists() || !tplFile.open(QIODevice::ReadOnly))
	{
		tpl = "";
		return;
	}
	
	tpl = QString::fromUtf8(tplFile.readAll().data());
	tplFile.close();
}
