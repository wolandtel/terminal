#include <iostream>
#include "ipc.h"
#include "debug.h"

Ipc::Ipc(const JConfig &conf, Ipc::Mode mode) :
	m_shared(conf["ipc"]["key"].toString(IPC_DEF_KEY)),
	m_timer(conf["ipc"]["timer"].toInt(IPC_DEF_TIMER)),
	m_timerId(0),
	m_timeout(conf["ipc"]["timeout"].toInt(IPC_DEF_TIMEOUT)),
	m_mode(mode),
	m_state(StateNone),
	m_error(ErrorNone),
	m_sentCmd(CmdNone)
{
#ifdef DEBUG
	dbg << " i: IPC key = " << m_shared.key() << "; mode = " << mode;
#endif
	switch (mode)
	{
		case ModeApplication:
			if (m_shared.create(IPC_SIZE))
			{
				if (!send(ResultOk) || !(m_timerId = startTimer(m_timer)))
				{
					m_shared.detach();
					m_error = ErrorOther;
				}
			}
			else
			{
				if (m_shared.error() == QSharedMemory::AlreadyExists)
					m_error = ErrorAlreadyExists;
				else
					m_error = ErrorShmem;
			}
			break;
		case ModeManager:
			if (!m_shared.attach())
			{
				if (m_shared.error() == QSharedMemory::NotFound)
					m_error = ErrorNotFound;
				else
					m_error = ErrorShmem;
			}
			break;
	}
#ifdef DEBUF
	dbg << " i: IPC error = " << m_error;
#endif
}

Ipc::~Ipc()
{
	disable();
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
		default:;
	}
	
	if (connected && !m_cmdConnected.contains(code))
		m_cmdConnected << code;
	
	return connected;
}

bool Ipc::connect(enum Result code, const QObject *reciever, const char *member)
{
	bool connected = false;
	
	switch (code)
	{
		case ResultOk:
			connected = QObject::connect(this, SIGNAL(ok(QByteArray)), reciever, member);
			break;
		case ResultNotImplemented:
			connected = QObject::connect(this, SIGNAL(notImplemented()), reciever, member);
			break;
		case ResultFail:
			connected = QObject::connect(this, SIGNAL(fail(QByteArray)), reciever, member);
			break;
		case ResultTimedOut:
			connected = QObject::connect(this, SIGNAL(timedOut()), reciever, member);
			break;
	}
	
	if (connected && !m_resultConnected.contains(code))
		m_resultConnected << code;
	
	return connected;
}

bool Ipc::cmd(enum Cmd code)
{
	if (m_mode != ModeManager)
		return false;
	
	if (send(code))
	{
		m_sentCmd = code;
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

void Ipc::disable()
{
	stopTimer();
	
	if (m_shared.isAttached())
		m_shared.detach();
}

void Ipc::timerEvent(QTimerEvent *event)
{
	QByteArray data;
	Message *msg;
	enum Cmd cmd;
	enum Result result;
	(void) event;
	
	if ((m_mode == ModeManager) && (m_cmdSent.msecsTo(QTime::currentTime()) > m_timeout)) // Проверка таймаута
	{
		stopTimer();
		
		if (m_resultConnected.contains(ResultTimedOut))
			emit timedOut(m_sentCmd);
		else
		{
			std::cout << "Command execution timed out" << std::endl;
			qApp->quit();
		}
		
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
#ifdef DEBUG
				dbg << " i: IPC got command = " << cmd << " [" << data.toHex() << "]";
#endif
				if (m_cmdConnected.contains(cmd))
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
							else
								send(ResultNotImplemented, true);
							break;
						case CmdStop:
							send(ResultOk, true);
							qApp->quit();
							break;
						case CmdPid:
							send(ResultOk, QByteArray::number(QApplication::applicationPid()), true);
							break;
						default:
							send(ResultNotImplemented, true);
					}
			}
			break;
		case ModeManager:
			result = (enum Result)msg->code;
			if (msg->sender == ModeApplication)
			{
#ifdef DEBUG
				dbg << " i: IPC got response = " << result << " [" << data.toHex() << "]";
#endif
				stopTimer();
				if (m_resultConnected.contains(result))
					notify(result, data);
				else
				{
					switch (result)
					{
						case ResultOk:
							switch (m_sentCmd)
							{
								case CmdPid:
									std::cout << data.data() << std::endl;
									break;
								case CmdStop:
									break;
								default:
									std::cout << "Command execution succeeded. Response dump: " << data.toHex().data() << std::endl;
									break;
							}
							break;
						case ResultNotImplemented:
							std::cout << "Command not implemented" << std::endl;
							break;
						case ResultFail:
							std::cout << "Error while executing command: " << data.data() << std::endl;
							break;
						default:;
					}
					qApp->quit();
				}
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
	Message *msg;
	unsigned short maxSize;
	
	if (!gLocked)
		if(!m_shared.lock())
			return false;
	
#ifdef DEBUG
	dbg << " i: IPC sent code = " << code << " [" << data.toHex() << "]";
#endif
	
	msg = (Message *)m_shared.data();
	msg->sender = (unsigned char)m_mode;
	msg->code = code;
	msg->dataSize = (unsigned short)data.size();
	
	maxSize = IPC_SIZE - sizeof(struct Message);
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
		default:;
	}
}

void Ipc::notify(enum Result code, QByteArray data)
{
	switch (code)
	{
		case ResultOk:
			emit ok(m_sentCmd, data);
			break;
		case ResultNotImplemented:
			emit notImplemented(m_sentCmd);
			break;
		case ResultFail:
			emit fail(m_sentCmd, data);
			break;
		default:;
	}
}
