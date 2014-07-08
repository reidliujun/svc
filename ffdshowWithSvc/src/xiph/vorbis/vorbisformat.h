#ifndef _VORBISFORMAT_H_
#define _VORBISFORMAT_H_

struct VORBISFORMAT
{
 WORD  nChannels;
 long  nSamplesPerSec;
 long  nMinBitsPerSec;
 long  nAvgBitsPerSec;
 long  nMaxBitsPerSec;
 float fQuality;
};

struct VORBISFORMAT2
{
 DWORD Channels;
 DWORD SamplesPerSec;
 DWORD BitsPerSample;
 DWORD HeaderSize[3]; // 0: Identification, 1: Comment, 2: Setup
};

struct VORBISFORMATILL
{
 unsigned long vorbisVersion;
 unsigned long samplesPerSec;
 unsigned long minBitsPerSec;
 unsigned long avgBitsPerSec;
 unsigned long maxBitsPerSec;
 unsigned char numChannels;
};

#pragma pack(push,2)
struct OGGWAVEFORMAT
{
 WAVEFORMATEX wfx;
 DWORD dwVorbisACMVersion;
 DWORD dwLibVorbisVersion;
 //BYTE byExtra[];
} __attribute__((packed));
typedef OGGWAVEFORMAT *LPOGGWAVEFORMAT;
#pragma pack(pop)

#define OGGWAVEFORMATMAXEXTRABYTES (64*1024-sizeof(WAVEFORMATEX)-2)

#endif
