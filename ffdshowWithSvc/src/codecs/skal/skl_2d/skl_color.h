/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_color.h
 *
 * float-color
 ********************************************************/

#ifndef _SKL_COLOR_H_
#define _SKL_COLOR_H_


//////////////////////////////////////////////////////////
// -- useful macros
//////////////////////////////////////////////////////////

#define SKL_RGB_TO_332(r,g,b) (SKL_BYTE)( ((r)&0xE0)|(((g)&0xE0)>>3)|(((b)&0xC0)>>6) )
#define SKL_RGB_TO_555(r,g,b) (SKL_UINT16)( (((r)&0xF8)<<7)|(((g)&0xF8)<<2)|(((b)&0xF8)>>3) )
#define SKL_RGB_TO_565(r,g,b) (SKL_UINT16)( (((r)&0xF8)<<8)|(((g)&0xFC)<<3)|(((b)&0xF8)>>3) )
#define SKL_RGB_TO_777(r,g,b) (SKL_UINT32)( (((r)&0xFE)<<15)|(((g)&0xFE)<<7)|(((b)&0xFE)>>1) )
#define SKL_RGB_TO_888(r,g,b) (SKL_UINT32)( (((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF) )
#define SKL_RGB_777_TO_555(r,g,b) (SKL_UINT16)( (((r)&0x7C)<<8)|(((g)&0x7C)<<3)|(((b)&0x7C)>>2) )
#define SKL_RGB_777_TO_565(r,g,b) (SKL_UINT16)( (((r)&0x7C)<<9)|(((g)&0x7E)<<4)|(((b)&0x7C)>>2) )
#define SKL_RGB_565_TO_R(C) (((C)>>8)&0xf8)
#define SKL_RGB_565_TO_G(C) (((C)>>3)&0xfc)
#define SKL_RGB_565_TO_B(C) (((C)<<3)&0xf8)
#define SKL_RGB_555_TO_R(C) (((C)>>7)&0xf8)
#define SKL_RGB_555_TO_G(C) (((C)>>2)&0xf8)
#define SKL_RGB_555_TO_B(C) (((C)<<3)&0xf8)

//////////////////////////////////////////////////////////
// SKL_COLOR
//////////////////////////////////////////////////////////

typedef SKL_UINT32 SKL_ARGB;

    // useful static colors
#define SKL_COLOR_BLACK   ((SKL_ARGB)0x000000)
#define SKL_COLOR_WHITE   ((SKL_ARGB)0xffffff)
#define SKL_COLOR_BLUE    ((SKL_ARGB)0x0000ff)
#define SKL_COLOR_GREEN   ((SKL_ARGB)0x00ff00)
#define SKL_COLOR_RED     ((SKL_ARGB)0xff0000)

class SKL_COLOR
{
  private:

    SKL_ARGB _Col;
    enum { A_SHFT=24, R_SHFT=16, G_SHFT=8, B_SHFT=0 };

  public:

    SKL_COLOR() {}
    SKL_COLOR(const SKL_COLOR &rhs) : _Col(rhs.Get_Color()) {}
    SKL_COLOR(const SKL_ARGB rhs) : _Col(rhs) {}
    SKL_COLOR(int R, int G, int B) : _Col((R<<R_SHFT)|(G<<G_SHFT)|(B<<B_SHFT)) {}
    SKL_COLOR(int R, int G, int B, int A) : _Col((A<<A_SHFT)|(R<<R_SHFT)|(G<<G_SHFT)|(B<<B_SHFT)) {}

    SKL_ARGB Get_Color() const { return _Col; }
    operator SKL_ARGB() const { return _Col; }
    int operator==(const SKL_COLOR &rhs) { return (_Col==rhs._Col); }

    void Set_R(int R) { _Col = (_Col & ~(0xff<<R_SHFT)) | (R<<R_SHFT); }
    void Set_G(int G) { _Col = (_Col & ~(0xff<<G_SHFT)) | (G<<G_SHFT); }
    void Set_B(int B) { _Col = (_Col & ~(0xff<<B_SHFT)) | (B<<B_SHFT); }
    void Set_A(int A) { _Col = (_Col & ~(0xff<<A_SHFT)) | (A<<A_SHFT); }

    void From_RGB(int R, int G, int B, int A=0) {
      _Col = (A<<A_SHFT) | (R<<R_SHFT) | (G<<G_SHFT) | (B<<B_SHFT);
    }
    void From_RGB_Bound(int R, int G, int B) {
      if (R>256) R=255; else if (R<0) R=0;
      if (G>256) G=255; else if (G<0) G=0;
      if (B>256) B=255; else if (B<0) B=0;
      _Col = (R<<R_SHFT) | (G<<G_SHFT) | (B<<B_SHFT);
    }

    void To_RGB(int &R, int &G, int &B) const {
      R = (_Col>>R_SHFT)&0xff;
      G = (_Col>>G_SHFT)&0xff;
      B = (_Col>>B_SHFT)&0xff;
    }
    void To_ARGB(int &R, int &G, int &B, int &A) const {
      To_RGB(R,G,B);
      A = (_Col>>A_SHFT)&0xff;
    }

    int R() const { return (_Col>>R_SHFT)&0xff; }
    int G() const { return (_Col>>G_SHFT)&0xff; }
    int B() const { return (_Col>>B_SHFT)&0xff; }
    int A() const { return (_Col>>A_SHFT)&0xff; }

    SKL_COLOR Mix(const SKL_COLOR Col, float x) const {
      int R1,G1,B1, R2,G2,B2;
      To_RGB(R1,G1,B1); Col.To_RGB(R2,G2,B2);
      R1 = (int)(1.0f*R1 + x*(R2-R1))&0xff;
      G1 = (int)(1.0f*G1 + x*(G2-G1))&0xff;
      B1 = (int)(1.0f*B1 + x*(B2-B1))&0xff;
      return SKL_COLOR(R1,G1,B1);
    };

    int Dist_Squared(const SKL_COLOR Col) const;
    int Dist_Squared_ARGB(const SKL_COLOR Col) const;
    int Dist_Squared(int R, int G, int B, int A=0) const;

    float Y() const  { return  0.29900f*R() + 0.58700f*G() + 0.11400f*B(); }
    float Cb() const { return -0.16874f*R() - 0.33126f*G() + 0.50000f*B() + 128.0f; }
    float Cr() const { return  0.50000f*R() - 0.41869f*G() - 0.08131f*B() + 128.0f; }
    static float R_From_Ycc(int Y, int /*Cb*/, int Cr) { return 1.0f*Y               + 1.40200f*Cr; }
    static float G_From_Ycc(int Y, int Cb, int Cr) { return 1.0f*Y - 0.34414f*Cb - 0.71414f*Cr; }
    static float B_From_Ycc(int Y, int Cb, int /*Cr*/) { return 1.0f*Y + 1.77200f*Cb; }

#if 0
    SKL_COLOR operator +(const SKL_COLOR c) const
      { return SKL_COLOR(R()+c.R(),G()+c.G(),B()+c.B()); }
    SKL_COLOR operator -(const SKL_COLOR c) const
      { return SKL_COLOR(R()-c.R(),G()-c.G(),B()-c.B()); }
    SKL_COLOR operator *(const SKL_COLOR c) const
      { return SKL_COLOR(R()*c.R(),G()*c.G(),B()*c.B()); }
#endif
};


//////////////////////////////////////////////////////////
// SKL_FCOLOR
//////////////////////////////////////////////////////////

class SKL_FCOLOR
{
  protected:

    float _R, _G, _B, _A;

  public:

    SKL_FCOLOR() {}
    SKL_FCOLOR(const SKL_FCOLOR &c) : _R(c.R()), _G(c.G()), _B(c.B()), _A(c.A()) {}
    SKL_FCOLOR(const SKL_COLOR &c) : _R(c.R()/256.0f), _G(c.G()/256.0f), _B(c.B()/256.0f), _A(c.A()/256.0f) {}
    SKL_FCOLOR(int R, int G, int B, int A=0) : _R(R/256.0f), _G(G/256.0f), _B(B/256.0f), _A(A/256.0f) {}
    SKL_FCOLOR(float R, float G, float B, float A=0) : _R(R), _G(G), _B(B), _A(A) {}

    int operator==(const SKL_FCOLOR &c) { return ( (_R==c.B()) && (_G==c.G()) && (_B==c.B())&& (_A==c.A()) ); }
    void From_RGB_Bound(int R, int G, int B) {
      if (R>256) _R=1.0f; else if (R<0) _R=0.0; else _R = R/256.0f;
      if (G>256) _G=1.0f; else if (G<0) _G=0.0; else _G = G/256.0f;
      if (B>256) _B=1.0f; else if (B<0) _B=0.0; else _B = B/256.0f;
    }
    void Set_R(float R) { _R = R; }
    void Set_G(float G) { _G = G; }
    void Set_B(float B) { _B = B; }
    void Set_A(float A) { _A = A; }

    void To_RGB(int &R, int &G, int &B) const {
      R = (int)(_R*255.9f);
      G = (int)(_G*255.9f);
      B = (int)(_B*255.9f);
    }
    void To_ARGB(int &R, int &G, int &B, int &A) const {
      To_RGB(R,G,B);
      A = (int)(_A*255.9f);
    }
    float R() const { return _R; }
    float G() const { return _G; }
    float B() const { return _B; }
    float A() const { return _A; }

    SKL_UINT32 iR() const { return (SKL_UINT32)(_R*255.9f); }
    SKL_UINT32 iG() const { return (SKL_UINT32)(_G*255.9f); }
    SKL_UINT32 iB() const { return (SKL_UINT32)(_B*255.9f); }
    SKL_UINT32 iA() const { return (SKL_UINT32)(_A*255.9f); }

    SKL_FCOLOR Mix(const SKL_FCOLOR Col, const float x) const {
      float r = 1.0f*R() + x*(Col.R()-R());
      float g = 1.0f*G() + x*(Col.G()-G());
      float b = 1.0f*B() + x*(Col.B()-B());
      return SKL_FCOLOR(r, g, b);
    };
//    int Dist_Squared(const SKL_FCOLOR Col) const;
//    int Dist_Squared_ARGB(const SKL_FCOLOR Col) const;
//    int Dist_Squared(int R, int G, int B, int A=0) const;

    float Y() const { return 0.29900f*R() + 0.58700f*G() + 0.11400f*B(); }
    float Cb() const { return -0.16874f*R() - 0.33126f*G() + 0.50000f*B() + 128.0f; }
    float Cr() const { return 0.50000f*R() - 0.41869f*G() - 0.08131f*B() + 128.0f; }

#if 1
    SKL_FCOLOR operator +(const SKL_FCOLOR c) const
      { return SKL_FCOLOR(R()+c.R(),G()+c.G(),B()+c.B()); }
    SKL_FCOLOR operator -(const SKL_FCOLOR c) const
      { return SKL_FCOLOR(R()-c.R(),G()-c.G(),B()-c.B()); }
    SKL_FCOLOR operator *(const SKL_FCOLOR c) const
      { return SKL_FCOLOR(R()*c.R(),G()*c.G(),B()*c.B()); }
    SKL_FCOLOR operator *(const float x) const
      { return SKL_FCOLOR(R()*x,G()*x,B()*x); }
#endif
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_COLOR_H_ */
