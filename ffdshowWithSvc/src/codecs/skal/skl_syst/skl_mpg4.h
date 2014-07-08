/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_mpg4.h
 *
 *   Main MPEG4 header
 ************************************************/

#ifndef _SKL_MPG4_H_
#define _SKL_MPG4_H_

#include "skl.h"
#include "skl_syst/skl_cpu_specs.h"

//////////////////////////////////////////////////////////

struct SKL_MP4_ENC;
struct SKL_MP4_DEC;
class  SKL_MP4_ANALYZER;
struct SKL_MP4_INFOS;       // infos passed to analyzer
class  SKL_MEM_I;           // memory pool interface
struct SKL_MP4_PIC;         // visible YUV picture

typedef struct SKL_IMG_DSP SKL_IMG_DSP; // image helper funcs
typedef struct SKL_MB_DSP  SKL_MB_DSP;  // macroblock helper funcs
typedef struct SKL_GMC_DSP SKL_GMC_DSP; // GMC helper funcs
typedef struct SKL_METRIC_DSP SKL_METRIC_DSP;

//////////////////////////////////////////////////////////
// Macroblock types & MV macros
//  shared by encoder and analyzer
//

#define SKL_MB_INTER   0
#define SKL_MB_INTER_Q 1
#define SKL_MB_INTER4V 2
#define SKL_MB_INTRA   3
#define SKL_MB_INTRA_Q 4
#define SKL_MB_SKIPPED 6
#define SKL_MB_LAST    7

typedef SKL_INT16 SKL_MV[2];

  // some compilers don't like copying typedef'd arrays...
#define SKL_COPY_MV(a,b)  ((a)[0]=(b)[0], (a)[1]=(b)[1])
#define SKL_ZERO_MV(a)    ((a)[0]=0, (a)[1]=0 )

  // useful macros
#define SKL_IS_ZERO_MV(MV) ((*(SKL_UINT32*)(MV)) == 0x00000000)
#define SKL_IS_SAME_MV(MV1,MV2) (*(SKL_UINT32*)(MV1) == *(SKL_UINT32*)(MV2))

  /** @file */
  /** @var SKL_MP4_SLICER
      Hook function for pre/post processing of pictures. <p>
      This function is called after a macroblock row has been decoded,
      or just before encoding a row. The vertical row position is given
      by the parameter 'y', and its height is given by the parameter 'Height'.<p>
      In addition, this hook is called two more times at the start and
      end of picture scan. In this case, the 'Height' parameter is zero,
      and 'y' is either equal to 0 or to picture's Height.
      @see SKL_MP4_ENC
      @see SKL_MP4_DEC
    */
typedef void (*SKL_MP4_SLICER)(const SKL_MP4_PIC *Pic,
                               int y, int Height, SKL_ANY Data);

//////////////////////////////////////////////////////////

extern "C" {

  /** @internal
      Macroblock types for SKL_MP4_MAP::Type attribute, used
      by the analyzer. */
#define SKL_MAP_SKIPPED 0
#define SKL_MAP_INTRA   1   // for SKL_MB_INTRA
#define SKL_MAP_GMC     2   // real GMC (not SKIPPED)
#define SKL_MAP_16x16   3   // for SKL_MB_INTER
#define SKL_MAP_16x8    4   // ..
#define SKL_MAP_8x8     5   // ..
#define SKL_MAP_LAST    6


  /** Structure for storing analyzer's result */
struct SKL_MP4_MAP {
  SKL_UINT8 Type;        /**< 0: skipped <br>   1: INTRA (16x16) <br>
                             INTER:  2:16x16, 3:16x8 (field pred), 4:8x8 (4v)
                                    5: GMC (~16x16) <br>*/
  SKL_UINT8  Fields;     /**< field dirs (used for INTER 16x8 and B-DIRECT)  */
  SKL_INT8   dQ;         /**< delta quant (used for INTER 16x8 and B-DIRECT) */
  SKL_INT8   Flags;      /**< Encoder Flags. bit 0: use field DCT */
  SKL_UINT32 Sad;        /**< Storage of the macroblock's final SAD */

  SKL_UINT32 Sad16;      /**< SAD after 16x16 full-pel search */
  SKL_MV     MV;         /**< Motion vector after 16x16 full-pel search */
  SKL_MV     Acc;        /**< Tentative value for acceleration vector */
};

  /** @class SKL_MP4_PIC
      This class is used for storing input/output frame informations
      such as dimensions, YUV data, time stamps...<p>
      <b>Warning</b> : The size of luma component is Width x Height,
      whereas the U and V components are of size Width/2 x Height/2 each.<br>
      However, <b>all</b> of the Y, U and V component planes share the
      same stride, given by BpS (Bytes Per Scanline).<br>
      Here is an example of the layout, with internal hidden edges
      shown in blue:<br>
      <img src="../mp4_yuv.jpg">
      Note: This video format is often called "IMC2".
      <p>
    */
struct SKL_MP4_PIC
{
  SKL_BYTE *Y;    /**< Pointer to the Y-plane data, of size Width x Height, with stride BpS */
  SKL_BYTE *U;    /**< Pointer to the U-plane data, of size Width/2 x Height/2, with stride BpS */
  SKL_BYTE *V;    /**< Pointer to the V plane data, of size Width/2 x Height/2, with stride BpS */

  int Coding;            /**< Coding type. Set by the encoder/decoder. Values: 0:key frame (I-VOP), 1:P-VOP, 2:B-VOP, 3:S/D-VOP, 4: not coded */
  int Width;             /**< Width of picture, in pixel. */
  int Height;            /**< Height of picture, in pixel. */
  int BpS;               /**< Bytes Per Scanline */
  SKL_MV   *MV;          /**< Motion Vectors */
  double Time;           /**< Presentation time, in seconds */
  SKL_UINT64 Time_Ticks; /**< Presentation time, in ticks */
  SKL_MP4_MAP *Map;      /**< Auxiliary informations, for the analyzer. */
  void        *Data;     /**< (Opaque) Any data pertaining to the analyzer */

    /** @internal helper function */
  void Set_Data(void *d) const { ((SKL_MP4_PIC*)this)->Data = d; }
};

  /** proxies */

  /** @internal signatures for easy cast while importing from .dll */
typedef SKL_MP4_ENC *(*SKL_MP4_NEW_ENC)();
typedef void (*SKL_MP4_DELETE_ENC)(SKL_MP4_ENC *);
typedef SKL_MP4_DEC *(*SKL_MP4_NEW_DEC)();
typedef void (*SKL_MP4_DELETE_DEC)(SKL_MP4_DEC *);

  /** Factory for instantiating SKL_MP4_ENC class */
extern SKL_EXPORT SKL_MP4_ENC *Skl_MP4_New_Encoder();
  /** Factory for deleting SKL_MP4_ENC class instances */
extern SKL_EXPORT void Skl_MP4_Delete_Encoder(SKL_MP4_ENC *);

  /** Factory for instantiating SKL_MP4_DEC class */
extern SKL_EXPORT SKL_MP4_DEC *Skl_MP4_New_Decoder();
  /** Factory for deleting SKL_MP4_DEC class instances */
extern SKL_EXPORT void Skl_MP4_Delete_Decoder(SKL_MP4_DEC *);

}

//////////////////////////////////////////////////////////
// SKL_MP4_DEC : decoder class
//////////////////////////////////////////////////////////

 /** @class SKL_MP4_DEC
    SKL_MP4_DEC is the main decoding class.

    All methods are virtual, just like the COM-object
    paradigm. It eases the dynamic class loading
    (just map the proxy's symbol from dll, the vtbl
    will come along).
  */

struct SKL_MP4_DEC
{
  protected:
    SKL_MP4_DEC();            /**< protected constructor. use Skl_MP4_New_Decoder() */
    virtual ~SKL_MP4_DEC();   /**< protected destructor. use Skl_MP4_Delete_Decoder() */
    friend SKL_MP4_DEC *Skl_MP4_New_Decoder();
    friend void Skl_MP4_Delete_Decoder(SKL_MP4_DEC*);

  public:
      /** This function decodes 'Len' bytes of bitstream pointed to by 'Buf'.
          It returns the number of bytes consumed from the bitstream.
          For safety, it is required that the buffer contains at least a full chunk
          of VOP data (start code 0x000001b6) within it boundary. This is to ensure
          complete parsing of syntax elements.<p>
          If the 'Len' argument is 0, the last decoded frame will be removed from
          the decoder and made available for Consume_Frame(). Keep in mind that the decoder
          has a latency of one frame, so that inbetween incoming B-VOP can be
          decoded using future prediction. Hence, call with Len=0 should be performed
          at the end of the bitstream decoding, when you are sure that no frame
          are left to decode. <p>
          @param Buf the bitstream's bytes
          @param Len the number of bytes available. If equal to zero, the very
          last residual frame, will be made available.
        */
    virtual int Decode(const SKL_BYTE *Buf, int Len) = 0;
      /** This function is the equivalent of Decode(), for partial MPEG1/2 decoding function.
          It only supports FRAME pic struct/4:2:0 format. You must call it when you
          are sure the bitstream is a MPEG1/2 one (no autodetection is available).
          @see Decode
        */
    virtual int Decode_MPEG12(const SKL_BYTE *Buf, int Len) = 0;

      /** Returns the current frame's number (if available).
          @see Is_Frame_Ready
          @see Decode */
    virtual int Get_Frame_Number() const = 0;
      /** Returns true if the current frame is finished decoding
          @see Decode */
    virtual int Is_Frame_Ready() const = 0;
      /** Definitively retreive the last decoded frame. The data will remain
          valid until the next call to Decode() or Decode_MPEG12().
          <b>Warning</b> : beware of the SKL_MP4_PIC data layout!
          @param Pic the structure  to be filled with last frame's data.
          @see Decode
          @see SKL_MP4_PIC
        */
    virtual void Consume_Frame(SKL_MP4_PIC *Pic) = 0;

      /** Set a new memory pool to use internally for subsequent memory allocation.
          Warning: it should preferably be called at start-up only,
          to ensure that cycles of New()/Delete() are performed on the
          same memory pool.
          @param Mem The memory pool to use. If equal to 0 (default),
          the C++ heap will be used.
          @return This function will return the previous memory manager.
          @see SKL_MEM_I */
    virtual SKL_MEM_I *Set_Memory_Manager(SKL_MEM_I *Mem=0) = 0;

      /** Sets the CPU feature to use.
          @param Cpu : see the enum description for a list of available
          cpu features. */
    virtual void Set_CPU(SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT) = 0;

       /** Set post-decoding hook function.
           @ see SKL_MP4_SLICER
           @param Slicer callback function to use
           @param Slicer_Data can be anything, and will be passed as is
           to the Slicer hook function. */
    virtual void Set_Slicer(SKL_MP4_SLICER Slicer, SKL_ANY Slicer_Data=0) = 0;

      /** @internal Sets the internal debug level. */
    virtual void Set_Debug_Level(int Level=0) = 0;

      /** @internal
          For debug only. Returns all the Y/U/V planes in a single, edged, frame
          @param Pic the structure filled with frame's pixels  */
    virtual void Get_All_Frames(SKL_MP4_PIC *Pic) const = 0;

     /** @internal unused for now. */
    virtual int Ioctl(SKL_CST_STRING Param) = 0;
};

//////////////////////////////////////////////////////////
//  SKL_MP4_ENC : encoder class
//////////////////////////////////////////////////////////

 /** @class SKL_MP4_ENC

    SKL_MP4_ENC is the main encoding class. It is responsible for
    the bitstream coding only (that is: enforcing the syntax).
    The motion estimation is performed by an external module
    (SKL_MP4_ANALYZER), and parameter exchange is done using
    Get_Param().

    All methods are virtual, just like the COM-object
    paradigm. It eases the dynamic class loading
    (just map the proxy's symbol from dll, the vtbl
    will come along).

    @see SKL_MP4_ANALYZER.
  */

struct SKL_MP4_ENC
{
  protected:

    SKL_MP4_ENC(); /**< protected constructor. Use Skl_MP4_New_Encoder() */
    virtual ~SKL_MP4_ENC();  /**< protected destructor. use Skl_MP4_Delete_Encoder() */
    friend SKL_MP4_ENC *Skl_MP4_New_Encoder();
    friend void Skl_MP4_Delete_Encoder(SKL_MP4_ENC*);

  public:
      /** Set the dimensions of the next input frame. A SKL_MP4_PIC structure
          is returned, ready for holding fresh data.<p>
          <b>Warning</b> : beware of the SKL_MP4_PIC data layout!
          @param Width the width, in pixel, of the input picture.
          @param Height the height, in pixel, of the input picture.
          @return This functions returns a uninitialized picture structure
          SKL_MP4_PIC, ready to be filled with incoming YUV data.
          @see SKL_MP4_PIC
        */
    virtual const SKL_MP4_PIC *Prepare_Next_Frame(int Width, int Height) = 0;

      /** Return the last input frame set up by previous call to Prepare_Next_Frame().
          This data is only valid until next call to Encode()
          <b>Warning</b> : beware of the SKL_MP4_PIC data layout!
          @return Last available input frame.
          @see SKL_MP4_PIC
        */
    virtual const SKL_MP4_PIC *Get_Next_Frame() const = 0;

      /** Return the last coded frame, if any.
          This data is only valid until next call to Encode()
          <b>Warning</b> : beware of the SKL_MP4_PIC data layout!
          @return Last coded frame.
          @see SKL_MP4_PIC
        */
    virtual const SKL_MP4_PIC *Get_Last_Coded_Frame() const = 0;

      /** Encode next input frame, as set up by call to Prepare_Next_Frame.
          @return the number of coded bytes available for writting in the buffer is returned.
          @see Get_Bits, Get_Bits_Length, Prepare_Next_Frame. */
    virtual int Encode() = 0;

      /** Mark the end of sequence in the bitstream. Returns number of coded bytes.
          @see Get_Bits, Get_Bits_Length. */
    virtual int Finish_Encoding() = 0;

      /** Returns the pointer to the coded bytes after last call to Encode().
          This pointer and its content is only valid until next call to Encode().
          The number of bytes is available using the Get_Bits_Length() member function.
          @see Encode, Get_Bits_Length */
    virtual const SKL_BYTE *Get_Bits() const = 0;

      /** Returns the number of coded bytes after last call to Encode().
          This value is only valid until next call to Encode().
          The pointer to these bytes is available using the Get_Bits() member function.
          @see Encode, Get_Bits */
    virtual int Get_Bits_Length() const = 0;

      /** Set a new memory pool to use internally for subsequent memory allocation.
          Warning: it should preferably be called at start-up only,
          to ensure that cycles of New()/Delete() are performed on the
          same memory pool.
          @param Mem The memory pool to use. If equal to 0 (default),
          the C++ heap will be used.
          @return This function will return the previous memory manager.
          @see SKL_MEM_I */
    virtual SKL_MEM_I *Set_Memory_Manager(SKL_MEM_I *Mem=0) = 0;

      /** Sets the CPU feature to use.
          @param Cpu : see the enum description for a list of available
          cpu features. */
    virtual void Set_CPU(SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT) = 0;

      /** Sets the custom quantization matrix for MPEG4 quantization type (0).
          @param Intra Should be 1 to use the custom matrix for intra macroblocks, or 0
          for inter macroblocks.
          @param M a pointer to 64 bytes containing the 8x8 matrix coefficients. If
          the pointer is null, the coefficients are restored to their default, initial,
          values. */
    virtual void Set_Custom_Matrix(int Intra, const SKL_BYTE *M=0) = 0;

      /** This function plugs a new analyzer code in replacement of
          the previous one.
          @return Set_Analyzer() returns previous analyzer.
          @param Analyzer Should be 0 to restore the default, built-in, analyzer.
          @see Get_analyzer
          @see SKL_MP4_ANALYZER */
    virtual SKL_MP4_ANALYZER *Set_Analyzer(SKL_MP4_ANALYZER *Analyzer=0) = 0;

      /** Returns the analyzer currently associated with the encoder.
          @see Set_analyzer */
    virtual SKL_MP4_ANALYZER *Get_Analyzer() const = 0;

       /** Set post-coding hook function. The signature of this function
           is the following typedef: <p>
           typedef void (*SKL_MP4_SLICER)(const SKL_MP4_PIC *Pic, int y, int Height, SKL_ANY Data);
           @param Slicer callback function to use
           @param Slicer_Data can be anything, and will be passed as is
           to the Slicer hook function. */
    virtual void Set_Slicer(SKL_MP4_SLICER Slicer, SKL_ANY Slicer_Data=0) = 0;

      /** @internal
          For debug only. Returns all the Y/U/V planes in a single, edged, frame
          @param Pic the structure filled with frame's pixels  */
    virtual void Get_All_Frames(SKL_MP4_PIC *Pic) const = 0;

      /** @internal Sets the internal debug level. */
    virtual void Set_Debug_Level(int Level=0) = 0;

     /** This method can be used for very special syntax controls over the bitstream.
        @param Param can be one of the following constant strings:<br>
        <ul>
        <li>"emit-key-headers": for streaming purpose, the bitstream will contain
        a VOL header every time a keyframe is transmitted.<br></li>
        <li>"no-emit-key-headers": disables the above (default).<br></li>
        <li>"emit-sequence-codes": the bitstream will contain a SEQUENCE_START and
        SEQUENCE_END codes at the beginning and the end.<br></li>
        <li>"no-sequence-codes": disables the above (default).<br></li>
        </ul>
        Note: Call to this method should occur preferably at the beginning of
        the encoding process.
       */
    virtual int Ioctl(SKL_CST_STRING Param) = 0;
};


//////////////////////////////////////////////////////////
//  SKL_MP4_ANALYZER
// Frame analyzers to be plugged into SKL_MP4_ENC
//////////////////////////////////////////////////////////

  /** @struct SKL_MP4_INFOS
      SKL_MP4_INFOS is used to share informations between
      the bitstream-coding core (instance of SKL_MP4_ENC)
      and the frame analyzer (instance of SKL_MP4_ANALYZER)
    */
struct SKL_MP4_INFOS
{
  SKL_MP4_INFOS(SKL_MEM_I *Memory=0);

  SKL_MEM_I *Mem;                                   /**< Memory pool */
  SKL_MEM_I *Set_Memory_Manager(SKL_MEM_I *mem);    /**< mem=0 => use C++ heap */

    /** public infos of general interest */

  int Width;             /**< Width of picture, in pixel. */
  int Height;            /**< Height of picture, in pixel. */
  int BpS;               /**< Bytes Per Scanline */
  int Frame_Number;      /**< Frame number in display order. */

  int MB_W;              /**< Width of padded picture, in macroblock units */
  int MB_H;              /**< Height of padded picture, in macroblock units */
  int MV_Stride;         /**< stride for motion vectors array */

  int Texture_Bits;
  int MV_Bits;
  int Coded_Bits;

  SKL_MP4_PIC *Past;     /**< The past picture use for reference */
  SKL_MP4_PIC *Cur;      /**< The current picture being motion-searched */
  SKL_MP4_PIC *Future;   /**< The future reference picture, if applicable (B-VOP). */

  const SKL_IMG_DSP *Img_Dsp;   /**< Image low-level DSP functions */
  const SKL_MB_DSP  *MB_Dsp;    /**< Macroblock low-level DSP functions */
  SKL_GMC_DSP       *GMC_Dsp;   /**< GMC low-level DSP functions */
};

  /** @class SKL_MP4_ANALYZER
      Base class for analyzing input, making coding decisions
      and performing bit-rate control. Actually drives
      the bitstream-coder (SKL_MP4_ENC).
      Setting/Access/Control of internal parameter is granted
      through the Set_Param() and Get_Param() functions.
      See description of this function for details. We encourage you
      to have a look at the example 'tmp4.cpp', that exercises every
      possible params of Set_Param() through the command-line options.

      @see SKL_MP4_ENC
    */
class SKL_MP4_ANALYZER
{
  protected:

    SKL_MEM_I *_Mem;       /**< @internal Memory pool, for allocating auxiliary data, e.g. */

  public:

    SKL_MP4_ANALYZER(SKL_MEM_I *Mem=0) : _Mem(Mem) {}
    virtual ~SKL_MP4_ANALYZER() {}


      /** This function is called when the invoked analyzer is about to be enabled. */
    virtual void Wake_Up(SKL_MP4_ANALYZER *Previous) = 0;
      /** this function is called when the invoked analyzer is about to be disabled. */
    virtual void Shut_Down() = 0;

      /** This function is responsible for motion estimation.
          It should populate the array of motion
          vectors MVs[] and the macroblock map *Map with the
          desired macroblock types.

          @return
          return code: 0: intra coding required (I-VOP)
                       1/2: predictive coding required (1=P or 2=B-VOP)
                       3: GMC coding requested
       */
    virtual int Analyze(SKL_MP4_INFOS * const Frame) = 0;

      /** This function handles variable per-macroblocks quantization.

          New->Map[].dQ should be filled with dQ hints.
          This is a separate virtual, not merged with the above Analyze()
          motion estimation, since the coding type can be changed by the
          encoder inbetween. This impacts the range of allowed dQ
          values (eg.: for B-VOP).
        */
    virtual void Analyze_dQ(SKL_MP4_INFOS * const Frame, int For_BVOP) = 0;


      /** Post-coding update (for rate control, e.g.)
          @param Infos contains informations about the last coded frame
       */
    virtual void Post_Coding_Update(const SKL_MP4_INFOS * const Infos) = 0;

      /** Sets the value of the integer parameter described by Param string.
          These parameter settings control the core of the codec during
          motion estimation and bitstream coding.
          @return 0 if parameter is not applicable, or -1 if Value has invalid range.
          @param Param can be one of the following constant strings:<br>

          <ul>
          <li>"buffer-size": If non-zero, specify the size of coded bits buffer to allocate for each frame coding.<br></li>

          <li>"quant": Sets the main quantizer (the lower, the higher the quality).
          Should be in range [1..31]. When variable bitrate is used, this value is
          internally adjusted after each frame coding, while trying to meet the
          desired bitrate.<br></li>
          <li>"base-quant": Similar to "quant" parameter, but sets the quantizer of key-frames.
          By default, key-frames are assumed to share the global quantizer, if this parameter
          is not set.<br></li>
          <li>"quant-type": Quantization method used: 0 for H263's, 1 for MPEG4's.<br></li>

          <li>"hi-mem": Consumes more memory for faster motion search. Only useful for Quarter Pixel.</li>

          <li>"intra-limit": Percentage of INTRA macroblock necessary to trigger a scene
          change (with key frame signaled).<br></li>
          <li>"intra-max-delay": Maximum allowed number of P-frames between each INTRA key-frame.<br></li>
          <li>"intra-max-count": Counter for "intra-max-delay". When it reaches 0, an INTRA frame is emitted.<br></li>

          <li>"rounding": Block-interpolation rounding mode: 0 (down) or 1 (up)<br></li>
          <li>"subpixel": Subpixel precision to use: 0=full-pixel, 1=half-pixel (default), 2=quarter pixel.<br></li>
          <li>"use-trellis": Switch trellis-based quantization on (non-zero Value) or off (Value=0)<br></li>

          <li>"search-size": size in 16-pixels units of the search window. When
          bitrate control is on, this size is changed adaptively.<br></li>
          <li>"base-search-size": Similar to "search-size", this is the default value
          used as starting point after each key-frame.<br></li>

          <li>"search-method": Internal searcho method (0 to 5)<br></li>
          <li>"search-metric": Metric for evaluating match: 0=SAD (default), 1=SSD, 2=Hadamard.<br></li>

          <li>"4v-probing": Worthiness Threshold (as percentage) of 4-motion-vector blocks to probe.<br></li>

          <li>"sad-skip-limit": Metric threshold below which a block is SKIPed <br></li>
          <li>"sad-intra-limit": Metric threshold above which a block is coded INTRA<br></li>
          <li>"inter-threshold": Amplitude of INTER coefficient below which a sub-block is skipped (using Coded Block Pattern)<br></li>

          <li>"bitrate": If non-zero, turns (simplistic) bitrate control on. It then will adjust "quant", "search-size"
          parameter, and use the "framerate" setting too.<br></li>

          <li>"field-pred-probing": Worthiness Threshold (as percentage) for field/frame block decision.<br></li>
          <li>"interlace-field": Use interlace-block coding if non-zero<br></li>
          <li>"interlace-dct": Use interlace-block DCT coding of coefficient, if non-zero<br></li>

          <li>"gmc-pts": Number of GMC points to use (0 to 3)<br></li>
          <li>"gmc-mode": If non-zero, turns GMC estimation on (experimental)<br></li>
          <li>"gmc-accuracy": GMC interpolation precision (0 to 3)<br></li>

          <li>"reduced-frame": Use reduced-frame coding, if non-zero<br></li>
          <li>"luminance-masking": Switch luminance-masking on (non-zero Value) or off (Value=0)<br></li>

          <li>"frequency": Frequency of frame-tick resets.<br></li>
          <li>"frame-ticks": Precision of frame-tick coding<br></li>

          </ul>
          @param Value the parameter value to set.

          Note: call to Set_Param() can occur any time during the encoding process
          and will take effect immediately during next call to Encode(). In particular,
          this method can be used during two-pass encoding to restore modified parameters
          stored in a first-pass file.
        */
    virtual int Set_Param(const char * const Param, int Value) = 0;

      /** Sets the value of the float parameter described by Param string.
          These parameter settings control the core of the codec during
          motion estimation and bitstream coding.
          @return 0 if parameter is not applicable, or -1 if Value has invalid range.
          @param Param can be one of the following constant strings:<br>

          <ul>
          <li>"dquant-amp": If non-zero, turns luminance-masking on, with the specified Amplitude (the higher, the higher the effect, and the lower the quality).<br></li>
          <li>"framerate": Number of frame-per-seconds to achieve when bit-rate control is on.<br></li>
          <li>"quant": Specify the quantizer as a float value.<br></li>
          <li>"base-quant": Specify the base quantizer as a float value.<br></li>
          <li>"search-size": Specify the search-window size as a float value.<br></li>
          <li>"base-search-size":Specify the base search-window size as a float value.<br></li>
          </ul>

          Note: call to Set_Param() can occur any time during the encoding process
          and will take effect immediately during next call to Encode(). In particular,
          this method can be used during two-pass encoding to restore modified parameters
          stored in a first-pass file.
          @param Value the parameter value to set.
        */
    virtual int Set_Param(const char * const Param, float Value) = 0;

      /** Sets the value of the string parameter described by Param string.
          @return 0 if parameter is not applicable, or -1 if Value has invalid range.
          @param Param can be one of the following constant strings:<br>

          <ul>
          <li>"passfile": Name of log file to use for 2-pass bitrate control. This parameter
          should be set before encoding process begins.<br></li>
          </ul>

          @param Value the string parameter value to set.
        */
    virtual int Set_Param(const char * const Param, const char * const Value) = 0;

      /** Returns the current value of the integer parameter described by Param string.
          @return 0 if parameter is not applicable.
          @see Set_Param()
          @param Value
          @param Param This string can be one of the corresponding ones passed to Set_Param().
          In addition, there are some non-settable parameters, whose values can be
          accessed using the following string:
          <ul>
          <li>"fcode": Desired typical length of motion vectors (bitstream syntax).<br></li>
          <li>"version": the version of the core (analyzer)<br></li>
          </ul>
          @param Value pointer the parameter value.
        */
    virtual int Get_Param(const char * const Param, int *Value) const = 0;

      /** Returns the current value of the float parameter described by Param string.
          @return 0 if parameter is not applicable.
          @see Set_Param()
          @param Param: This string can be one of the corresponding ones passed to Set_Param().
          @param Value pointer the parameter value.
        */
    virtual int Get_Param(const char * const Param, float *Value) const = 0;

      /** Returns the current value of the string parameter described by Param string.
          @return 0 if parameter is not applicable.
          @see Set_Param()
          @param Param: This string can be one of the corresponding ones passed to Set_Param().
          @param Value pointer the parameter value.
        */
    virtual int Get_Param(const char * const Param, const char ** const Value) const = 0;

      /** Get read-only internal data. Returns NULL is not applicable. This
          is mainly for internal use. */
    virtual const int *Get_Param(const char * const Param) const = 0;

      /** Set a new memory pool to use internally for subsequent memory allocation.
          Warning: it should preferably be called at start-up only,
          to ensure that cycles of New()/Delete() are performed on the
          same memory pool.
          @param Mem The memory pool to use. If equal to 0 (default),
          the C++ heap will be used.
          @return This function will return the previous memory manager.
          @see SKL_MEM_I
          @see Get_Mem() */
    virtual SKL_MEM_I *Set_Memory_Manager(SKL_MEM_I *Mem=0) = 0;
      /** This function returns the current memory pool, as set by
        Set_Memory_Manager()
        @see Get_Memory_Manager()
        @see SKL_MEM_I  */
    SKL_MEM_I *Get_Mem() const { return _Mem; }

     /** @internal prints analyzer's capabilities.  */
    virtual void Print_Caps() = 0;
};

  /** factory for default Analyzer. Note that it is already assigned by default
      to each SKL_MP4_ENC instances. You need not set another one. */
extern SKL_EXPORT SKL_MP4_ANALYZER *Skl_MP4_Get_Default_Analyzer(SKL_MEM_I *Mem);

  /** Destructor of default analyzer.
      @param Analyzer Must be an instance returned by Skl_MP4_Get_Default_Analyzer */
extern SKL_EXPORT void Skl_MP4_Destroy_Default_Analyzer(SKL_MP4_ANALYZER *Analyzer);

//////////////////////////////////////////////////////////

#endif   /* _SKL_MPG4_H_ */
