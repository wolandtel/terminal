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

	m_waitbytes = 0;
	m_state = ST_NONE;
	m_tty = new QextSerialPort("/dev/ttyS1", settings);
}

bool Cardreader::init()
{
	if (!m_tty->open(QIODevice::ReadWrite | QIODevice::Text))
		return false;

	connect(m_tty, SIGNAL(readyRead()), this, SLOT(read()));
	m_state = ST_INIT;
	sendCmd("C0@000"); // Инициализация

	return true;
}

void Cardreader::read()
{
	while ((m_state != ST_NONE) && (m_tty->bytesAvailable() > 0))
	{
		int remains = m_waitbytes - m_rcvbuf.size();
		if (remains <= 0)
		{
			m_tty->flush();
			return;
		}

		QByteArray data = m_tty->read(remains);
		m_rcvbuf.append(data);

		if (m_rcvbuf.size() < m_waitbytes)
			return;

#ifdef DEBUG
		qDebug() << "<< " << m_rcvbuf.toHex();
#endif

		handleMsg();
		m_rcvbuf.clear();
	}
}

ByteArray Cardreader::mkCmd(const QByteArray &data)
{
	ByteArray cmd = (ByteArray)QByteArray(1, RESP_STX);
	unsigned short length = data.size();

	cmd += ByteArray::fromShortSwapped(length);
	cmd += data;

	ByteArray cs = cmd.crcCcittBa();
	cmd += cs;

	return cmd;
}

void Cardreader::sendCmd(const QByteArray &data)
{
	ByteArray cmd = mkCmd(data);
#ifdef DEBUG
	qDebug() << ">> " << cmd.toHex();
#endif
	m_substate = SS_RESP;
	m_waitbytes = 1;
	m_tty->write(cmd);
}

void Cardreader::sendChr(const char chr)
{
	QByteArray buf(1, chr);
#ifdef DEBUG
	qDebug() << ">> " << buf.toHex();
#endif
	m_tty->write(buf);
}

void Cardreader::handleMsg()
{
	switch (m_substate)
	{
		case SS_RESP:
		{
			switch ((unsigned char)m_rcvbuf.data()[0])
			{
				case RESP_STX:
					m_rheader = m_rcvbuf;
					m_waitbytes = 2;
					m_substate = SS_LEN;
					break;
				case RESP_ACK:
					break;
				case RESP_DLE:
				case RESP_EOT:
					break;
				case RESP_NAK:
					/* FIX: Выставить флаг ошибки */
					break;
			}
			break;
		}
		case SS_LEN:
			m_rheader.append(m_rcvbuf);
			m_waitbytes = m_rcvbuf.toShort();
			m_substate = SS_DATA;
			break;
		case SS_DATA:
			m_rdata = m_rcvbuf;
			m_waitbytes = 2;
			m_substate = SS_CRC;
			break;
		case SS_CRC:
			if (((ByteArray)(m_rheader + m_rdata)).crcCcittBa() != m_rcvbuf)
			{
				sendChr(RESP_NAK);
				/* FIX: Выставить флаг ошибки */
			}
			else
			{
				sendChr(RESP_ACK);
				m_substate = SS_RESP;
				/* FIX: Выставить флаг готовности */
			}
			break;
	}
}
