#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QTimerEvent>
#include "bytearray.h"

// Команды

#define CMD_INIT 0x30
#define CMD_CARD 0x31
#define CMD_MAGTRACK 0x32
#define CMD_LED 0x33
/*
#define CMD_ 0x34
#define CMD_ 0x35
#define CMD_ 0x36
#define CMD_ 0x
*/
#define CMD_SHUTTER 0x40
#define CMD_TIMER 0x57

// Ответы кардридера
#define RESP_STX 0xF2
#define RESP_ACK 0x06
#define RESP_DLE 0x10
#define RESP_EOT 0x04
#define RESP_NAK 0x15

// 
#define ATYPE_NONE 0x2
#define ATYPE_ACK 0x4
#define ATYPE_FULL 0x8
#define ATYPE_DLE 0x10
#define ATYPE_ALL 0xFF

// Смещения
#define CMD_CODE 4

class Command : public QObject
{
		Q_OBJECT
	public:
		Command(QObject *, const ByteArray &, const ByteArray &);
		inline unsigned char code() { return m_code; };
		inline unsigned char param() { return m_param; };
		void send(int timeout = 0);
		void setNext(Command *next);
		inline Command *next() { return m_next; };
		bool atype(int atype);
		inline int waitBytes () { return m_waitbytes; };
		
	signals:
		void send(ByteArray);
		
	public slots:
	
	protected:
		void timerEvent(QTimerEvent *event);
		
	private:
		unsigned char m_code;
		unsigned char m_param;
		ByteArray m_cmd;
		Command *m_next;
		int m_waitbytes;
		int m_atype;
};

#endif // COMMAND_H
