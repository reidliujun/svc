#include "stdafx.h"
#include "Tbitdata.h"

Tbitdata::Tbitdata()
{
	wordMode=true;
	bigEndian=true;
	currentWord=0;
	bitsremaining=0;
	bitindex=0;
}

void Tbitdata::copy(const Tbitdata *bitdataToCopy)
{
	wordMode=bitdataToCopy->wordMode;
	bigEndian=bitdataToCopy->bigEndian;
	bitindex=bitdataToCopy->bitindex;
	bitsremaining=bitdataToCopy->bitsremaining;
	bitsleft=bitdataToCopy->bitsleft;
	currentWord=bitdataToCopy->currentWord;
	wordpointer=bitdataToCopy->wordpointer;
}

// Does not work in non word mode
uint32_t Tbitdata::getBits(int number_of_bits)
{
  uint32_t rval=0;
  if((bitsleft-number_of_bits)<0) return 0;
  if(!number_of_bits) return 0;
  int bitsToRead=number_of_bits;

  while (bitsToRead>0)
  {
	  // Number of bits to read in the current byte starting from bitindex
	  int bits = (32-bitindex);
	  // Read the byte and keep the last "bits" bits
	  currentWord=(uint32_t)((wordpointer[0]<<24)+(wordpointer[1]<<16)+(wordpointer[2]<<8)+wordpointer[3]);


	  rval |= (currentWord & ((uint32_t)pow((double)2,(double)bits) - 1));

	  // We read too many bits. Remove extra right bits
	  if (bitsToRead<bits)
	  {
		  bitindex=(bitindex+bitsToRead)%32;
		  rval >>= (bits-bitsToRead);
		  bitsleft-=bitsToRead;
		  bitsToRead=0;
	  }
	  else // else shift to the left for the next byte
	  {
		  // bitindex = 0 now, next bytes are read completely
		  bitindex=0;
		  bitsToRead-=bits;
		  bitsleft-=bits;
		  if (bitsToRead>0)
			  rval <<= 32;
		  wordpointer+=4;
	  }
  }
  bitsremaining=32-bitindex;
  result=rval;
  return rval;
}

// Initialize the bytes alignment (use with non word mode)
void Tbitdata::align()
{
	bitsremaining=0;
	intptr_t align=(uintptr_t)wordpointer & 3;
	wordpointer=wordpointer-align;
	getBits2(8*align);
}

// Works only in word mode
uint32_t Tbitdata::getBitsBackward(int number_of_bits)
{
  Tbitdata backupBitData;

  if (bitindex>=number_of_bits) // Data to read is in the current byte
	  bitindex-=number_of_bits;
  else
  {
	  wordpointer-=(number_of_bits-bitindex)/32+4;
	  bitindex=32-(number_of_bits-bitindex)%32;	  
  }
  bitsremaining=32-bitindex;
  bitsleft+=number_of_bits;
  backupBitData.copy(this);
  return backupBitData.getBits(number_of_bits);
}

uint32_t Tbitdata::showBits(int number_of_bits)
{
	Tbitdata tmplData;
	tmplData.copy(this);
	return tmplData.getBits(number_of_bits);
}

uint32_t Tbitdata::showBits(int number_of_bits, int bitsoffset)
{
	Tbitdata tmplData;
	tmplData.copy(this);
	tmplData.getBits(bitsoffset);
	return tmplData.getBits(number_of_bits);
}

// Read the bits for non word mode. Also works with word mode
uint32_t Tbitdata::getBits2(int number_of_bits)
{
	uint32_t result=0;
	int step=wordMode?32:28;

	if (number_of_bits < bitsremaining) {
		result = (currentWord << (32 - bitsremaining))
					  >> (32 - number_of_bits);

		bitindex+=number_of_bits;
		bitsremaining-=number_of_bits;
		bitsleft-=number_of_bits;
		return result;
	}

	if (bitsremaining)
	{
		number_of_bits -= bitsremaining;
			result = ((currentWord << (32 - bitsremaining)) >>
			  (32 - bitsremaining));
	}

	if ( !wordMode && number_of_bits > 28 ) {
		bitstream_fill_current();
		result = (result << 28) | currentWord;
		number_of_bits -= 28;
	}

	bitstream_fill_current();

	if (wordMode)
	{
		if (number_of_bits != 0)
		result = (result << number_of_bits) |
				 (currentWord >> (32 - number_of_bits));
		bitindex=number_of_bits;
		bitsremaining=32-number_of_bits;
		bitsleft-=number_of_bits;
	}
	else
	{
		if (number_of_bits != 0)
		result = (result << number_of_bits) |
				 (currentWord >> (28 - number_of_bits));

		bitindex=number_of_bits;
		bitsremaining=28-number_of_bits;
		bitsleft-=number_of_bits;
	}
	return result;
}

void Tbitdata::bitstream_fill_current ()
{
	uint32_t tmp;

	tmp = *(uint32_t*)wordpointer;
	wordpointer+=4;
	bitsleft-=32;

	if (bigEndian)
		currentWord = swab32 (tmp);
	else
		currentWord = swable32 (tmp);

	if (!wordMode)
	{
		currentWord = (currentWord & 0x00003FFF) |
			((currentWord & 0x3FFF0000 ) >> 2);
	}
}
