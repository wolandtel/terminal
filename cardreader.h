#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include "qextserialport.h"
#include "bytearray.h"

// Ответы кардридера
#define RESP_STX 0xF2
#define RESP_ACK 0x06
#define RESP_DLE 0x10
#define RESP_EOT 0x04
#define RESP_NAK 0x15

/*
#define RESP_ 0x
#define RESP_ 0x
#define RESP_ 0x
#define RESP_ 0x
#define RESP_ 0x
*/

class Cardreader : public QObject
{
		Q_OBJECT
	public:
		explicit Cardreader(QObject *parent = 0);
		bool init();
		
	signals:
		
	public slots:
		
	private:
		enum
		{
			ST_NONE,
			ST_INIT,
			ST_CWAIT,
			ST_CREAD
		} m_state;

		enum
		{
			SS_RESP,
			SS_LEN,
			SS_DATA,
			SS_CRC
		} m_substate;

		QextSerialPort *m_tty;
		int m_waitbytes;
		ByteArray m_rcvbuf;
		ByteArray m_rheader;
		ByteArray m_rdata;

		ByteArray mkCmd(const QByteArray &);
		void sendCmd(const QByteArray &);
		void sendChr(const char);
		void handleMsg();

	private slots:
		void read();
};

#endif // CARDREADER_H
