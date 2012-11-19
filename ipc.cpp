#include "ipc.h"

Ipc::Ipc(const JConfig &conf, Ipc::Mode mode) :
	m_shared(conf["ipc"]["key"].toString(IPC_DEF_KEY)),
	m_timer(conf["ipc"]["timer"].toInt(IPC_DEF_TIMER)),
	m_timerId(0),
	m_timeout(conf["ipc"]["timeout"].toInt(IPC_DEF_TIMEOUT)),
	m_mode(mode),
	m_state(StateNone)
{ // FIX: выставить коды ошибок
	switch (mode)
	{
		case ModeApplication:
			if (m_shared.create(IPC_SIZE))
			{
				if (!send(ResultOk) || !(m_timerId = startTimer(m_timer)))
				{
					m_shared.detach();
				}
			}
			else
			{
				if (m_shared.error() == QSharedMemory::AlreadyExists);
				else {}
			}
			break;
		case ModeManager:
			if (!m_shared.attach())
			{
				if (m_shared.error() == QSharedMemory::NotFound);
				else {}
			}
			break;
	}
}

Ipc::~Ipc()
{
	stopTimer();
	
	if (m_shared.isAttached())
		m_shared.detach();
}

bool Ipc::connect(enum Cmd code, const QObject *reciever, const char *member)
{
	bool connected = false;
	
	switch (code)
	{
		case CmdStatus:
			connected = QObject::connect(this, SIGNAL(status()), reciever, member);
			break;
		case CmdStop:
			connected = QObject::connect(this, SIGNAL(stop()), reciever, member);
			break;
		case CmdPid:
			connected = QObject::connect(this, SIGNAL(pid()), reciever, member);
			break;
		case CmdConfigRead:
			connected = QObject::connect(this, SIGNAL(configRead()), reciever, member);
			break;
		case CmdLogReopen:
			connected = QObject::connect(this, SIGNAL(logReopen()), reciever, member);
			break;
		case CmdStateSave:
			connected = QObject::connect(this, SIGNAL(stateSave()), reciever, member);
			break;
	}
	
	if (connected && !m_connected.contains(code))
		m_connected << code;
	
	return connected;
}

bool Ipc::cmd(enum Cmd code)
{
	if (m_mode != ModeManager)
		return false;
	
	if (send(code))
	{
		m_cmdSent = QTime::currentTime();
		return (m_timerId = startTimer(m_timer));
	}
	
	return false;
}

bool Ipc::reply(enum Result code, const QByteArray &data)
{
	if (m_mode != ModeApplication)
		return false;
	
	return send(code, data) && (m_timerId = startTimer(m_timer));
}

void Ipc::timerEvent(QTimerEvent *event)
{
	QByteArray data;
	Message *msg;
	enum Cmd cmd;
	(void) event;
	
	if ((m_mode == ModeManager) && (m_cmdSent.msecsTo(QTime::currentTime()) > m_timeout)) // Проверка таймаута
	{
		stopTimer();
		emit result(ResultTimedOut);
		return;
	}
	
	if (m_shared.lock())
	{
		msg = (Message *)m_shared.constData();
		if (msg->dataSize > 0)
			data.append((const char *)(msg + 1), msg->dataSize);
	}
	else
		return;
	
	switch (m_mode)
	{
		case ModeApplication:
			cmd = (enum Cmd)msg->code;
			if (msg->sender == ModeManager)
			{
				if (m_connected.contains(cmd))
				{
					stopTimer();
					notify(cmd);
				}
				else
					switch (cmd)
					{
						case CmdStatus:
							if (m_state != StateNone)
								send(ResultOk, QByteArray(1, m_state), true);
							break;
						case CmdPid:
							send(ResultOk, QByteArray::number(QCoreApplication::applicationPid()), true);
							break;
						default:
							send(ResultNotImplemented, true);
					}
			}
			break;
		case ModeManager:
			if (msg->sender == ModeApplication)
			{
				stopTimer();
				emit result((enum Result)msg->code, data);
			}
			break;
	}
	m_shared.unlock();
}

void Ipc::stopTimer()
{
	if (!m_timerId)
		return;
	
	killTimer(m_timerId);
	m_timerId = 0;
}

bool Ipc::send(unsigned code, const QByteArray &data, bool gLocked)
{
	bool locked = gLocked;
	Message *msg;
	unsigned short maxSize;
	
	if (!locked)
		if((locked = m_shared.lock()))
			return false;
	
	msg = (Message*)m_shared.data();
	msg->sender = (unsigned char)m_mode;
	msg->code = code;
	msg->dataSize = (unsigned short)data.size();
	
	maxSize = msg->dataSize = IPC_SIZE - sizeof(struct Message);
	if (msg->dataSize > maxSize)
		msg->dataSize = maxSize;
	memcpy(msg + 1, data.data(), msg->dataSize);
	
	if (!gLocked)
		m_shared.unlock();
	
	return true;
}

bool Ipc::send(unsigned int code, bool gLocked)
{
	return send(code, QByteArray(), gLocked);
}

void Ipc::notify(enum Cmd code)
{
	switch (code)
	{
		case CmdStatus:
			emit status();
			break;
		case CmdStop:
			emit stop();
			send(ResultOk, true);
			break;
		case CmdPid:
			break;
		case CmdConfigRead:
			emit configRead();
			break;
		case CmdLogReopen:
			emit logReopen();
			break;
		case CmdStateSave:
			emit stateSave();
			break;
	}
}
