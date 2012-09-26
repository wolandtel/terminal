#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include "qextserialport.h"
#include "bytearray.h"
#include "command.h"

// Таймауты (мс)
#define TM_WCMD 10
#define TM_READCHAR 250
#define TM_OPEN 800

class Cardreader : public QObject
{
		Q_OBJECT
	public:
		explicit Cardreader(QObject *parent = 0);
		void init();
		
	signals:
		void initFailed();
		void initSucceeded();
		void hasCard();
		
	public slots:
		
	protected:
		void timerEvent(QTimerEvent *event);
		
	private:
		struct
		{
			bool initialized;
		} m_state;
		
		enum
		{
			R_NONE,
			R_RESP,
			R_LEN,
			R_DATA,
			R_CRC
		} m_reading;
		
		enum errcode
		{
			ERR_NONE,
			ERR_NAK,
			ERR_CRC,
			ERR_NEGATIVE
		};
		
		QextSerialPort *m_tty;
		int m_waitbytes, m_rtimer;
		ByteArray m_rcvbuf, m_rheader, m_rdata;
		Command *m_lastCmd, *m_curCmd;
		
		void sendCmd(const ByteArray &, const ByteArray &, Command * = NULL);
		void sendCmd(const ByteArray &, Command * = NULL);
		void handleMsg();
		void handleResponse();
		void handleError(enum errcode);
		void handleCurCmd(int);
		void stopTimer(int *, bool = true);
		void reset();
		void handleCard();
		
	private slots:
		void initContinue();
		void read();
		void writeCmd(ByteArray);
};

#endif // CARDREADER_H
