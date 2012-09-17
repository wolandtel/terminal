#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include "qextserialport.h"

// Макросы для генерации CRC16-CCITT
#define INIT 0x0000 /* Initial value */
#define POLINOMIAL 0x1021 /* Polynomial X16+X12+X5+1 */

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
			ST_INIT,
			ST_CWAIT,
			ST_CREAD
		} m_state;

		QextSerialPort *m_tty;

		unsigned short crc(unsigned char *,unsigned short);
		unsigned short crc_aux(unsigned short, unsigned short);
		QByteArray *mkCmd(const QByteArray &);
		void sendCmd(const QByteArray &);

	private slots:
		void read();
};

#endif // CARDREADER_H
