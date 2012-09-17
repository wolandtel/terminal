#include <QDebug>
#include "cardreader.h"

Cardreader::Cardreader(QObject *parent) :
	QObject(parent)
{
	PortSettings settings;
	settings.BaudRate = BAUD38400;
	settings.Parity = PAR_EVEN;
	settings.StopBits = STOP_1;
	settings.DataBits = DATA_8;

	m_state = ST_INIT;
	m_tty = new QextSerialPort("/dev/ttyS1", settings);
}

bool Cardreader::init()
{
	if (!m_tty->open(QIODevice::ReadWrite | QIODevice::Text))
		return false;

	connect(m_tty, SIGNAL(readyRead()), this, SLOT(read()));
	sendCmd("C0@000"); // Инициализация

	return true;
}

void Cardreader::read()
{
	QByteArray data = m_tty->readAll();
#ifdef DEBUG
	qDebug() << "<< " << data.toHex();
#endif
}

unsigned short Cardreader::crc_aux(unsigned short crc, unsigned short ch)
{
	unsigned short i;
	ch <<= 8;
	for (i = 8; i > 0; i--)
	{
		if ((ch ^ crc ) & 0x8000)
			crc = (crc << 1) ^ POLINOMIAL;
		else
			crc <<= 1;
		ch <<= 1;
	}
	return crc;
}

unsigned short Cardreader::crc(unsigned char *p, unsigned short n)
{
	unsigned char ch;
	unsigned short i;
	unsigned short crc = INIT;

	for (i = 0; i < n; i++)
	{
		ch = *p++;
		crc = crc_aux(crc,(unsigned short)ch);
	}
	return crc;
}

QByteArray *Cardreader::mkCmd(const QByteArray &pcmd)
{
	QByteArray *cmd = new QByteArray();
	unsigned short length = pcmd.size();

	cmd->append(0xF2);
	cmd->append(length >> 8 & 0xFF);
	cmd->append(length & 0xFF);
	cmd->append(pcmd);

	unsigned short cs = crc((unsigned char *)cmd->data(), cmd->size());
	cmd->append(cs >> 8 & 0xFF);
	cmd->append(cs & 0xFF);

	return cmd;
}

void Cardreader::sendCmd(const QByteArray &pcmd)
{
	QByteArray *cmd = mkCmd(pcmd);
#ifdef DEBUG
	qDebug() << ">> " << cmd->toHex();
#endif
	m_tty->write(*cmd);
	delete cmd;
}
