#include "bytearray.h"

ByteArray::ByteArray() : QByteArray() {}

ByteArray::ByteArray(QByteArray src) : QByteArray(src) {}

ByteArray::ByteArray(const char *cstr) : QByteArray(cstr) {}

ByteArray::ByteArray(int size, char chr) : QByteArray(size, chr) {}

ByteArray::ByteArray(int chr) : QByteArray(1, (unsigned char)chr) {}

QByteArray ByteArray::fromShortSwapped(unsigned short num)
{
	unsigned short swp = (num << 8 | num >> 8);
	return QByteArray((const char *)&swp, 2);
}

unsigned short ByteArray::crcCcitt() const
{
	unsigned char *p = (unsigned char *)data();
	unsigned short n = size();
	unsigned char ch;
	unsigned short i;
	unsigned short crc = INIT;

	for (i = 0; i < n; i++)
	{
		ch = *p++;
		crc = crcCcittAux(crc,(unsigned short) ch);
	}
	return crc;
}

QByteArray ByteArray::crcCcittBa() const
{
	return fromShortSwapped(crcCcitt());
}

unsigned short ByteArray::toShort() const
{
	return toHex().toShort(0, 16);
}

unsigned short ByteArray::crcCcittAux(unsigned short crc, unsigned short ch) const
{
	unsigned short i;
	ch <<= 8;
	for (i = 8; i > 0; i--)
	{
		if ((ch ^ crc ) & 0x8000)
			crc = (crc << 1) ^ POLINOMIAL;
		else
			crc <<= 1;
		ch <<= 1;
	}
	return crc;
}
