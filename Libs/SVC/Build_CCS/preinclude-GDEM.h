/* RAL: "preinclude.h"-> file to be pre-included in CCS project
 * (building option: "--preinclude="<myPath>/preinclude.h"").
 *  DM642 Version
 */

// Include functions headers to reside in "global scope"
#include <std.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include TI´s functions headers to reside in "global scope"
#include <tsk.h>
#include <mem.h>
#include <sem.h>


// Other global definitions
//#define 	MSYS_C64X  //RAL
#undef   WIN32

// Global Profiling configurations (used with DSP/Bios modules)
//#define LOG_TO_STDOUT //* E2C: si no se define se guarda en fichero, 
						//		si se define se muestra por pantalla

//#define PROFILE_MEM_USAGE //*E2C deshabilitar


// Other global macros & defs. for debugging
#ifndef LOG_TO_STDOUT

   // Global variable "FILE* pFile" is defined in "H264AVCDecoderLibTest.cpp"...
   // Other source files use pFile as external reference.
   #ifndef __H264AVCDECODERLIBTEST_H_D65BE9B4_A8DA_11D3_AFE7_005004464B79 
      extern FILE* pLogFile; 
	  extern FILE* pLogFileTime;
   #endif
   #define	FILE_OUT pLogFile
   #define	FILE_OUT_TIME pLogFileTime
#else
   #define FILE_OUT stdout
   #define	FILE_OUT_TIME stdout
#endif

#ifdef PROFILE_MEM_USAGE
   #ifndef __H264AVCDECODERLIBTEST_H_D65BE9B4_A8DA_11D3_AFE7_005004464B79  
      extern SEM_Handle sem; 
#endif

   #define FPRINTF(args) \
         {SEM_pendBinary(sem, SYS_FOREVER);\
          fprintf args;\
          SEM_postBinary(sem);}
#else 
   #define FPRINTF(args) fprintf args
#endif 
