/* Toggles debugging informations */
#undef MP_DEBUG

/* Define this if your system has the "alloca.h" header file */
#define HAVE_ALLOCA_H 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#undef WORDS_BIGENDIAN

#ifndef WIN64
#ifdef __GNUC__
#define ARCH_X86 1
#endif
#endif

/* Define this to any prefered value from 386 up to infinity with step 100 */
#define __CPU__ 686

#undef TARGET_LINUX

/* Runtime Cpudetection */
#define RUNTIME_CPUDETECT 1
#define USE_FASTMEMCPY 1
#define HAVE_THREADS 1
#define HAVE_MEMALIGN 1
#define ASMALIGN(ZEROBITS) ".align 1<<" #ZEROBITS "\n\t"
#define HAVE_BSWAP 1

#ifndef WIN64
#ifdef __GNUC__
/* Extension defines */
#define HAVE_3DNOW 1    // only define if you have 3DNOW (AMD k6-2, AMD Athlon, iDT WinChip, etc.)
#define HAVE_3DNOWEX 1  // only define if you have 3DNOWEX (AMD Athlon, etc.)
#define HAVE_MMX 1      // only define if you have MMX (newer x86 chips, not P54C/PPro)
#define HAVE_MMX2 1     // only define if you have MMX2 (Athlon/PIII/4/CelII)
#define HAVE_SSE 1      // only define if you have SSE (Intel Pentium III/4 or Celeron II)
#define HAVE_SSE2 1     // only define if you have SSE2 (Intel Pentium 4)
#define NAMED_ASM_ARGS 1 // GCC 3.1 or later
#endif
#endif

#ifndef __GNUC__
 #define inline __inline
 #ifndef __attribute__
  #define __attribute__(x) /**/
 #endif
 #pragma warning (disable:4002)
 #define attribute_used
 #define always_inline __forceinline
 #include <malloc.h>
 #define malloc(x) _aligned_malloc(x,16)
 #define memalign(a,b) _aligned_malloc(b,a)
 #define free(x) _aligned_free(x)
#else
 #define attribute_used __attribute__((used))
 #define always_inline __attribute__((__always_inline__)) inline
 #define malloc(x) __mingw_aligned_malloc(x,16)
 #define memalign(a,b) __mingw_aligned_malloc(b,a)
 #define free(x) __mingw_aligned_free(x)
#endif
