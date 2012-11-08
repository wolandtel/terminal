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
#define CMD_RESP_STX 0xF2
#define CMD_RESP_ACK 0x06
#define CMD_RESP_DLE 0x10
#define CMD_RESP_EOT 0x04
#define CMD_RESP_NAK 0x15

// 
#define CMD_ATYPE_NONE 0x2
#define CMD_ATYPE_ACK 0x4
#define CMD_ATYPE_FULL 0x8
#define CMD_ATYPE_DLE 0x10
#define CMD_ATYPE_ALL 0xFF

// Смещения
#define CMD_OFFSET_CODE 4

// Таймауты (мс)
#define CMD_TM_WRITE 10
#define CMD_TM_ERROR 1000

class Command : public QObject
{
		Q_OBJECT
	public:
		Command(QObject *parent, const ByteArray &cmd, const ByteArray &data);
		Command(const Command &cmd);
		inline unsigned char code() const { return m_code; }
		inline unsigned char param() const { return m_param; }
		void send(int timeout = -1);
		void setNext(Command *next);
		void setTimeout(int timeout);
		inline Command *next() const { return m_next; }
		bool atype(int atype) const;
		inline int waitBytes () const { return m_waitbytes; }
		inline const ByteArray &cmd() const { return i_cmd; }
		inline const ByteArray &data() const { return i_data; }
		
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
		int m_timeout;
		ByteArray i_cmd;
		ByteArray i_data;
		
		void init();
};

#endif // COMMAND_H
