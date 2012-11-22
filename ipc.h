#ifndef IPC_H
#define IPC_H

/*
	Формат сообщения:
	
	1 : запрос/ответ				|
	4 : команда/результат			|
	2 : размер данных				|	минимальный размер сообщения = 7 байт
	x : данные
*/

#include <QApplication>
#include "jconfig.h"

#define IPC_DEF_KEY "terminal"
#define IPC_DEF_TIMEOUT 1000 // ms
#define IPC_DEF_TIMER 10 // ms

#define IPC_SIZE 256

class Ipc : public QObject
{
		Q_OBJECT
	public:
		enum Mode
		{
			ModeApplication,
			ModeManager
		};
		
		enum Cmd
		{
			CmdNone,
			CmdStatus,
			CmdStop,
			CmdPid,
			CmdConfigRead,
			CmdLogReopen,
			CmdStateSave
		};
		
		enum Result
		{
			ResultOk,
			ResultNotImplemented,
			ResultFail,
			ResultTimedOut
		};
		
		enum State
		{
			StateNone,
			StateStarting,
			StateRunning,
			StateError,
			StateStopping
		};
		
		enum Error
		{
			ErrorNone,
			ErrorAlreadyExists,
			ErrorNotFound,
			ErrorShmem,
			ErrorOther
		};
		
		struct Message
		{
			unsigned char sender;
			unsigned code;
			unsigned short dataSize;
		};
		
		Ipc(const JConfig &conf, enum Mode mode);
		~Ipc();
		
		bool connect(enum Cmd code, const QObject *reciever, const char *member);
		bool connect(enum Result code, const QObject *reciever, const char *member);
		
		bool cmd(enum Cmd code);
		bool reply(enum Result code, const QByteArray &data = QByteArray());
		
		inline enum Mode mode() { return m_mode; }
		inline enum Error error() { return m_error; }
	
	public slots:
		void disable();
	
	signals:
		// cmds
		void status();
		void stop();
		void configRead();
		void logReopen();
		void stateSave();
		// results
		void ok(enum Cmd, QByteArray data);
		void notImplemented(enum Cmd);
		void fail(enum Cmd, QByteArray errmsg);
		void timedOut(enum Cmd);
	
	protected:
		void timerEvent(QTimerEvent *event);
	
	private:
		QSharedMemory m_shared;
		int m_timer, m_timerId, m_timeout;
		enum Mode m_mode;
		QTime m_cmdSent;
		QList<enum Cmd> m_cmdConnected;
		QList<enum Result> m_resultConnected;
		enum State m_state;
		enum Error m_error;
		enum Cmd m_sentCmd;
		
		void stopTimer();
		bool send(unsigned int code, const QByteArray &data, bool gLocked = false);
		bool send(unsigned int code, bool gLocked = false);
		void notify(enum Cmd code);
		void notify(enum Result code, QByteArray data);
};

#endif // IPC_H
