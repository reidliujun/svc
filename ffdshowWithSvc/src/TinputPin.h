#ifndef _TINPUTPIN_H_
#define _TINPUTPIN_H_

#include "DeCSSInputPin.h"
#include "ffcodecs.h"

class Tcodec;
class TinputPin :public CDeCSSInputPin
{
private:
 ffstring filesourceFlnm;
 bool wasGetSourceName;
protected:
 virtual bool init(const CMediaType &mt)=0;
 virtual void done(void)=0;
 CodecID codecId;
 Tcodec *codec;
 bool strippacket;
public:
 TinputPin(const char_t* objectName,CTransformFilter *filter,HRESULT* phr,LPWSTR pinname);

 HRESULT SetMediaType(const CMediaType* mt);
 STDMETHODIMP Disconnect(void);
 STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
 STDMETHODIMP EndOfStream(void);

 int getInCodecId2(void) {return codecId;}
 virtual HRESULT getInCodecString(char_t *buf,size_t buflen)=0;
 HRESULT getStreamName(char_t *buf,size_t buflen);
 const char_t* getDecoderName(void);
 virtual HRESULT getEncoderInfo(char_t *buf,size_t buflen);
 const char_t* getFileSourceName(void);
 HRESULT connectedTo(char_t *buf,size_t buflen); // buflen : count of characters, not byte size.
 bool onSeek(REFERENCE_TIME segmentStart);
};

template<class Tpin> class TpinsVector :public array_vector<Tpin*,40>
{
private:
 typedef array_vector<Tpin*,40> Tbase;
public:
 unsigned int getNumConnectedInpins(void)
  {
   unsigned int cnt=0;
   for (typename Tbase::iterator p=this->begin();p!=this->end();p++)
    if ((*p)->IsConnected())
     cnt++;
   return cnt;
  }
 Tpin* getConnectedInpin(unsigned int ii)
  {
   for (typename Tbase::iterator p=this->begin();p!=this->end();p++)
    if ((*p)->IsConnected())
     if (ii==0)
      return *p;
     else
      ii--;
   return NULL;
  }
 Tpin* find(const wchar_t *id)
  {
   for (typename Tbase::iterator p=this->begin();p!=this->end();p++)
    if (wcscmp((*p)->Name(),id)==0)
     return *p;
   return NULL;
  }
};

#endif
