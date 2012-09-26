#include <QDebug>
#include <QTimer>
#include "cardreader.h"

Cardreader::Cardreader(QObject *parent) :
	QObject(parent)
{
	m_state.initialized = false;
	m_lastCmd = m_curCmd = NULL;
	m_waitbytes = m_rtimer = 0;
	
	PortSettings settings;
	settings.BaudRate = BAUD38400;
	settings.Parity = PAR_EVEN;
	settings.StopBits = STOP_1;
	settings.DataBits = DATA_8;
	
	m_tty = new QextSerialPort("/dev/ttyS1", settings);
}

void Cardreader::timerEvent(QTimerEvent *event)
{
	int timerId = event->timerId();
	
	if (timerId == m_rtimer)
	{
#ifdef DEBUG
		qDebug() << QString::fromUtf8(" ⌚ Read character timeout");
#endif
		reset();
	}
	else
	{
#ifdef DEBUG
		qDebug() << QString::fromUtf8(" ⌚ Unknown timer event");
#endif
	}
	
	killTimer(timerId);
}

void Cardreader::init()
{
	if (!m_tty->open(QIODevice::ReadWrite | QIODevice::Text))
	{
		emit initFailed();
		return;
	}
	
	QTimer::singleShot(TM_OPEN, this, SLOT(initContinue()));
}

void Cardreader::initContinue()
{
	m_tty->flush();
	connect(m_tty, SIGNAL(readyRead()), this, SLOT(read()));
	sendCmd(CMD_INIT, "@001"); // Инициализация
}

void Cardreader::read()
{
	qDebug() << "Have data";
	while ((m_reading != R_NONE) && (m_tty->bytesAvailable() > 0))
	{
		int remains = m_waitbytes - m_rcvbuf.size();
		if (remains <= 0)
		{
			m_tty->flush();
			return;
		}
		
		QByteArray data = m_tty->read(remains);
		stopTimer(&m_rtimer, data.size() > 0);
		m_rcvbuf.append(data);
		
		if (m_rcvbuf.size() < m_waitbytes)
		{
			if (!m_rtimer)
				m_rtimer = startTimer(TM_READCHAR);
			return;
		}
		
		stopTimer(&m_rtimer);
		
#ifdef DEBUG
		qDebug() << "<< " << m_rcvbuf.toHex();
#endif
		
		handleMsg();
		m_rcvbuf.clear();
	}
}

void Cardreader::sendCmd(const ByteArray &cmd, const ByteArray &data, Command *replied)
{
	qDebug() << "cmd = " << cmd.toHex() << "; replied = " << (long int) replied << "; lastCmd = " << (long int) m_lastCmd; 
	Command *ncmd = new Command(this, cmd, data);
	if (!replied)
		replied = m_lastCmd;
	
	if (replied)
	{
		ncmd->setNext(replied->next());
		replied->setNext(ncmd);
	}
	else
		ncmd->send(TM_WCMD);
	
	if (replied == m_lastCmd)
		m_lastCmd = ncmd;
}

void Cardreader::sendCmd(const ByteArray &data, Command *replied)
{
	sendCmd(data, ByteArray(), replied);
}

void Cardreader::writeCmd(ByteArray cmd)
{
	m_curCmd = (Command *)sender();
	if (!m_curCmd)
		return;
	
	m_waitbytes = m_curCmd->waitBytes();
	m_reading = R_RESP;
	
#ifdef DEBUG
	qDebug() << ">> " << cmd.toHex();
#endif
	
	m_tty->write(cmd);
	
	if (!m_rtimer && m_waitbytes && !m_curCmd->atype(ATYPE_NONE))
		m_rtimer = startTimer(TM_READCHAR);
	
	handleCurCmd(ATYPE_NONE);
}

void Cardreader::handleMsg()
{
	switch (m_reading)
	{
		case R_NONE:
			break;
		case R_RESP:
		{
			switch ((unsigned char)m_rcvbuf[0])
			{
				case RESP_STX:
					m_rheader = m_rcvbuf;
					m_waitbytes = 2;
					m_reading = R_LEN;
					break;
				case RESP_ACK:
					switch (m_curCmd->code())
					{
						case CMD_CARD:
							if (!m_state.initialized)
							{
								m_state.initialized = true;
								emit initSucceeded();
							}
					}
					
					handleCurCmd(ATYPE_ACK);
					break;
				case RESP_DLE:
					if (m_curCmd)
						m_curCmd->send();
					break;
				case RESP_NAK:
					handleError(ERR_NAK);
					break;
			}
			break;
		}
		case R_LEN:
			m_rheader.append(m_rcvbuf);
			m_waitbytes = m_rcvbuf.toShort();
			m_reading = R_DATA;
			break;
		case R_DATA:
			m_rdata = m_rcvbuf;
			m_waitbytes = 2;
			m_reading = R_CRC;
			break;
		case R_CRC:
			m_waitbytes = 1;
			if (((ByteArray)(m_rheader + m_rdata)).crcCcittBa() != m_rcvbuf)
			{
				sendCmd(RESP_NAK, m_curCmd);
				handleError(ERR_CRC);
			}
			else
			{
				sendCmd(RESP_ACK, m_curCmd);
				m_reading = R_RESP;
				if (m_rdata[0] == 'P')
					handleResponse();
				else
					handleError(ERR_NEGATIVE);
			}
			break;
	}
}

void Cardreader::handleResponse()
{
	switch ((unsigned char)m_rdata[1])
	{
		case CMD_INIT:
	//		sendCmd(CMD_LED, "34");
			sendCmd(CMD_LED, "24");
			sendCmd(CMD_CARD, "100000000");
			break;
		case CMD_CARD:
			handleCard();
			break;
	}
	
	handleCurCmd(ATYPE_FULL);
}

void Cardreader::handleError(enum errcode errcode)
{
#ifdef DEBUG
	qDebug() << " ! handleError: " << errcode;
#endif
	
	switch ((unsigned char)m_rdata[1])
	{
		case CMD_INIT:
			emit initFailed();
			break;
		case CMD_CARD:
			handleCard();
			break;
	}
	
	handleCurCmd(-1);
}

void Cardreader::handleCurCmd(int atype)
{
	if (!(m_curCmd && m_curCmd->atype(atype)))
		return;
	
	Command *next = m_curCmd->next();
	
	delete m_curCmd;
	if (m_lastCmd == m_curCmd)
		m_lastCmd = m_curCmd = NULL;
	else
		m_curCmd = next;
	
	if (m_curCmd)
		m_curCmd->send(TM_WCMD);
}

void Cardreader::stopTimer(int *timer, bool condition)
{
	if (*timer && condition)
	{
		killTimer(*timer);
		*timer = 0;
	}
}

void Cardreader::reset()
{
	ByteArray rst = ByteArray(RESP_DLE).append(RESP_EOT);
	m_tty->flush();
	
#ifdef DEBUG
	qDebug() << ">> " << rst.toHex();
#endif
	
	m_reading = R_RESP;
	m_waitbytes = 2;
	m_tty->write(rst);
}

void Cardreader::handleCard()
{
	emit hasCard();
	sendCmd(CMD_CARD, "100000000");
	handleCurCmd(ATYPE_ACK);
}
