#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <QByteArray>

// Макросы для генерации CRC16-CCITT
#define INIT 0x0000 /* Initial value */
#define POLINOMIAL 0x1021 /* Polynomial X16+X12+X5+1 */

class ByteArray : public QByteArray
{
	public:
		ByteArray();
		ByteArray(QByteArray);
		ByteArray(const char *);
		ByteArray(int, char);
		ByteArray(int);
		static QByteArray fromShortSwapped(unsigned short);
		unsigned short crcCcitt();
		QByteArray crcCcittBa();
		unsigned short toShort();

	private:
		unsigned short crcCcittAux(unsigned short, unsigned short);
};

#endif // BYTEARRAY_H
