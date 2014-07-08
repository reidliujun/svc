/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_bits.h
 *
 *  bitstream I/O + masking utils
 ********************************************************/

#ifndef _SKL_BITS_H_
#define _SKL_BITS_H_

//////////////////////////////////////////////////////////
// Bit-masking utils
//////////////////////////////////////////////////////////

class SKL_BMASKS {
  public:
    static const SKL_UINT32 And[33];    // (1<<n)-1
    static const SKL_UINT32 Or[33];     // 1<<(n-1)

    static int Is_Power_Of_2(int Value)
      { return (!(Value & (Value-1))); }  // wow!

    static const SKL_BYTE Bit_Sizes[256];
    static int Get_Size(int Value) {
      int Size = 0;
      while (Value>=256) { Value>>=8; Size+=8; }
      return Size + Bit_Sizes[Value];
    }
    // Get_Size() actually returns 'ceil(log2(Value))'
    // So here's a wrapper:
    static int Log2(int Value) { return Get_Size(Value); }
      // and even faster, using IEEE float representation:
    static SKL_INT32 fLog2(float v) {
      SKL_INT32 I = (SKL_INT32&)v;
      return ((I&0x7f800000)>>23) - 127; // extract un-biased 8-bit exponent
    }
    static SKL_INT32 fLog2(SKL_INT32 v) { return fLog2((float)v); }

    static const SKL_BYTE First_Bit_Set[256];
    static int Get_First_Bit_Set_16(int V) {
      SKL_ASSERT(V>0 && V<=0xffff);
      if (V>>8) return First_Bit_Set[(V>>8)];
      else return First_Bit_Set[V]+8;
    }
    static int Get_First_Bit_Set_8(int V) {
      SKL_ASSERT(V>0 && V<=0xff);
      return First_Bit_Set[V];
    }
};

//////////////////////////////////////////////////////////
// These are fast, inlined, MSBF bitstream.
// It heavily supposes you know what you're doing.
// Read/write overflow is *NOT* checked.
//////////////////////////////////////////////////////////

struct SKL_FBB {
  SKL_BYTE  *Buf;
  SKL_INT32  Bits_Left;
  SKL_UINT32 Bits;

  SKL_FBB() {}
  SKL_FBB(const SKL_BYTE *buf) { Init(buf); } // read
  SKL_FBB(SKL_BYTE *buf) { Init(buf); }       // write


  inline void Init(const SKL_BYTE *buf) {
    SKL_ASSERT(buf!=0);
    Buf       = (SKL_BYTE*)buf;
    Bits_Left = 0;
    Bits      = 0x0;
  }
  inline void Copy(const SKL_FBB *In) {
    Buf       = In->Buf;
    Bits_Left = In->Bits_Left;
    Bits      = In->Bits;
  }


    // -- loading bytes/words

  inline SKL_UINT32 Next_Byte() { return *Buf++; }
  inline SKL_UINT32 Next_Word() {
    SKL_UINT32 Val = (Buf[0]<<8) | (Buf[1]);
    Buf += 2;
    return Val;
  }
  inline void Load() {
    SKL_ASSERT(Bits_Left<=24);
    Bits_Left += 8;
    Bits |= Next_Byte() << (32-Bits_Left);
  }
  inline void Load_Word() {
    SKL_ASSERT(Bits_Left<=24);
    Bits_Left += 16;
    Bits |= Next_Word() << (32-Bits_Left);
  }

  inline void Check_Bits(int Nb) {  // unsigned
    SKL_ASSERT(Nb<=24);
    while (Bits_Left<Nb) Load();
  }
  inline void Check_Bits_Word(int Nb) {  // unsigned
    SKL_ASSERT(Nb<=16);
    if (Bits_Left<Nb) Load_Word();  // only load 1 word
  }

  inline SKL_UINT32 Get_Bits(int Nb) {  // unsigned
    Check_Bits(Nb);
    SKL_INT32 Val = Bits >> (32-Nb);
    Bits <<= Nb;
    Bits_Left -= Nb;
    return Val;
  }
  inline SKL_UINT32 Get_Bits_Word(int Nb) {  // unsigned
    Check_Bits_Word(Nb);
    SKL_INT32 Val = Bits >> (32-Nb);
    Bits <<= Nb;
    Bits_Left -= Nb;
    return Val;
  }

    // -- checking forthcoming bits

  inline SKL_UINT32 See_Bits_Safe(int Nb) const {
    SKL_ASSERT(Bits_Left>=Nb);
    return (Bits >> (32-Nb));
  }
  inline SKL_UINT32 See_Bits(int Nb) {  // unsigned
    Check_Bits(Nb);
    return (Bits >> (32-Nb));
  }
  inline SKL_UINT32 See_Bits_Word(int Nb) {  // unsigned
    Check_Bits_Word(Nb);
    return (Bits >> (32-Nb));
  }

    // -- discard, forward and backward

  inline void Discard(int Nb) {
    Check_Bits(Nb);
    Bits <<= Nb;
    Bits_Left -= Nb;
  }
  inline void Discard_Word(int Nb) {
    Check_Bits_Word(Nb);
    Bits <<= Nb;
    Bits_Left -= Nb;
  }
  inline void Discard_Safe(int Nb) {
    SKL_ASSERT(Bits_Left>=Nb);
    Bits <<= Nb;
    Bits_Left -= Nb;
  }

  inline void Rewind(int Nb)
  {
    SKL_ASSERT(Nb>=0);
    Nb += Bits_Left + 7;
    Buf -= Nb/8;
    Nb = 1 + (Nb&7);
    Bits = Next_Byte()<<(32-Nb);
    Bits_Left = Nb;
  }
  inline void Forward(int Nb)
  {
    SKL_ASSERT(Nb>=0);
    if (Nb>=Bits_Left) {
      Nb -= Bits_Left;
      Buf += Nb/8;
      Bits_Left = 8-(Nb&7);
      Bits = Next_Byte()<<(32-Nb);
    }
    else {
      Bits_Left -= Nb;
      Bits <<= Nb;
    }
  }
  inline void Skip_Bytes(int Nb) {   // warning! unsafe!
    SKL_ASSERT(Is_Aligned());
    Buf += Nb;
  }

  inline int Is_Aligned() const { return !(Bits_Left&7); }
  inline void Align() { if (Bits_Left&7) Discard_Safe( Bits_Left&7 ); }

    // -- positioning

  inline SKL_BYTE *Pos() const { return Buf - Bits_Left/8; }
  inline SKL_SAFE_INT Bit_Pos() const
    { return ((SKL_SAFE_INT)Buf)*8 - Bits_Left; }

    // Safely storing the offset in *bits* would theoretically
    // require 32+8 bits. Using a known reference (Pos0) not far
    // from current position helps avoiding overflow.
    // Note that overflow can only happen if you use a bit buffer
    // bigger than 16.7 Mo :)
  inline SKL_SAFE_INT Safe_Bit_Pos(SKL_BYTE *Pos0) const
    { return ((SKL_SAFE_INT)(Buf-Pos0)*8 - Bits_Left); }


    // -- Writing bits

  inline void Put_Bits(SKL_UINT32 Val, int Nb)
  {
    SKL_ASSERT(Nb<32 && !(Val&~SKL_BMASKS::And[Nb]));
    Bits_Left += Nb;
    if (Bits_Left>32) {
      Bits_Left -= 32;
      Bits |= Val >> Bits_Left;
      *Buf++ = ( Bits>>24 ) & 0xff;
      *Buf++ = ( Bits>>16 ) & 0xff;
      *Buf++ = ( Bits>> 8 ) & 0xff;
      *Buf++ = ( Bits>> 0 ) & 0xff;
      Bits = Val<<(32-Bits_Left);
    }
    else Bits |= Val<<(32-Bits_Left);
  }

  inline void Put_Bits0(int Nb) // emits the bit '0' Nb times
  {
    SKL_ASSERT(Nb>=0);
    Bits_Left += Nb;
    while (Bits_Left>32) {
      Bits_Left -= 32;
      *Buf++ = ( Bits>>24 ) & 0xff;
      *Buf++ = ( Bits>>16 ) & 0xff;
      *Buf++ = ( Bits>> 8 ) & 0xff;
      *Buf++ = ( Bits>> 0 ) & 0xff;
      Bits = 0;
    }
  }
  inline void Put_Bits1(int Nb) // emits the bit '1' Nb times
  {
    SKL_ASSERT(Nb>=0);
    Bits |= SKL_BMASKS::And[32-Bits_Left];
    Bits_Left += Nb;
    while (Bits_Left>32) {
      Bits_Left -= 32;
      *Buf++ = ( Bits>>24 ) & 0xff;
      *Buf++ = ( Bits>>16 ) & 0xff;
      *Buf++ = ( Bits>> 8 ) & 0xff;
      *Buf++ = ( Bits>> 0 ) & 0xff;
      Bits = 0xffffffff;
    }
    Bits &= ~SKL_BMASKS::And[32-Bits_Left];
  }

  inline void Put_Word(SKL_UINT32 Val)
  {
    SKL_ASSERT(Is_Flushed());
    *Buf++ = ( Val>>8 ) & 0xff;
    *Buf++ = ( Val>>0 ) & 0xff;
  }

  inline void Put_DWord(SKL_UINT32 Val)
  {
    SKL_ASSERT(Is_Flushed());
    *Buf++ = ( Val>>24 ) & 0xff;
    *Buf++ = ( Val>>16 ) & 0xff;
    *Buf++ = ( Val>> 8 ) & 0xff;
    *Buf++ = ( Val>> 0 ) & 0xff;
  }

  inline SKL_BYTE *Write_Pos() const { return Buf + Bits_Left/8; }
  inline SKL_SAFE_INT Write_Bit_Pos() const
    { return ((SKL_SAFE_INT)Buf)*8 + Bits_Left; }
  inline SKL_SAFE_INT Safe_Write_Bit_Pos(SKL_BYTE *Pos0) const
    { return ((SKL_SAFE_INT)(Buf-Pos0)*8 + Bits_Left); }

   // -- sync pending bits and align to byte

  inline void Flush_Write() {
    while (Bits_Left>0) {
      *Buf++ = (Bits >> 24) & 0xff;
      Bits <<= 8;
      Bits_Left -= 8;
    }
    Bits_Left = 0;
    SKL_ASSERT(Bits==0);
  }
  inline int Is_Flushed() const { return (Bits_Left==0); }


    // exponential Golomb codes

#if 1

    // Table-based not inlined version

  static const SKL_BYTE  EG_Len[512-16];
  static const SKL_UINT8 UEG_Val[512-16];
  static const SKL_INT8  SEG_Val[512-16];

  SKL_UINT32 UEG();               // unsigned
  SKL_INT32  SEG();               // signed

  static const SKL_UINT16 UEG_Tab[256][2];
  static const SKL_UINT16 SEG_Tab[256][2];

  void Put_UEG(SKL_UINT32 v);   // unsigned
  void Put_SEG(SKL_INT32 v);    // signed

#else

      // Reference, slow but robust, versions

  inline SKL_UINT32 UEG()              // unsigned
  {
    int Len = 0;
    while (!Get_Bits(1)) Len++;
    return (Len==0) ? 0 : (1<<Len)-1 + Get_Bits(Len);
  }

  inline SKL_INT32 SEG()               // signed
  {
    int Len = 0;
    while (!Get_Bits(1)) Len++;
    if (Len==0) return 0;
    SKL_INT32 Val = (1<<Len) | Get_Bits(Len);
    return (Val&1) ? -(Val>>1) : (Val>>1);
  }

  inline void Put_UEG(SKL_UINT32 v)              // unsigned
  {
    if (v) {
      v += 1;
      const int Len = SKL_BMASKS::Log2(v);
      //  Put_Bits(v, 2*Len-1);   <- this could overflow if 2*Len-1>24
      Put_Bits(0, Len-1);
      Put_Bits(v, Len  );
    }
    else Put_Bits(1,1);
  }

  inline void Put_SEG(SKL_INT32 v)               // signed
  {
    if (v) {
      int Len;
      if (v<0) v = 1-2*v;
      else     v = 2*v;
      Len = SKL_BMASKS::Log2(v);
      //  Put_Bits(v, 2*Len-1);   <- this could overflow if 2*Len-1>24
      Put_Bits(0, Len-1);
      Put_Bits(v, Len  );
    }
    else Put_Bits(1,1);
  }

#endif

    // computes code length

  static int UEG_Len(SKL_UINT32 v)              // unsigned
  {
    return 2*SKL_BMASKS::Log2(v+1) - 1;
  }

  static int SEG_Len(SKL_INT32 v)               // signed
  {
    if (v==0) return 1;
    if (v<0) v = 1-2*v;
    else     v = 2*v;
    return 2*SKL_BMASKS::Log2(v) - 1;
  }
};

//////////////////////////////////////////////////////////
// MACRO-ized version
//////////////////////////////////////////////////////////

#define SKL_FBB_DECL             SKL_BYTE *Buf; SKL_INT32 Bits_Left; SKL_UINT32 Bits
#define SKL_FBB_START(FBB)       Buf = (FBB).Buf; Bits_Left = (FBB).Bits_Left; Bits = (FBB).Bits
#define SKL_FBB_STOP(FBB)        (FBB).Buf = Buf; (FBB).Bits_Left = Bits_Left; (FBB).Bits = Bits
#define SKL_FBB_NEXT_WORD        do { Bits_Left+=16; Bits |= ((Buf[0]<<8) | Buf[1])<<(32-Bits_Left); Buf+=2; } while(0)
#define SKL_FBB_LOAD_WORD(NB)    if (Bits_Left<NB) SKL_FBB_NEXT_WORD
#define SKL_FBB_NEXT_BYTE        do { Bits_Left+=16; Bits |= Buf[1]<<(32-Bits_Left); Buf+=1; } while(0)
#define SKL_FBB_LOAD_BYTE(NB)    if (Bits_Left<NB) SKL_FBB_NEXT_BYTE
#define SKL_FBB_BITS(NB)         (Bits>>(32-NB))
#define SKL_FBB_BITS_SIGNED(NB)  (((SKL_INT32)Bits)>>(32-NB))
#define SKL_FBB_CHECK(NB)        while (Bits_Left<NB) SKL_FBB_NEXT_BYTE
#define SKL_FBB_DISCARD(NB)      do { SKL_FBB_CHECK(NB); Bits<<=NB; Bits_Left-= NB; } while(0)
#define SKL_FBB_DISCARD_SAFE(NB) do { Bits<<=NB; Bits_Left-= NB; } while(0)

//////////////////////////////////////////////////////////

#endif  /* _SKL_BITS_H_ */
