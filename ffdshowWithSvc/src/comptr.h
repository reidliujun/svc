#ifndef _COMPTR_H_
#define _COMPTR_H_

template<class T> struct comptr
{
protected:
 T* p;
 IUnknown* ptrAssign(IUnknown **pp,IUnknown *lp)
  {
   if (lp!=NULL)
    lp->AddRef();
   if (*pp)
    (*pp)->Release();
   *pp=lp;
   return lp;
  }
public:
 comptr(void):p(NULL) {}
 comptr(T* lp)
  {
   if ((p=lp)!=NULL)
    p->AddRef();
  }
 comptr(const comptr<T> &lp)
  {
   if ((p=lp.p)!=NULL)
    p->AddRef();
  }
 ~comptr()
  {
   if (p) p->Release();
  }
 void Release()
  {
   if (p) p->Release();
   p=NULL;
  }
 operator T*() {return (T*)p;}
 T& operator*() {ASSERT(p!=NULL);return *p;}
 T** operator&() {ASSERT(p==NULL);return &p;}
 T* operator->() {ASSERT(p!=NULL);return p;}
 T* operator=(T* lp)
  {
   return (T*)ptrAssign((IUnknown**)&p,lp);
  }
 T* operator=(const comptr<T> &lp)
  {
   return (T*)ptrAssign((IUnknown**)&p,lp.p);
  }
 bool operator!() {return p==NULL;}
};

template<class T> static inline const GUID& getGUID(void);

#define DEFINE_TGUID(IID,I, l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
 DEFINE_GUID(IID##_##I,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8);     \
 template<> inline const GUID& getGUID<I>(void) {return IID##_##I;}

template<class T> class comptrQ :public comptr<T>
{
public:
 comptrQ(void) {}
 template<class Tsrc> comptrQ(Tsrc *src)
  {
   this->p=NULL;
   if (src->QueryInterface(getGUID<T>(),(void**)&this->p)!=S_OK)
    this->p=NULL;
  }
 template<class Tsrc> comptrQ(comptr<Tsrc> &src)
  {
   this->p=NULL;
   if (src->QueryInterface(getGUID<T>(),(void**)&this->p)!=S_OK)
    this->p=NULL;
  }
 T* operator=(T* lp)
  {
   return (T*)ptrAssign((IUnknown**)&this->p,lp);
  }
};

template<> inline const GUID& getGUID<IMediaPosition>(void) {return IID_IMediaPosition;}
template<> inline const GUID& getGUID<IMediaControl>(void) {return IID_IMediaControl;}
template<> inline const GUID& getGUID<IMediaSeeking>(void) {return IID_IMediaSeeking;}
template<> inline const GUID& getGUID<IMediaSample2>(void) {return IID_IMediaSample2;}
template<> inline const GUID& getGUID<ISpecifyPropertyPages>(void) {return IID_ISpecifyPropertyPages;}
template<> inline const GUID& getGUID<IMixerPinConfig>(void) {return IID_IMixerPinConfig;}
template<> inline const GUID& getGUID<IMediaEventSink>(void) {return IID_IMediaEventSink;}

#endif
