#ifndef _DSUTIL_H_
#define _DSUTIL_H_

HRESULT getFilterMerit(const CLSID &clsid,DWORD *merit);
HRESULT getFilterMerit(HKEY hive,const char_t *reg,DWORD *merit);
HRESULT setFilterMerit(const CLSID &clsid,DWORD  merit);
HRESULT setFilterMerit(HKEY hive,const char_t *reg,DWORD merit);

//from DSUtil by Gabest
bool ExtractBIH(const AM_MEDIA_TYPE &pmt,BITMAPINFOHEADER* bih);
struct Trect;
void setVIH2aspect(VIDEOINFOHEADER2 *vih2,const Trect &r,int userDAR256);

CLSID GetCLSID(IBaseFilter* pBF);
CLSID GetCLSID(IPin* pPin);

void getFilterName(IBaseFilter *filter,char_t *name,char_t *filtername,size_t bufsize); // bufsize : count of characters, not byte size.

struct pins_vector : std::vector<IPin*>
{
public:
 pins_vector(IBaseFilter *filter,PIN_DIRECTION dir);
 ~pins_vector();
};
typedef std::vector<IBaseFilter*> IbaseFilters;
template<class Tcompare> bool searchPrevNextFilter(PIN_DIRECTION direction,IPin *pin,IPin *prevpin,IBaseFilter* *dest,const Tcompare &compFc,IbaseFilters prevFilters=IbaseFilters())
{
 struct PIN_INFOex : PIN_INFO
  {
   CLSID clsid;
   operator const CLSID&() const {return clsid;}
   operator IBaseFilter*() const {return pFilter;}
  } pi;
 if (SUCCEEDED(pin->QueryPinInfo(&pi)))
  {
   if (!isIn(prevFilters,pi.pFilter))
    {
     prevFilters.push_back(pi.pFilter);
     if (SUCCEEDED(pi.pFilter->GetClassID(&pi.clsid)))
      {
       static const GUID CLSID_DEXFILT={0xc4d81942,0x607,0x11d2,0xa3,0x92,0x00,0xe0,0x29,0x1f,0x39,0x59};
       if (pi.clsid == CLSID_DEXFILT)
        {
         // Work around the compatibility problem with Windows Movie Maker.
         pi.pFilter->Release();
         return false;
        }
       if (compFc(pi,prevpin))
        {
         if (dest)
          (*dest=pi.pFilter)->AddRef();
         pi.pFilter->Release();
         return true;
        }
       pins_vector pipins(pi.pFilter,direction);
       for (pins_vector::iterator p=pipins.begin();p!=pipins.end();p++)
        {
         comptr<IPin> ipc=NULL;
         (*p)->ConnectedTo(&ipc);
         if (ipc && searchPrevNextFilter(direction,ipc,prevpin,dest,compFc,prevFilters))
          {
           pi.pFilter->Release();
           return true;
          }
        }
      }
    }
   pi.pFilter->Release();
  }
 return false;
}
bool searchPrevNextFilter(PIN_DIRECTION direction,IPin *pin,const CLSID &clsid);
bool searchFilterInterface(IFilterGraph *graph,const IID &iid,IUnknown* *dest);
bool searchPinInterface(IFilterGraph *graph,const IID &iid,IUnknown* *dest);
IBaseFilter* searchFilter(IFilterGraph *graph,const CLSID &clsid,IBaseFilter *exclude=NULL);
void bih2mediatype(const BITMAPINFOHEADER &bih,CMediaType *mt);
REFERENCE_TIME getAvgTimePerFrame(const CMediaType &mt);

static inline int calcBIstride(unsigned int width,int bitcount)
{
 return ((width*bitcount+31)&~31)>>3;
}

struct Textradata
{
private:
 bool own;
 void done(void);
 typedef std::pair<const unsigned char*,size_t> TdataSize;
 TdataSize parse(const WAVEFORMATEX *wfex);
 TdataSize parse(const MPEG1VIDEOINFO *mpeg1info);
 TdataSize parse(const MPEG2VIDEOINFO *mpeg2info);
 void set(const TdataSize &dataSize,unsigned int padding=0,bool Iown=false);
public:
 Textradata(void):data(NULL),size(0) {}
 Textradata(const CMediaType &mt,int padding=0);
 Textradata(const WAVEFORMATEX &wfex,int padding=0);
 Textradata(const MPEG1VIDEOINFO &mpeg1info,int padding=0);
 Textradata(const MPEG2VIDEOINFO &mpeg2info,int padding=0);
 ~Textradata(void);
 void clear(void);
 void set(const void *Idata,size_t Isize,unsigned int padding=0,bool Iown=false);
 const uint8_t *data;
 size_t size;
};

class TsearchInterfaceInGraph
{
// IFilterGraph::EnumFilters can freeze if it is called from streaming thread.
// This class is work around for it.
// Create a thread to execute IFilterGraph::EnumFilters and if it seems to be freezing, skip it.
// So return value is not always correct. Check waitSucceeded().
// In case of freezes, the thread is likely to return until next sample is delivered.
private:
 IFilterGraph *graph;
 IID iid;
 IUnknown *dest;
 HANDLE hThread;
 DWORD waitResult;
 static unsigned int __stdcall searchFilterInterfaceThreadEntry(void *self);
 bool (*searchInterfaceFunc)(IFilterGraph *graph,const IID &iid,IUnknown **dest);
public:
 TsearchInterfaceInGraph(IFilterGraph *Igraph,const IID &Iiid,bool (*IsearchInterfaceFunc)(IFilterGraph *graph,const IID &iid,IUnknown **dest)):
   graph(Igraph),
   iid(Iiid),
   hThread(NULL),
   searchInterfaceFunc(IsearchInterfaceFunc)
  {
  }
 ~TsearchInterfaceInGraph();
 bool getResult(IUnknown* *Idest);
 bool waitSucceeded(void)
  {
   return waitResult == WAIT_OBJECT_0;
  }
};

#endif
