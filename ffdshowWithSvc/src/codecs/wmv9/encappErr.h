//*@@@+++@@@@******************************************************************
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//###########################################################################
//## AVISampleVFW                                                          ##
//##-----------------------------------------------------------------------##
//## 0x6000 - 0x6FFF Errors for AVISampleVFW                               ##
//## This file contains the message definitions for the AVISampleVFW       ##
//##                                                                       ##
//###########################################################################


/*
-----------------------------------------------------------------------------
 HEADER SECTION

 The header section defines names and language identifiers for use
 by the message definitions later in this file. The MessageIdTypedef,
 SeverityNames, FacilityNames, and LanguageNames keywords are
 optional and not required.


 The MessageIdTypedef keyword gives a typedef name that is used in a
 type cast for each message code in the generated include file. Each
 message code appears in the include file with the format: #define
 name ((type) 0xnnnnnnnn) The default value for type is empty, and no
 type cast is generated. It is the programmer's responsibility to
 specify a typedef statement in the application source code to define
 the type. The type used in the typedef must be large enough to
 accommodate the entire 32-bit message code.


 The SeverityNames keyword defines the set of names that are allowed
 as the value of the Severity keyword in the message definition. The
 set is delimited by left and right parentheses. Associated with each
 severity name is a number that, when shifted left by 30, gives the
 bit pattern to logical-OR with the Facility value and MessageId
 value to form the full 32-bit message code. The default value of
 this keyword is:

 SeverityNames=(
   Success=0x0
   Informational=0x1
   Warning=0x2
   Error=0x3
   )

 Severity values occupy the high two bits of a 32-bit message code.
 Any severity value that does not fit in two bits is an error. The
 severity codes can be given symbolic names by following each value
 with :name


 The FacilityNames keyword defines the set of names that are allowed
 as the value of the Facility keyword in the message definition. The
 set is delimited by left and right parentheses. Associated with each
 facility name is a number that, when shifted left by 16 bits, gives
 the bit pattern to logical-OR with the Severity value and MessageId
 value to form the full 32-bit message code. The default value of
 this keyword is:

 FacilityNames=(
   System=0x0FF
   Application=0xFFF
 )

 Facility codes occupy the low order 12 bits of the high order
 16-bits of a 32-bit message code. Any facility code that does not
 fit in 12 bits is an error. This allows for 4,096 facility codes.
 The first 256 codes are reserved for use by the system software. The
 facility codes can be given symbolic names by following each value
 with :name


 The 1033 comes from the result of the MAKELANGID() macro
 (SUBLANG_ENGLISH_US << 10) | (LANG_ENGLISH)

 The LanguageNames keyword defines the set of names that are allowed
 as the value of the Language keyword in the message definition. The
 set is delimited by left and right parentheses. Associated with each
 language name is a number and a file name that are used to name the
 generated resource file that contains the messages for that
 language. The number corresponds to the language identifier to use
 in the resource table. The number is separated from the file name
 with a colon. The initial value of LanguageNames is:

 LanguageNames=(English=1:MSG00001)

 Any new names in the source file that don't override the built-in
 names are added to the list of valid languages. This allows an
 application to support private languages with descriptive names.


-------------------------------------------------------------------------
 MESSAGE DEFINITION SECTION

 Following the header section is the body of the Message Compiler
 source file. The body consists of zero or more message definitions.
 Each message definition begins with one or more of the following
 statements:

 MessageId = [number|+number]
 Severity = severity_name
 Facility = facility_name
 SymbolicName = name

 The MessageId statement marks the beginning of the message
 definition. A MessageID statement is required for each message,
 although the value is optional. If no value is specified, the value
 used is the previous value for the facility plus one. If the value
 is specified as +number, then the value used is the previous value
 for the facility plus the number after the plus sign. Otherwise, if
 a numeric value is given, that value is used. Any MessageId value
 that does not fit in 16 bits is an error.

 The Severity and Facility statements are optional. These statements
 specify additional bits to OR into the final 32-bit message code. If
 not specified, they default to the value last specified for a message
 definition. The initial values prior to processing the first message
 definition are:

 Severity=Success
 Facility=Application

 The value associated with Severity and Facility must match one of
 the names given in the FacilityNames and SeverityNames statements in
 the header section. The SymbolicName statement allows you to
 associate a C/C++ symbolic constant with the final 32-bit message
 code.


-----------------------------------------------------------------------------
  IMPORTANT - PLEASE READ BEFORE EDITING FILE
  This file is divided into four sections. They are:
   1. Success Codes
   2. Information Codes
   3. Warning Codes
   4. Error Codes

  Please enter your codes in the appropriate section.
  All codes must be in sorted order. Please use codes
  in the middle that are free before using codes at the end.
*/
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0
#define FACILITY_STUBS                   0x3
#define FACILITY_RUNTIME                 0x2


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: E_INVALID_ARG
//
// MessageText:
//
//  One of the arguments is not supported.%0
//
#define E_INVALID_ARG                    ((DWORD)0xC0046000L)

//
// MessageId: E_PARSE_INTERNAL
//
// MessageText:
//
//  Internal parsing error occured. Please report this error.%0
//
#define E_PARSE_INTERNAL                 ((DWORD)0xC0046001L)

//
// MessageId: E_PARSE_NO_PARAM
//
// MessageText:
//
//  Argument "%1" needs a parameter.%0
//
#define E_PARSE_NO_PARAM                 ((DWORD)0xC0046002L)

//
// MessageId: E_PARSE_OUT_OF_RANGE
//
// MessageText:
//
//  Parameter "%1" is out of range.%0
//
#define E_PARSE_OUT_OF_RANGE             ((DWORD)0xC0046004L)

//
// MessageId: E_PARSE_NO_INPUT
//
// MessageText:
//
//  Input file was not specified.%0
//
#define E_PARSE_NO_INPUT                 ((DWORD)0xC0046005L)

//
// MessageId: E_PARSE_NO_OUTPUT
//
// MessageText:
//
//  Output file was not specified.%0
//
#define E_PARSE_NO_OUTPUT                ((DWORD)0xC0046006L)

//
// MessageId: E_CONFLICTING_ONLYS
//
// MessageText:
//
//  "audio_only" and "video_only" arguments are conflicting.%0
//
#define E_CONFLICTING_ONLYS              ((DWORD)0xC0046007L)

//
// MessageId: E_REMAP_AUDIO
//
// MessageText:
//
//  Failed to remap audio arguments.%0
//
#define E_REMAP_AUDIO                    ((DWORD)0xC0046008L)

//
// MessageId: E_REMAP_VIDEO
//
// MessageText:
//
//  Failed to remap video arguments.%0
//
#define E_REMAP_VIDEO                    ((DWORD)0xC0046009L)

//
// MessageId: E_NO_COM
//
// MessageText:
//
//  Application "%1" failed to initialize COM.%0
//
#define E_NO_COM                         ((DWORD)0xC0046010L)

//
// MessageId: E_INVALID_INPUT
//
// MessageText:
//
//  Can't open input file.%0
//
#define E_INVALID_INPUT                  ((DWORD)0xC0046019L)

//
// MessageId: E_INVALID_OUTPUT
//
// MessageText:
//
//  Can't open output file.%0
//
#define E_INVALID_OUTPUT                 ((DWORD)0xC004601AL)

//
// MessageId: E_CANT_UNCOMPRESS
//
// MessageText:
//
//  Can't uncompress the input stream.%0
//
#define E_CANT_UNCOMPRESS                ((DWORD)0xC004601CL)

//
// MessageId: E_NO_STREAM
//
// MessageText:
//
//  There is no stream available to encode.%0
//
#define E_NO_STREAM                      ((DWORD)0xC0046020L)

//
// MessageId: E_NO_INPUT_TYPE
//
// MessageText:
//
//  Can't obtain the input type for the audio stream.%0
//
#define E_NO_INPUT_TYPE                  ((DWORD)0xC0046033L)

//
// MessageId: E_FORMAT_NOT_SUPPORTED
//
// MessageText:
//
//  One stream has a format wich is not WAVFORMATEX, VIDEOINFO or VIDEOINFO2.%0
//
#define E_FORMAT_NOT_SUPPORTED           ((DWORD)0xC004603AL)

//
// MessageId: E_MEDIA_TYPE_REJECTED
//
// MessageText:
//
//  Pump Filter can't accept this format.%0
//
#define E_MEDIA_TYPE_REJECTED            ((DWORD)0xC004603BL)

//
// MessageId: E_NO_CONTEXT
//
// MessageText:
//
//  Tap Filter can't provide the contextpad.%0
//
#define E_NO_CONTEXT                     ((DWORD)0xC004603CL)

//
// MessageId: E_DMO_NO_STATUS
//
// MessageText:
//
//  DMO encoder status cannot be obtained.%0
//
#define E_DMO_NO_STATUS                  ((DWORD)0xC0046050L)

//
// MessageId: E_DMO_NOTACCEPTING
//
// MessageText:
//
//  DMO encoder doesn't accept input data.%0
//
#define E_DMO_NOTACCEPTING               ((DWORD)0xC0046051L)

//
// MessageId: E_AUDIO_INVALID_FORMAT
//
// MessageText:
//
//  Audio DMO encoder enumerated an invalid media type.%0
//
#define E_AUDIO_INVALID_FORMAT           ((DWORD)0xC0046060L)

//
// MessageId: E_AUDIO_FORMAT_DOESNT_MATCH
//
// MessageText:
//
//  Audio parameters don't match with the supported encoder modes.%0
//
#define E_AUDIO_FORMAT_DOESNT_MATCH      ((DWORD)0xC0046061L)

//
// MessageId: E_AUDIO_COMPRESSION_NOT_SUPPORTED
//
// MessageText:
//
//  Requested audio compression algorithm is not supported.%0
//
#define E_AUDIO_COMPRESSION_NOT_SUPPORTED ((DWORD)0xC0046062L)

//
// MessageId: E_AUDIO_VBR_NOT_SUPPORTED
//
// MessageText:
//
//  VBR is not supported for the current settings.%0
//
#define E_AUDIO_VBR_NOT_SUPPORTED        ((DWORD)0xC0046063L)

//
// MessageId: E_AUDIO_NPASS_NOT_SUPPORTED
//
// MessageText:
//
//  Multiple passes not supported for the current settings.%0
//
#define E_AUDIO_NPASS_NOT_SUPPORTED      ((DWORD)0xC0046064L)

//
// MessageId: E_AUDIO_PEAK_BITRATE_REJECTED
//
// MessageText:
//
//  Audio peak bitrate was rejected.%0
//
#define E_AUDIO_PEAK_BITRATE_REJECTED    ((DWORD)0xC0046065L)

//
// MessageId: E_AUDIO_PEAK_BUFFER_REJECTED
//
// MessageText:
//
//  Audio peak buffer was rejected.%0
//
#define E_AUDIO_PEAK_BUFFER_REJECTED     ((DWORD)0xC0046066L)

//
// MessageId: E_SPEECH_MODE_NOT_SUPPORTED
//
// MessageText:
//
//  Audio type ( speech/music ) not supported for the current settings.%0
//
#define E_SPEECH_MODE_NOT_SUPPORTED      ((DWORD)0xC0046067L)

//
// MessageId: E_AUDIO_TYPE_NOT_SET
//
// MessageText:
//
//  DMO audio encoder rejected the type.%0
//
#define E_AUDIO_TYPE_NOT_SET             ((DWORD)0xC0046068L)

//
// MessageId: E_VIDEO_COMPRESSION_NOT_SUPPORTED
//
// MessageText:
//
//  Requested video compression algorithm is not supported.%0
//
#define E_VIDEO_COMPRESSION_NOT_SUPPORTED ((DWORD)0xC0046070L)

//
// MessageId: E_VIDEO_INVALID_INPUT_TYPE
//
// MessageText:
//
//  Video input type is invalid.%0
//
#define E_VIDEO_INVALID_INPUT_TYPE       ((DWORD)0xC0046071L)

//
// MessageId: E_NO_PRIVATE_DATA
//
// MessageText:
//
//  Video encoder doesn't support IWMPrivateData interface.%0
//
#define E_NO_PRIVATE_DATA                ((DWORD)0xC0046072L)

//
// MessageId: E_PARTIAL_TYPE_REJECTED
//
// MessageText:
//
//  Video partial output type was rejected.%0
//
#define E_PARTIAL_TYPE_REJECTED          ((DWORD)0xC0046073L)

//
// MessageId: E_NO_PRIVATE_DATA_COUNT
//
// MessageText:
//
//  Video encoder failed to provide the size of private data.%0
//
#define E_NO_PRIVATE_DATA_COUNT          ((DWORD)0xC0046074L)

//
// MessageId: E_PRIVATE_DATA_FAILED
//
// MessageText:
//
//  Video encoder failed to provide the private data.%0
//
#define E_PRIVATE_DATA_FAILED            ((DWORD)0xC0046075L)

//
// MessageId: E_VIDEO_VBR_NOT_SUPPORTED
//
// MessageText:
//
//  Video encoder doesn't support VBR mode.%0
//
#define E_VIDEO_VBR_NOT_SUPPORTED        ((DWORD)0xC0046076L)

//
// MessageId: E_VBR_QUALITY_REJECTED
//
// MessageText:
//
//  Video encoder rejected the quality for 1 pass VBR mode.%0
//
#define E_VBR_QUALITY_REJECTED           ((DWORD)0xC0046077L)

//
// MessageId: E_VIDEO_NPASS_NOT_SUPPORTED
//
// MessageText:
//
//  Video encoder doesn't support multiple pass encoding.%0
//
#define E_VIDEO_NPASS_NOT_SUPPORTED      ((DWORD)0xC0046078L)

//
// MessageId: E_VIDEO_BITRATE_REJECTED
//
// MessageText:
//
//  Video encoder rejected the average bitrate provided.%0
//
#define E_VIDEO_BITRATE_REJECTED         ((DWORD)0xC0046079L)

//
// MessageId: E_VIDEO_BUFFER_REJECTED
//
// MessageText:
//
//  Video encoder rejected the buffer size provided.%0
//
#define E_VIDEO_BUFFER_REJECTED          ((DWORD)0xC004607AL)

//
// MessageId: E_VIDEO_PEAK_BITRATE_REJECTED
//
// MessageText:
//
//  Video encoder rejected the maximum bitrate provided.%0
//
#define E_VIDEO_PEAK_BITRATE_REJECTED    ((DWORD)0xC004607BL)

//
// MessageId: E_VIDEO_PEAK_BUFFER_REJECTED
//
// MessageText:
//
//  Video encoder rejected the maximum buffer size provided.%0
//
#define E_VIDEO_PEAK_BUFFER_REJECTED     ((DWORD)0xC004607CL)

//
// MessageId: E_VIDEO_PROFILE_REJECTED
//
// MessageText:
//
//  Video encoder rejected the profile provided.%0
//
#define E_VIDEO_PROFILE_REJECTED         ((DWORD)0xC004607DL)

//
// MessageId: E_VIDEO_KEYDIST_REJECTED
//
// MessageText:
//
//  Video encoder rejected the key frame distance provided.%0
//
#define E_VIDEO_KEYDIST_REJECTED         ((DWORD)0xC004607EL)

//
// MessageId: E_VIDEO_CRISPNESS_REJECTED
//
// MessageText:
//
//  Video encoder rejected the quality parameter provided.%0
//
#define E_VIDEO_CRISPNESS_REJECTED       ((DWORD)0xC004607FL)

//
// MessageId: E_VIDEO_INTERLACE_REJECTED
//
// MessageText:
//
//  Tnterlace mode mode rejected.%0
//
#define E_VIDEO_INTERLACE_REJECTED       ((DWORD)0xC0046080L)

//
// MessageId: E_VIDEO_INVALID_PROFILE
//
// MessageText:
//
//  Video profile index out of range.%0
//
#define E_VIDEO_INVALID_PROFILE          ((DWORD)0xC0046081L)

//
// MessageId: E_INVALID_FORMAT
//
// MessageText:
//
//  Input stream format is invalid.%0
//
#define E_INVALID_FORMAT                 ((DWORD)0xC0046082L)

//
// MessageId: E_VIDEO_TYPE_NOT_SET
//
// MessageText:
//
//  DMO video encoder rejected the type.%0
//
#define E_VIDEO_TYPE_NOT_SET             ((DWORD)0xC0046083L)

//
// MessageId: E_BAD_STREAM_LENGTH
//
// MessageText:
//
//  Input stream has incorrect length.%0
//
#define E_BAD_STREAM_LENGTH              ((DWORD)0xC0046090L)

//
// MessageId: E_READ_FRAME
//
// MessageText:
//
//  Failed to read one input frame.%0
//
#define E_READ_FRAME                     ((DWORD)0xC0046091L)

//
// MessageId: E_NO_FRAMES
//
// MessageText:
//
//  No frames written to the output file.%0
//
#define E_NO_FRAMES                      ((DWORD)0xC0046092L)

//
// MessageId: E_NEEDS_LOOKAHEAD
//
// MessageText:
//
//  The encoder needs lookahead.%0
//
#define E_NEEDS_LOOKAHEAD                ((DWORD)0xC0046093L)

//
// MessageId: E_AUTO_MODE_NOT_SUPPORTED
//
// MessageText:
//
//  The speech codec doesn't support auto mode for this bitrate.%0
//
#define E_AUTO_MODE_NOT_SUPPORTED        ((DWORD)0xC0046094L)

#define E_VIDEO_QUALITY_REJECTED         ((DWORD)0xC0046095L)
