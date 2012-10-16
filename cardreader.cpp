#include <QDebug>
#include <QTimer>
#include "cardreader.h"
#include "debug.h"

Cardreader::Cardreader(const QString &tty, QObject *parent) :
	QObject(parent)
{
	m_state.initialized = false;
	m_lastCmd = m_curCmd = NULL;
	m_waitbytes = m_rtimer = 0;
	m_cardnum = "";
	
	PortSettings settings;
	settings.BaudRate = BAUD38400;
	settings.Parity = PAR_EVEN;
	settings.StopBits = STOP_1;
	settings.DataBits = DATA_8;
	
	nextMode(ReadResponse);
	
	m_tty = new QextSerialPort(tty, settings, QextSerialPort::EventDriven);
	connect(m_tty, SIGNAL(readyRead()), this, SLOT(read()));
}

Cardreader::~Cardreader()
{
	delete m_tty;
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
	if (!m_tty->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
	{
		emit initFailed();
		return;
	}
	
	QTimer::singleShot(CR_TM_OPEN, this, SLOT(initContinue()));
}

void Cardreader::initContinue()
{
	m_tty->flush();
	sendCmd(CMD_INIT, "@801"); // Инициализация
}

bool Cardreader::read(ByteArray &buffer)
{
	int size = m_rcvbuf.size();
	if (size < m_waitbytes)
	{
		if ((size > 0) && !m_rtimer)
			m_rtimer = startTimer(CR_TM_READCHAR);
		
		return false;
	}
	
	if (m_waitbytes > 0)
	{
		buffer = m_rcvbuf.left(m_waitbytes);
		m_rcvbuf.remove(0, m_waitbytes);
	}
	
	m_waitbytes = 0;
	
	return true;
}

void Cardreader::skip(int bytes)
{
	m_rcvbuf.remove(0, bytes);
}

void Cardreader::read()
{
///*DEBUG*/	debug("Have data");
	QByteArray data = m_tty->readAll();
	if (data.size() == 0)
		return;
	
	stopTimer(&m_rtimer);
	
	m_rcvbuf.append(data);
	
#ifdef DEBUG
	qDebug() << "<< " << m_rcvbuf.toHex();
#endif
	
	handleData();
}

void Cardreader::sendCmd(const ByteArray &cmd, const ByteArray &data, Command *replied)
{
///*DEBUG*/	qDebug() << "cmd = " << cmd.toHex() << "; replied = " << (long int) replied << "; lastCmd = " << (long int) m_lastCmd; 
	Command *ncmd = new Command(this, cmd, data);
	if (!replied)
		replied = m_lastCmd;
	
	if (replied)
	{
		ncmd->setNext(replied->next());
		replied->setNext(ncmd);
	}
	else
		ncmd->send(CR_TM_WCMD);
	
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
	
#ifdef DEBUG
	qDebug() << " i>> cmd = " << ByteArray(1, m_curCmd->code()).toHex()
					 << "; param = " << ByteArray(1, m_curCmd->param()).toHex();
	qDebug() << ">> " << cmd.toHex();
#endif
	
	m_tty->write(cmd);
	
	handleCurCmd(CMD_ATYPE_NONE);
}

void Cardreader::handleData()
{
	ByteArray block;
	while (read(block))
		handleMsg(block);
}

void Cardreader::nextMode(enum ReadMode mode, int bytes)
{
	switch (m_reading = mode)
	{
		case ReadResponse:
			m_waitbytes = 1;
			break;
		case ReadLength:
		case ReadCrc:
			m_waitbytes = 2;
			break;
		case ReadData:
			m_waitbytes = bytes;
			break;
		default:;
	}
}

void Cardreader::handleMsg(const ByteArray &block)
{
	switch (m_reading)
	{
		case ReadNone:
			return;
		case ReadResponse:
		{
			enum ReadMode mode = ReadResponse;
			switch ((unsigned char)block[0])
			{
				case CMD_RESP_STX:
					m_rheader = block;
					mode = ReadLength;
					break;
				case CMD_RESP_ACK:
					switch (m_curCmd->code())
					{
						case CMD_CARD:
							if (!m_state.initialized)
							{
								m_state.initialized = true;
								emit initSucceeded();
							}
					}
					handleCurCmd(CMD_ATYPE_ACK);
					break;
				case CMD_RESP_DLE:
					if (m_curCmd)
						m_curCmd->send();
					break;
				case CMD_RESP_NAK:
					handleError(ErrorNak);
					break;
				default:
					skip();
			}
			nextMode(mode);
			break;
		}
		case ReadLength:
			m_rheader.append(block);
			nextMode(ReadData, block.toShort());
			break;
		case ReadData:
			m_rdata = block;
			nextMode(ReadCrc);
			break;
		case ReadCrc:
			if (((ByteArray)(m_rheader + m_rdata)).crcCcittBa() != block)
			{
				sendCmd(CMD_RESP_NAK, m_curCmd);
				handleError(ErrorCrc);
			}
			else
			{
				sendCmd(CMD_RESP_ACK, m_curCmd);
				handleResponse(m_rdata[0] == 'P');
			}
			nextMode(ReadResponse);
			break;
	}
}

void Cardreader::handleResponse(bool positive)
{
	unsigned char cmd = *m_rdata.mid(1, 1).constData();
	unsigned char param = *m_rdata.mid(2, 1).constData();
	int errcode = 0;
	ByteArray st1 = ByteArray(1, 0xFF);
	ByteArray st0 = ByteArray(1, 0xFF);
	if (positive)
	{
		st1 = m_rdata.mid(3, 1);
		st0 = m_rdata.mid(4, 1);
	}
	else
		errcode = m_rdata.mid(3, 2).toInt();
	
#ifdef DEBUG
	qDebug() << " i<< cmd = " << ByteArray(1, cmd).toHex()
					 << "; param = " << ByteArray(1, param).toHex()
					 << "; st0 = " << st0.toHex()
					 << "; st1 = " << st1.toHex()
					 << "; errcode = " << errcode;
#endif
	
	switch (cmd)
	{
		case CMD_INIT:
			if (positive)
				ready();
			else
				emit initFailed();
			break;
		case CMD_CARD:
			if (!errcode || ((errcode >= 7) && (errcode <= 9)))
				handleCard(param, errcode);
			break;
		case CMD_MAGTRACK:
			if (positive)
			{
				emit cardInserted();
				sendCmd(CMD_LED, "20");
				m_cardnum = m_rdata.mid(5, CR_CARDNUM_LENGTH);
			}
			else
			{
				// 20 = card has not been read yet or other errors
				emit ejectCard(true);
			}
			break;
	}
	
	if (positive)
		handleCurCmd(CMD_ATYPE_FULL);
	else
		handleCurCmd(CMD_ATYPE_ALL);
}

void Cardreader::handleError(enum Error error)
{
#ifdef DEBUG
	qDebug() << " ! errcode: " << error;
#endif
	
	switch (error)
	{
		case ErrorNak:
		case ErrorCrc:
			break;
	}
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
		m_curCmd->send(CR_TM_WCMD);
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
	ByteArray rst = ByteArray(CMD_RESP_DLE).append(CMD_RESP_EOT);
	flush();
	
#ifdef DEBUG
	qDebug() << ">> " << rst.toHex();
#endif
	
	nextMode(ReadResponse);
	m_tty->write(rst);
}

void Cardreader::handleCard(unsigned char param, int errcode)
{
	switch (param)
	{
		case 0x31:
			switch (errcode)
			{
				case 8: // данные прочитаны
				case 0:
					sendCmd(CMD_MAGTRACK, 0x32); // Читать вторую дорожку | FIX: проверить, закрыта ли защёлка.
					break;
				case 7: // карточка вставлена не до конца
					ejectCard(true);
					break;
				case 9: // таймаут
				default:
					sendCmd(CMD_CARD, ByteArray("1").append(ByteArray(CR_TM_WAITCARD)));
			}
			break;
		case 0x32:
			switch (errcode)
			{
				case 0:
					emit cardEjected();
					ready();
					break;
				case 9: // таймаут
				default:
					sendCmd(CMD_CARD, ByteArray("2").append(ByteArray(CR_TM_EJECTCARD)));
			}
			break;
	}
	
	handleCurCmd(CMD_ATYPE_ACK);
}

void Cardreader::ready()
{
	sendCmd(CMD_LED, "24");
	sendCmd(CMD_SHUTTER, "0");
	sendCmd(CMD_CARD, ByteArray("1").append(ByteArray(CR_TM_WAITCARD)));
}

void Cardreader::ejectCard(bool err)
{
	emit cardEject(err);
	
	if (err)
		sendCmd(CMD_LED, "14");
	else
		sendCmd(CMD_LED, "30");
	
	sendCmd(CMD_SHUTTER, "0");
	sendCmd(CMD_CARD, ByteArray("2").append(ByteArray(CR_TM_EJECTCARD)));
}
