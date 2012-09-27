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
#define TM_WAITCARD "00060000" // 8 символов
#define TM_EJECTCARD "00060000" // 8 символов

#define CARDNUM_LENGTH 16

class Cardreader : public QObject
{
		Q_OBJECT
	public:
		explicit Cardreader(QObject *parent = 0);
		void init();
		
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
			ERR_NAK,
			ERR_CRC
		};
		
		QextSerialPort *m_tty;
		int m_waitbytes, m_rtimer;
		ByteArray m_rcvbuf, m_rheader, m_rdata;
		Command *m_lastCmd, *m_curCmd;
		QString m_cardnum;
		
		void sendCmd(const ByteArray &, const ByteArray &, Command * = NULL);
		void sendCmd(const ByteArray &, Command * = NULL);
		void handleMsg();
		void handleResponse(bool = true);
		void handleError(enum errcode);
		void handleCurCmd(int);
		void stopTimer(int *, bool = true);
		void reset();
		void handleCard(unsigned char, int);
		void ready();
		
	private slots:
		void initContinue();
		void read();
		void writeCmd(ByteArray);
		void ejectCard(bool = false);
};

#endif // CARDREADER_H
