#include <QApplication>
#include <math.h>
#include "printer.h"
#include "terminal.h"
#include "debug.h"

Printer::Printer(const JConfig &conf, Terminal *terminal)
{
	m_device = new QFile(conf["printer"]["device"].toString());
	if (!m_device->exists() || !m_device->open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Unbuffered))
	{
		delete m_device;
		m_device = NULL;
		return;
	}
	
	loadTpl(conf["printer"]["ejectTpl"].toString(PRN_DEF_EJECT_TPL), m_ejectTpl);
	loadTpl(conf["printer"]["paymentTpl"].toString(PRN_DEF_PAYMENT_TPL), m_paymentTpl);
	
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
	print(m_ejectTpl, amount * -1);
}

void Printer::payment(int amount)
{
	print(m_paymentTpl, amount);
}

void Printer::print(QString receipt, int amount)
{
	QString cardnumber = m_terminal->cardreader()->cardnum();
	QDateTime current = QDateTime::currentDateTime();
	
	receipt.replace("__DATE__", current.toString("dd.MM.yyyy"))
			.replace("__TIME__", current.toString("hh:mm:ss"))
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

void Printer::loadTpl(const QString &file, QString &tpl)
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
