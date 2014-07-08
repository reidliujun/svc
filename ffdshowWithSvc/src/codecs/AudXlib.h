#ifndef AUDXLIB_H
#define AUDXLIB_H

#if defined( __cplusplus )
	extern "C"
{
#endif


//============================================================================
//:::::::::::::::::::::::::::::::ENCODER::::::::::::::::::::::::::::::::::::::

/**
 * Initializes audx encoding
 * Input:
 *   quality - values: 0 (STRQ 80 kbps), 1 (STDQ 128 kbps), 2 (HGHQ 192 kbps) or 3 (SPBQ 192 kbps)
 * Returns:
 *   0       - couldn't start new encoding process,
 *   nonzero - handler of the newly initialized encoding process
 */
long __cdecl initEncoding ( int quality );

/**
 * Encodes audx frame.
 * Input:
 *   henc      - handler of an encoding process,
 *   pcm_block - input buffer containing pcm data to be encoded (must be nonNULL) with 6912 samples (1152 per each channel),
 *                     the interlaced channel order MUST be: [0: FL]   [1: FR]   [2: C]   [3: LFE]   [4: SL]   [5: SR].
 * Output:
 *   audx_frame - output buffer filled with audx stream data
 *
 * Returns:
 *   -1           - handler of an uninitialized encoding process, invalid input data or error in processing,
 *    0           - input data and processing correct - needs more input,
 *    frameLength - frame length (in bytes) - the data int audx_frame can be consumed.
 */
int __cdecl encodeAudXframe ( long henc, short* pcm_block, unsigned char* audx_frame );

/**
 * Flushing internal buffers. Should be called after all PCM input samples have been consumed.
 * Output:
 *   henc       - handler of an encoding process,
 *   audx_frame - output buffer filled with audx stream data.
 *
 * Returns:
 *   -1           - flushing finished - all internal buffers are empty and encoding process is reset
 *                  (henc is no longer a handler of the process),
 *                  or uninitialized encoding process,
 *    0           - flushing should be called once again - no output data produced,
 *    frameLength - frame length (in bytes) - the data in audx_frame can be consumed.
 */
int __cdecl flushEncoder ( long henc, unsigned char* audx_frame );
//============================================================================


//============================================================================
//:::::::::::::::::::::::::::::::DECODER::::::::::::::::::::::::::::::::::::::
/**
 * Initializes audx decoding
 * Input:
 *   quality - values: 0 (STRQ 80 kbps), 1 (STDQ 128 kbps), 2 (HGHQ 192 kbps) or 3 (SPBQ 192 kbps)
 * Returns:
 *   0       - couldn't initilize ne decoding process
 *   nonzero - handler of the newly initialized decoding process
 */
long __cdecl initDecoding ( void );

/**
 * Decodes audx/mp3 frame.
 * Input:
 *   hdec            - handler of a decoding process,
 *   audx_frame      - input buffer containing data to be decoded (must be nonNULL),
 *   audx_frame_size - size of the input buffer in bytes (must be > 0)
 *
 * Output:
 *   dec_pcm_block      - output buffer filled with 16bits PCM samples (must be nonNULL)
 *   dec_pcm_block_size - size of the output buffer in bytes (must be nonNULL)
 *   is_audx            - indicates whether input data contained an audx frame or regular mp3 (must be nonNULL)
 *
 * Returns:
 *   -1 - handler of an uninitialized decoding process, invalid input data or error in processing,
 *    0 - input data and processing correct, input data was not consumed (should be supplied once again)
 *    1 - input data and processing correct, input data was consumed (new input portion can be supplied)
 */
int __cdecl decodeAudXframe ( long hdec, unsigned char* audx_frame, long audx_frame_size, short* dec_pcm_block, long* dec_pcm_block_size, int* is_audx );

/**
 * Resets an initialized decoding process
 * (puts the process in a uninitialized state, hdec is no longer a handler of the process),
 * or does nothing if a decoding process is uninitialized
 * Input:
 *   hdec - handler to a decoding process
 */
void __cdecl resetDecoder ( long hdec );
//============================================================================

#if defined(__cplusplus)
}
#endif

#endif
