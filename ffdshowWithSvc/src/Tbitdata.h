#ifndef _TLIBITDATA_H_
#define _TLIBITDATA_H_

#include "ffdebug.h"

#define swab32(x)\
	((((uint8_t*)&x)[0] << 24) | (((uint8_t*)&x)[1] << 16) |  \
		(((uint8_t*)&x)[2] << 8)  | (((uint8_t*)&x)[3]))

#define swable32(x)\
	((((uint32_t)x) >> 16) | (((uint32_t)x) << 16))

// Structure used for bytes parsing
class Tbitdata
{
public:
	int bitsleft;
	int bitindex;
	int bitsremaining;
	unsigned char *wordpointer;
	bool wordMode; /* 16/14 bits word format (1 -> 16, 0 -> 14) */
	bool bigEndian; /* endianness (1 -> be, 0 -> le) */
	uint32_t result;
	uint32_t currentWord;

	Tbitdata();
	virtual void copy(const Tbitdata *bitdataToCopy);
	virtual uint32_t getBits(int number_of_bits);
	void align();
	virtual uint32_t getBitsBackward(int number_of_bits);
	virtual uint32_t showBits(int number_of_bits);
	virtual uint32_t showBits(int number_of_bits, int bitsoffset);
	virtual uint32_t getBits2(int number_of_bits);
	virtual void bitstream_fill_current();
};
#endif