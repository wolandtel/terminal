#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include "qextserialport.h"
#include "bytearray.h"
#include "command.h"

// Таймауты (мс)
#define CR_TM_READCHAR 250
#define CR_TM_OPEN 800
#define CR_TM_WAITCARD "00060000" // 8 символов
#define CR_TM_EJECTCARD "00060000" // 8 символов

#define CR_CARDNUM_LENGTH 16

class Cardreader : public QObject
{
		Q_OBJECT
	public:
		Cardreader(const QString &tty, QObject *parent = 0);
		~Cardreader();
		void init();
		inline QString cardnum() { return m_cardnum; }
		
	signals:
		void initFailed();
		void initSucceeded();
		void cardInserted();
		void cardEject(bool);
		void cardEjected();
		
	public slots:
		
	protected:
		void timerEvent(QTimerEvent *event);
		
	private:
		struct
		{
			bool initialized;
		} m_state;
		
		enum ReadMode
		{
			ReadNone,
			ReadResponse,
			ReadLength,
			ReadData,
			ReadCrc
		} m_reading;
		
		enum Error
		{
			ErrorNak,
			ErrorCrc
		};
		
		QextSerialPort *m_tty;
		int m_waitbytes, m_rtimer;
		ByteArray m_rcvbuf, m_rheader, m_rdata;
		Command *m_lastCmd, *m_curCmd;
		QString m_cardnum;
		
		bool read(ByteArray &buffer);
		void skip(int bytes = 1);
		inline void flush() { m_tty->flush(); m_rcvbuf.clear(); }
		void sendCmd(const ByteArray &cmd, const ByteArray &data, Command *prev = NULL);
		void sendCmd(const ByteArray &data, Command *prev = NULL);
		void queueCmd(Command *cmd, Command *prev);
		void repeatCmd(int timeout = CMD_TM_ERROR);
		void handleData();
		void nextMode(enum ReadMode mode, int bytes = 0);
		void handleMsg(const ByteArray &block);
		void handleResponse(bool positive = true);
		void handleError(enum Error error);
		void handleCurCmd(int answerType);
		void stopTimer(int *timer, bool condition = true);
		void reset();
		void handleCard(unsigned char param, int errcode);
		void ready();
		
	private slots:
		void initContinue();
		void read();
		void writeCmd(ByteArray cmd);
		void ejectCard(bool err = false);
		void ejectCardError();
};

#endif // CARDREADER_H
