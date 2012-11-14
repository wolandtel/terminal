#include "command.h"
#include "debug.h"

Command::Command(QObject *parent, const ByteArray &cmd, const ByteArray &data) :
	QObject(parent)
{
	i_cmd = cmd;
	i_data = data;
	
	init();
}

Command::Command(const Command &cmd) : 
	QObject(cmd.parent())
{
	i_cmd = cmd.cmd();
	i_data = cmd.data();
	
	init();
}

void Command::send(int timeout)
{
	if (timeout >= 0)
		m_timeout = timeout;
	
	if (m_timeout > 0)
		startTimer(m_timeout);
	else
		emit send(m_cmd);
}

void Command::setNext(Command *next)
{
	m_next = next;
}

void Command::setTimeout(int timeout)
{
	if (timeout >= 0)
		m_timeout = timeout;
}

bool Command::answerType(int answerType) const
{
	return answerType & this->m_answerType;
}

void Command::timerEvent(QTimerEvent *event)
{
	killTimer(event->timerId());
	emit send(m_cmd);
}

void Command::init()
{
	m_next = NULL;
	m_timeout = CMD_TM_WRITE;
	m_code = (unsigned char)i_cmd[0];
	
	switch (m_code)
	{
		case CMD_BYTE_DLE:
			m_answerType = AnswerDLE;
			m_waitbytes = 2;
			break;
		case CMD_BYTE_ACK:
		case CMD_BYTE_NAK:
			m_answerType = AnswerNone;
			m_waitbytes = 0;
			break;
		default: // CMD_CARD устанавливалось в AnswerACK для исключения ожидания полного ответа в рамках таймаута чтения символа
			m_answerType = AnswerFull;
			m_waitbytes = 1;
	}
	
	connect(this, SIGNAL(send(ByteArray)), parent(), SLOT(writeCmd(ByteArray)));
	
	if ((i_cmd.size() > 1) || (i_data.size() == 0))
	{
		m_cmd += i_cmd;
		m_param = -1;
		return;
	}
	
	m_param = (unsigned char)i_data[0];
	
	m_cmd += CMD_BYTE_STX;
	m_cmd += ByteArray::fromShortSwapped(i_data.size() + 2);
	m_cmd += 'C';
	m_cmd += i_cmd;
	m_cmd += i_data;
	
	ByteArray cs = (ByteArray)m_cmd.crcCcittBa();
	m_cmd += cs;
}
