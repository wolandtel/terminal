#include "command.h"

Command::Command(QObject *parent,
								 const ByteArray &cmd,
								 const ByteArray &data) :
	QObject(parent)
{
	m_next = NULL;
	m_code = (unsigned char)cmd[0];
	
	switch (m_code)
	{
		case RESP_DLE:
			m_atype = ATYPE_DLE;
			m_waitbytes = 2;
			break;
		case RESP_ACK:
		case RESP_NAK:
			m_atype = ATYPE_NONE;
			m_waitbytes = 0;
			break;
		case CMD_CARD:
			m_atype = ATYPE_ACK;
			m_waitbytes = 1;
			break;
		default:
			m_atype = ATYPE_FULL;
			m_waitbytes = 1;
	}
	
	connect(this, SIGNAL(send(ByteArray)), parent, SLOT(writeCmd(ByteArray)));
	
	if ((cmd.size() > 1) || (data.size() == 0))
	{
		m_cmd += cmd;
		m_param = -1;
		return;
	}
	
	m_param = (unsigned char)data[0];
	
	m_cmd += RESP_STX;
	m_cmd += ByteArray::fromShortSwapped(data.size() + 2);
	m_cmd += 'C';
	m_cmd += cmd;
	m_cmd += data;
	
	ByteArray cs = (ByteArray)m_cmd.crcCcittBa();
	m_cmd += cs;
}

void Command::send(int timeout)
{
	if (timeout > 0)
		startTimer(timeout);
	else
		emit send(m_cmd);
}

void Command::setNext(Command *next)
{
	m_next = next;
}

bool Command::atype(int atype)
{
	return atype & this->m_atype;
}

void Command::timerEvent(QTimerEvent *event)
{
	killTimer(event->timerId());
	emit send(m_cmd);
}
