#include "stdafx.h"
#include "TaudioParser.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "dsutil.h"
#include "ffdshow_mediaguids.h"
#include "ffdebug.h"
#include "Tpresets.h"

TaudioParser::TaudioParser(IffdshowBase *Ideci,IdecAudioSink *Isink):
 deci(Ideci),
 deciA(Ideci),
 sinkA(Isink)
{
	deci->getGlobalSettings((TglobalSettingsBase **)&globalSettings);
	init();
}


void TaudioParser::init(void)
{
	streamformat=UNDEFINED;channels=0;bit_rate=0;sample_rate=0;nbFormatChanges=0;
	codecId=CODEC_ID_NONE;
	wFormatTag=0;sample_format=0;
	includeBytes=0;skipBytes=0;
	hasMLPFrames=false;
	searchSync=true;
}



TaudioParser::~TaudioParser()
{
}


// For debug
#if 0
void TaudioParser::printbitssimple(uint32_t n) {
    	uint32_t i;
		ffstring string;
     	i = 1<<(sizeof(n) * 8 - 1);
     
     	while (i > 0) {
    		if (n & i)
				string.append(_l("1"));
    		else
    			string.append(_l("0"));
    		i >>= 1;
    	}
		DPRINTF(string.c_str());
    }
#endif

/* This method is used by other decoders to parse AC3/DTS streams
 Returns the stream format and parsed buffer
 */
CodecID TaudioParser::parseStream(unsigned char *src, int size, 
											TbyteBuffer *newsrcBuffer)
{
#if 0
 // DEBUG 1 : dump input buffer
 /*char_t string[10000] = _l("");
 int pos=0;
 Tbitdata bitdata;
 bitdata.bitindex=0;
 bitdata.bitsleft=size*8;
 bitdata.wordpointer=src;

 while(bitdata.bitsleft >= 4)
 {
	 pos+=tsprintf(&string[pos], _l("%01X"), bitdata.getBits(4));
	 if (pos%992==0)
	 {
		 DPRINTF(string);
		 strcpy(string, _l(""));
		 pos=0;
	 }

 }
 DPRINTF(string);*/

 //DEBUG 2 : dump parsed stream to file
	if (codecId==CODEC_ID_MLP)
	{
		if (!dumpfile)
			dumpfile=fopen(_l("c:\\temp\\dump.thd"),_l("wb"));
		if (dumpfile)
			fwrite(src,sizeof(uint8_t),size,dumpfile);
	}
#endif
	if (codecId==CODEC_ID_NONE)
	sinkA->getCodecId(&codecId);

 // For DTS streams, DTS-HD blocks must be removed, libavcodec does not know how to handle them yet
 if ((codecId==CODEC_ID_DTS || codecId==CODEC_ID_LIBDTS || codecId == CODEC_ID_SPDIF_DTS 
	 || codecId==CODEC_ID_PCM) && size >0)
 {
	parseDTS(src, size, newsrcBuffer);
	return getCodecIdFromStream();
 }
 else if ((codecId==CODEC_ID_AC3 || codecId==CODEC_ID_EAC3 || codecId==CODEC_ID_MLP 
	 || codecId == CODEC_ID_LIBA52 || codecId==CODEC_ID_SPDIF_AC3)
	 && size >0)
 {
	parseAC3(src, size, newsrcBuffer);
	return getCodecIdFromStream();
 }
 return codecId;
}

CodecID TaudioParser::getCodecIdFromStream()
{
    CodecID codecIDAC3 = globalSettings->getCodecId(WAVE_FORMAT_AC3_W,NULL);
    useAC3CoreOnly = (codecIDAC3 != CODEC_ID_NONE)
        && (globalSettings->getCodecId(WAVE_FORMAT_MLP,NULL)==CODEC_ID_NONE);

	useAC3Passthrough = (codecIDAC3 != CODEC_ID_NONE) && (deci->getParam2(IDFF_aoutpassthroughAC3) == 1);

	CodecID codecIDDTS = globalSettings->getCodecId(WAVE_FORMAT_DTS_W,NULL);
	useDTSPassthrough = (codecIDDTS != CODEC_ID_NONE) && (deci->getParam2(IDFF_aoutpassthroughDTS) == 1);
	// Don't allow more than 1 format change
	if (nbFormatChanges==2)
		return codecId;

	wFormatTag=0;

	switch (streamformat)
	{
	case REGULAR_AC3:
		wFormatTag=WAVE_FORMAT_AC3_W;
		if (useAC3Passthrough)
		{
			codecId=CODEC_ID_SPDIF_AC3;nbFormatChanges++;
			return codecId;
		}
		break;
	case EAC3:wFormatTag=WAVE_FORMAT_EAC3;
		// TODO : if EAC3 decoder disabled, find a compatible EAC3 decoder and pull FFDShow Audio out of the graph
		// Problem : no EAC3 mediaguid exist
		break;
	case MLP:case TRUEHD:wFormatTag=WAVE_FORMAT_MLP;
		// TODO : if MLP decoder disabled, find a compatible MLP decoder and pull FFDShow Audio out of the graph
		// Problem : no MLP mediaguid exist
		break;
	case AC3_TRUEHD:
		// If AC3 codec is set to SPDIF and MLP decoder disabled,
		// then send AC3 frams in passthrough and throw away TrueHD frames
		if (useAC3CoreOnly)
		{
			if (useAC3Passthrough)
			{
				codecId=CODEC_ID_SPDIF_AC3;nbFormatChanges++;
				return codecId;
			}
			else //MLP decoder is disabled and AC3 pass-through is disabled
			{
				// TODO : find a compatible MLP decoder and pull FFDShow Audio out of the graph
				// Problem : no MLP mediaguid exist
				wFormatTag=WAVE_FORMAT_AC3_W;
			}
		}
		else
		{
			wFormatTag=WAVE_FORMAT_MLP;
		}
		break;
	case DTS:
	case DTS_HD:
		// If DTS Pass-through is enabled, then send DTS frames (or DTS core frames for DTS-HD stream)
        // in passthrough (DTS-HD frames are thrown away for DTS-HD stream)
		if (useDTSPassthrough)
		{
			codecId=CODEC_ID_SPDIF_DTS;nbFormatChanges++;
			return codecId;
		}
		wFormatTag=WAVE_FORMAT_DTS_W;
		break;
	case UNDEFINED:return CODEC_ID_NONE;
	default:break;
	}
	nbFormatChanges++;
	codecId=globalSettings->getCodecId(wFormatTag, NULL);
	return codecId;
}

void TaudioParser::NewSegment(void)
{
	// A new segment has arrived (occurs when there is some skipping or a change of stream) :
	// Solution 1 : reset the parser to detect the stream format again. This could result in codec switch
	//init();

	// Solution 2: reset only the context
	channels=0;bit_rate=0;sample_rate=0;nbFormatChanges=0;
	sample_format=0;
	includeBytes=0;skipBytes=0;searchSync=true;
}

HRESULT TaudioParser::parseDTS(unsigned char *src, int size, TbyteBuffer *newsrcBuffer)
{

   TbyteBuffer tmpBuffer;

   // For DTS streams, DTS-HD blocks must be removed, libavcodec does not know how to handle them yet
   Tbitdata bitdata;
   bitdata.bitsleft=size*8;
   bitdata.wordpointer=src;
   // Include the remaining bytes from the previous frame
   if (!backupbuf.empty())
   {
	   tmpBuffer.clear();
	   tmpBuffer.append(&*backupbuf.begin(), backupbuf.size());
	   tmpBuffer.append(src, size);
	   bitdata.bitsleft=tmpBuffer.size()*8;
	   bitdata.wordpointer=&*tmpBuffer.begin();
	   backupbuf.clear();
   }
   
   int dtsHDBlockSize = 0;

   if (includeBytes > 0)
   {
	   if (bitdata.bitsleft<includeBytes*8)
	   {
		   includeBytes-=bitdata.bitsleft/8;
		   newsrcBuffer->append(bitdata.wordpointer, bitdata.bitsleft/8);
		   return S_OK;
	   }
	   newsrcBuffer->append(bitdata.wordpointer, includeBytes);
	   bitdata.bitsleft-=includeBytes*8;
	   bitdata.wordpointer+=includeBytes;
	   includeBytes=0;
   }

   if (skipBytes > 0)
   {
	   if (bitdata.bitsleft<skipBytes*8)
	   {
		   skipBytes-=bitdata.bitsleft/8;
		   return S_OK;
	   }
	   bitdata.bitsleft-=skipBytes*8;
	   bitdata.wordpointer+=skipBytes;		   
	   skipBytes=0;
   }

   while (bitdata.bitsleft > 64) // 64 : necessary size to have the DTS block header
   {
	   if (bitdata.showBits(32) == 0x64582025) //DTS-HD block
	   {
		   streamformat=DTS_HD;
		   // Save the start position and left length of the DTS HD block
		   unsigned char *backuppointer=bitdata.wordpointer;
		   int backupBitsLeft=bitdata.bitsleft;

		   bitdata.getBits(32);
		   bitdata.getBits(8);
		   bitdata.getBits(2);
		   bool blownupHeader = bitdata.getBits(1) == 1;
		   dtsHDBlockSize=0;
		   if (blownupHeader)
		   {
			   bitdata.getBits(12);
			   dtsHDBlockSize = bitdata.getBits(20) +1;
		   }
		   else
		   {
			   bitdata.getBits(8);
			   dtsHDBlockSize = bitdata.getBits(16) +1;
		   }
		   bitdata.bitindex=0;
		   bitdata.bitsleft=backupBitsLeft-dtsHDBlockSize*8;
		   if (bitdata.bitsleft<0)
		   {
			   skipBytes=-bitdata.bitsleft/8;
			   bitdata.bitsleft=0;
			   //bitdata.wordpointer+=skipStartBitsLeft/8; // EOF
		   }
		   else
			bitdata.wordpointer=backuppointer+dtsHDBlockSize;
	   }
	   else if ( // DTS block
		   /* 14 bits and little endian bitstream */
		   (bitdata.showBits(32) == 0xFF1F00E8 
		       && (bitdata.wordpointer[4] & 0xf0) == 0xf0 && bitdata.wordpointer[5] == 0x07) ||
		   /* 14 bits and big endian bitstream */
		   (bitdata.showBits(32) == 0x1FFFE800
		       && bitdata.wordpointer[4] == 0x07 && (bitdata.wordpointer[5] & 0xf0) == 0xf0) ||
		   /* 16 bits and little endian bitstream */
		    bitdata.showBits(32) == 0xFE7F0180 ||
		   /* 16 bits and big endian bitstream */
		    bitdata.showBits(32) == 0x7FFE8001) 
	   {
		   if (streamformat==UNDEFINED)
			   streamformat=DTS;
		   unsigned char *backuppointer=bitdata.wordpointer;
		   int backupBitsLeft=bitdata.bitsleft;
		   
		   bool bigendianMode=(bitdata.wordpointer[0]==0x1F || bitdata.wordpointer[0]==0x7F);
		   // word mode for 16 bits stream
		   bool wordMode=(bitdata.wordpointer[0]==0xFE || bitdata.wordpointer[0]==0x7F);
		   bitdata.bigEndian=bigendianMode;
		   bitdata.wordMode=wordMode;
		   bitdata.align();

		   bitdata.getBits2(32); /* Sync code */   
		   bitdata.getBits2(1); /* Frame type */
		   bitdata.getBits2(5); /* Samples deficit */
		   int crcpresent = bitdata.getBits2(1); /* CRC present */
		   int sample_blocks=(bitdata.getBits2(7)+1)/8; /* sample blocks */
		   int frame_size=bitdata.getBits2(14) + 1;

		   int amode = bitdata.getBits2(6);
		   
           sample_rate = dca_sample_rates[bitdata.getBits2(4)];
           bit_rate = dca_bit_rates[bitdata.getBits2(5)];
		   

		   bitdata.getBits2(10);
		   int lfe=!!bitdata.getBits2(2);
		   bitdata.getBits2(1);

		   uint32_t header_crc=0;
		   if (crcpresent)
			   header_crc=bitdata.getBits2(16);

		   bitdata.getBits2(16);
		   int output=amode;
		   if (lfe)
			   output |= 0x80; //DCA LFE

		   int subframes=bitdata.getBits2(4)+1;
		   int primchannels=bitdata.getBits2(3)+1;
		   int ffmpegchannels=primchannels;
		   if (ffmpegchannels>5)
			   ffmpegchannels=5;

		   channels=primchannels+lfe;
		   
		   int datasize=(sample_blocks / 8) * 256 * sizeof(int16_t) * channels;

		   bitdata.wordpointer=backuppointer;
		   bitdata.bitsleft=backupBitsLeft;
		   bitdata.bitindex=0;

		   if (!wordMode)
			   frame_size = frame_size * 8 / 14 * 2;

		   // DTS frame not complete in this buffer. Backup it for next pass
		   if (frame_size>bitdata.bitsleft/8)
		   {
			  backupbuf.append(bitdata.wordpointer, bitdata.bitsleft/8);
			  bitdata.bitsleft=0;
		   }
		   else // store the block into the source buffer
		   {
			   newsrcBuffer->append(bitdata.wordpointer, frame_size);
			   bitdata.bitsleft-=frame_size*8;
			   bitdata.wordpointer+=frame_size;
		   }
	   }
	   else // Ignore the byte
	   {
		   bitdata.bitsleft -= 8;
		   bitdata.wordpointer++;
	   }
   }

   // Copy remaining bytes into a backup buffer for next frame
   if (bitdata.bitsleft>0)
   {
	   backupbuf.clear();
	   backupbuf.append(bitdata.wordpointer, bitdata.bitsleft/8);
   }
   return S_OK;
}



HRESULT TaudioParser::parseAC3(unsigned char *src, int size, TbyteBuffer *newsrcBuffer)
{

   TbyteBuffer tmpBuffer;
   Tbitdata bitdata;
   bitdata.bitindex=0;
   bitdata.bitsleft=size*8;
   bitdata.wordpointer=src;

   // Include the remaining bytes from the previous frame
   if (!backupbuf.empty())
   {
	   tmpBuffer.clear();
	   tmpBuffer.append(&*backupbuf.begin(), backupbuf.size());
	   tmpBuffer.append(src, size);
	   bitdata.bitsleft=tmpBuffer.size()*8;
	   bitdata.wordpointer=&*tmpBuffer.begin();
	   backupbuf.clear();
   }
   
   if (includeBytes > 0)
   {
	   if (bitdata.bitsleft<includeBytes*8)
	   {
		   includeBytes-=bitdata.bitsleft/8;
		   newsrcBuffer->append(bitdata.wordpointer, bitdata.bitsleft/8);
		   return S_OK;
	   }
	   newsrcBuffer->append(bitdata.wordpointer, includeBytes);
	   bitdata.bitsleft-=includeBytes*8;
	   bitdata.wordpointer+=includeBytes;
	   includeBytes=0;
   }

   if (skipBytes > 0)
   {
	   if (bitdata.bitsleft<skipBytes*8)
	   {
		   skipBytes-=bitdata.bitsleft/8;
		   return S_OK;
	   }
	   bitdata.bitsleft-=skipBytes*8;
	   bitdata.wordpointer+=skipBytes;		   
	   skipBytes=0;
   }

   while (bitdata.bitsleft > 128) // 64 : necessary size to have the AC3 block header
   {
	   if (bitdata.showBits(16) == 0x0B77) // AC3 stream
	   {
		   searchSync=false;
		   unsigned char *backuppointer=bitdata.wordpointer;
		   int backupBitsLeft=bitdata.bitsleft;
		   int frame_size=0;


		   bitdata.getBits(16);
		   uint32_t bitstream_id = bitdata.showBits(29) & 0x1F;
		   if (bitstream_id<=10) // AC3
		   {
			   if (streamformat==UNDEFINED)
				streamformat=REGULAR_AC3;

			   bitdata.getBits(16); // CRC
			   int sr_code = bitdata.getBits(2); // Sample rate code
			   int frame_size_code=bitdata.getBits(6); // Frame size code
			   if (frame_size_code > 37) // Wrong size
			   {
				   // Restore pointer to sync word position
				   bitdata.wordpointer=backuppointer;
				   bitdata.bitsleft=backupBitsLeft;
				   bitdata.bitindex=0;
				   bitdata.getBits(8); // Jump to next byte
				   continue;
			   }
			   frame_size=ff_ac3_frame_size_tab[frame_size_code][sr_code] * 2;
			   bitdata.getBits(8); // Skip bsid and bitstream mode
			   int channel_mode=bitdata.getBits(3);
			   if (channel_mode==2) // stereo
				   bitdata.getBits(2); // Skip dsurmod
			   else
			   {
				   if ((channel_mode & 1) && channel_mode!=1) // Not mono
					   bitdata.getBits(2); // Center mix level
				   if (channel_mode & 4)
					   bitdata.getBits(2); // Surround mix levels
			   }
			   int lfe=bitdata.getBits(1);
			   int sr_shift=bitstream_id-8;
			   if (sr_shift<0)
				   sr_shift=0;

			   if (streamformat==REGULAR_AC3 || useAC3CoreOnly)
			   {
				sample_rate=ff_ac3_sample_rate_tab[sr_code] >> sr_shift;
				bit_rate=(ff_ac3_bitrate_tab[frame_size_code>>1]*1000) >> sr_shift;
				channels=ff_ac3_channels_tab[channel_mode]+lfe;
			   }
		   }
		   else // EAC3
		   {
			   if (streamformat==UNDEFINED || streamformat==REGULAR_AC3)
				   streamformat=EAC3;

			   bitdata.getBits(2); // Frame type
			   bitdata.getBits(3); // Substream id
			   frame_size=(bitdata.getBits(11) + 1) << 1;
			   int sr_code=bitdata.getBits(2); // Sample rate code
			   sample_rate=0;
			   int num_blocks=6;
			   int sr_shift=0;
			   if (sr_code==3)
			   {
				   int sr_code2=bitdata.getBits(2);
				   sample_rate=ff_ac3_sample_rate_tab[sr_code2] / 2;
				   sr_shift=1;
			   }
			   else
			   {
				   num_blocks=eac3_blocks[bitdata.getBits(2)];
				   sample_rate = ff_ac3_sample_rate_tab[sr_code];
			   }

			   int channel_mode=bitdata.getBits(3);
			   int lfe=bitdata.getBits(1);
			   channels = ff_ac3_channels_tab[channel_mode] + lfe;

			   if (streamformat==EAC3)
			   {
				   bit_rate=(uint32_t)(8.0 *frame_size * sample_rate /
                        (num_blocks * 256.0));
			   }

		   }


		   // Restore pointer to sync word position
		   bitdata.wordpointer=backuppointer;
		   bitdata.bitsleft=backupBitsLeft;
		   bitdata.bitindex=0;

		   // If AC3 codec is SPDIF or MLP decoder disabled, we keep AC3 frames otherwise (MLP) we throw them avay
		   if (useAC3CoreOnly || streamformat != AC3_TRUEHD)
		   {
			   // AC3 frame not complete in this buffer. 
			   // Back it up for next pass
			   if (frame_size>bitdata.bitsleft/8)
			   {
				  backupbuf.clear();
				  backupbuf.append(bitdata.wordpointer, bitdata.bitsleft/8);
			      /*newsrcBuffer->append(bitdata.wordpointer, bitdata.bitsleft/8);
				  includeBytes=frame_size-bitdata.bitsleft/8;*/
				  bitdata.bitsleft=0;
			   }
			   else // store the block into the source buffer
			   {
				   newsrcBuffer->append(bitdata.wordpointer, frame_size);
				   bitdata.bitsleft-=frame_size*8;
				   bitdata.wordpointer+=frame_size;
			   }
		   }
		   else // MLP parser does not know how to handle AC3 frames interweaved
		   {
			   bitdata.bitindex=0;
			   bitdata.bitsleft=backupBitsLeft-frame_size*8;
			   if (bitdata.bitsleft<0)
			   {
				   skipBytes=-bitdata.bitsleft/8;
				   bitdata.bitsleft=0;
				   //bitdata.wordpointer+=skipStartBitsLeft/8; // EOF
			   }
			   else
				bitdata.wordpointer=backuppointer+frame_size;
		   }
	   }
	   else if (bitdata.showBits(32,32) == 0xf8726fba || // True HD major sync frame
		   bitdata.showBits(32,32) == 0xf8726fbb) // MLP
	   {
		   hasMLPFrames=true;
		   searchSync=false;
		   
		   int32_t frame_size=(((bitdata.wordpointer[0] << 8) | bitdata.wordpointer[1])& 0xfff) * 2;

		   // Oops... current decoder is AC3 whereas we have MLP/TrueHD data =>
		   // and AC3 codec is not set to SPDIF and MLP decoder is enabled. So drop AC3 frames and keep MLP/TrueHD only
		   if ((streamformat==REGULAR_AC3 || streamformat==EAC3)
			   && !useAC3CoreOnly)
			   newsrcBuffer->clear();
		   
		   // Save the start position and left length of the MLP/TrueHD block
		   unsigned char *backuppointer=bitdata.wordpointer;
		   int backupBitsLeft=bitdata.bitsleft;
		   
		   bitdata.getBits(32); // Jump frame size (16) and ignored bits (16)	   
		   
		   // Identify the stream : if AC3 frames found (REGULAR_AC3 or EAC3) and we are
		   // in a MLP frame, then this is an AC3_TRUEHD stream.
		   // Otherwise this is either a TRUEHD or MLP stream (basing on the header sync)
		   if (streamformat!=AC3_TRUEHD)
		   {
		   if (streamformat==REGULAR_AC3 || streamformat==EAC3)
			   streamformat=AC3_TRUEHD;
		   else
			   bitdata.wordpointer[3] == 0xba ? streamformat=TRUEHD : streamformat=MLP;
		   }
		   
		   uint32_t ratebits=0;
		   
		   bool isTrueHD=false;
		   uint32_t group1_samplerate=0, group2_samplerate=0;
		   if (bitdata.wordpointer[3] == 0xbb) // MLP (0xbb)
		   {
			   bitdata.getBits(32);
			   int group1_bits=bitdata.getBits(4); // group1 bits
			   if (!useAC3CoreOnly) // Don't update stream format with MLP config if we are in AC3 mode
			   {
			   switch (group1_bits)
				 {
			   case 8:sample_format=TsampleFormat::SF_PCM8;break;
				  case 0:
				  case 16:
				  default:sample_format=TsampleFormat::SF_PCM16;break;
				  case 20:sample_format=TsampleFormat::SF_LPCM20;break;
				  case 24:sample_format=TsampleFormat::SF_PCM24;break;
				  case 32:sample_format=TsampleFormat::SF_PCM32;break;
				 }
			   }
			   int group2_bits=bitdata.getBits(4); // group2 bits
			   ratebits=bitdata.getBits(4);
			   group1_samplerate=(ratebits==0xF)?0:(ratebits & 8 ? 44100 : 48000) << (ratebits & 7);
			   if (!useAC3CoreOnly) // Don't update stream format with MLP config if we are in AC3 mode
				sample_rate=group1_samplerate;
			   
			   uint32_t ratebits2=bitdata.getBits(4);
			   group2_samplerate=(ratebits2==0xF)?0:(ratebits2 & 8 ? 44100 : 48000) << (ratebits2 & 7);
			   bitdata.getBits(11); // Skip
			   int channels_mlp=bitdata.getBits(5);
			   if (!useAC3CoreOnly)
			    channels=mlp_channels[channels_mlp];
		   }
		   else // Dolby TrueHD (0xba)
		   {
			   isTrueHD=true;
			   bitdata.getBits(32);
			   int group1_bits=24; // group1 bits
			   sample_format=TsampleFormat::SF_PCM24;
			   int group2_bits=0; // group2 bits
			   ratebits=bitdata.getBits(4);
			   group1_samplerate=(ratebits==0xF)?0:(ratebits & 8 ? 44100 : 48000) << (ratebits & 7);
			   if (!useAC3CoreOnly) // Don't update stream format with MLP config if we are in AC3 mode
				sample_rate=group1_samplerate;

			   group2_samplerate=0;
			   bitdata.getBits(8); // Skip
			   int channels_thd_stream1=bitdata.getBits(5);
			   bitdata.getBits(2); // Skip
			   int channels_thd_stream2=bitdata.getBits(13);

			   if (!useAC3CoreOnly) // Don't update stream format with MLP config if we are in AC3 mode
			   {
				   if (channels_thd_stream2)
					   channels=truehd_channels(channels_thd_stream2);
				   else
					   channels=truehd_channels(channels_thd_stream1);
			   }
		   }

		   bitdata.getBits(48); // Skip

		   // WRONG frame_size=40 << (ratebits & 7);

		   int isVbr=bitdata.getBits(1);
		   uint32_t peak_bitrate = (bitdata.getBits(15) * group1_samplerate + 8) >> 4;
		   
		   if (!useAC3CoreOnly) // Don't update stream format with MLP config if we are in AC3 mode
			bit_rate=peak_bitrate;

		   int num_substreams = bitdata.getBits(4);

		   

		   // Restore pointer to (sync word - 4 bytes) position
		   bitdata.bitindex=0;
		   bitdata.bitsleft=backupBitsLeft;
		   bitdata.wordpointer=backuppointer;

		   // If codecId is AC3, throw avay the MLP/TrueHD block
		   if (streamformat==AC3_TRUEHD && useAC3CoreOnly)
		   {
			   bitdata.bitsleft-=frame_size*8;
			   if (bitdata.bitsleft<0)
			   {
				   skipBytes=-bitdata.bitsleft/8;
				   bitdata.bitsleft=0;
				   //bitdata.wordpointer+=skipStartBitsLeft/8; // EOF
			   }
			   else
				   bitdata.wordpointer+=frame_size;
		   }
		   else
		   {
			   // MLP frame not complete in this buffer. 
			   // Back it up until having complete frame
			   if (frame_size>bitdata.bitsleft/8)
			   {
				  backupbuf.append(bitdata.wordpointer, bitdata.bitsleft/8);
				  /*newsrcBuffer->append(bitdata.wordpointer, bitdata.bitsleft/8);
				  includeBytes=frame_size-bitdata.bitsleft/8;*/
				  bitdata.bitsleft=0;
			   }
			   else // store the block into the source buffer
			   {
				   newsrcBuffer->append(bitdata.wordpointer, frame_size);
				   bitdata.bitsleft-=frame_size*8;
				   bitdata.wordpointer+=frame_size;
			   }			   
		   }
	   }
	   else // If just AC3, skip byte, or if has MLP frams this is a non major sync frame
	   {
		   // searchSync flag is enabled at the initialization and when doing a skip in the file
		   // In those cases we should look after a sync frame first
		   if (hasMLPFrames && !searchSync)
		   {
			   int32_t frame_size=(((bitdata.wordpointer[0] << 8) | bitdata.wordpointer[1])& 0xfff) * 2;
			   if (frame_size < 3)
			   {
				   bitdata.bitsleft-=8;
				   bitdata.wordpointer++;
			   }
			   
			   // If the stream contains AC3 frames and AC3 is set to SPDIF throw avay the MLP/TrueHD block
			   if (streamformat==AC3_TRUEHD && useAC3CoreOnly)
			   {
				   bitdata.bitsleft-=frame_size*8;
				   if (bitdata.bitsleft<0)
				   {
					   skipBytes=-bitdata.bitsleft/8;
					   bitdata.bitsleft=0;
					   //bitdata.wordpointer+=skipStartBitsLeft/8; // EOF
				   }
				   else
					   bitdata.wordpointer+=frame_size;
			   }
			   else
			   {
				   // MLP/TrueHD frame not complete in this buffer. 
				   // Back it up until having complete frame
				   if (frame_size>bitdata.bitsleft/8)
				   {
					  backupbuf.append(bitdata.wordpointer, bitdata.bitsleft/8);
					  /*newsrcBuffer->append(bitdata.wordpointer, bitdata.bitsleft/8);
					  includeBytes=frame_size-bitdata.bitsleft/8;*/
					  bitdata.bitsleft=0;
				   }
				   else // store the block into the source buffer
				   {
					   // Additional check : in case we are a midle of a frame, check that there is
					   // no AC3 or major MLP frame inside this supposed non major frame
					   // In that case we were wrong : this is not a non major frame, 
					   // so we throw the bytes between
					   unsigned char *ptr=bitdata.wordpointer;
					   bool skip=false;
					   while (ptr<bitdata.wordpointer+frame_size)
					   {
						   if (bitdata.showBits(16) == 0x0B77 || // AC3 stream
								bitdata.showBits(32,32) == 0xf8726fba || // True HD major sync frame
								bitdata.showBits(32,32) == 0xf8726fbb) // MLP major sync frame
						   {
							   bitdata.bitsleft-=(ptr-bitdata.wordpointer)*8;
							   bitdata.wordpointer+=(ptr-bitdata.wordpointer);
							   skip=true;
							   break;
						   }
						   ptr++;
					   }
					   if (skip) continue;

					   newsrcBuffer->append(bitdata.wordpointer, frame_size);
					   bitdata.bitsleft-=frame_size*8;
					   bitdata.wordpointer+=frame_size;
				   }			   
			   }
		   }
		   else // This is neither AC3/major MLP/non-major MLP frame so we throw away the byte
		   {
				bitdata.bitsleft -= 8;
				bitdata.wordpointer++;
		   }
	   }
   }

   // Copy remaining bytes into a backup buffer for next frame
   if (bitdata.bitsleft>0)
   {
	   //newsrcBuffer->append(bitdata.wordpointer, bitdata.bitsleft/8);
	   backupbuf.clear();
	   backupbuf.append(bitdata.wordpointer, bitdata.bitsleft/8);
   }
   return S_OK;
}

static int truehd_channels(int chanmap)
{
    int channels = 0, i;

    for (i = 0; i < 13; i++)
        channels += thd_chancount[i] * ((chanmap >> i) & 1);

    return channels;
}
