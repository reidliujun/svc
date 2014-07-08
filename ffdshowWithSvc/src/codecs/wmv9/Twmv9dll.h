#ifndef _TWMV9DLL_H_
#define _TWMV9DLL_H_

class Twmv9dll
{
private:
 HMODULE hdll;
 void loadDll(const char *dllName);
public:
 bool ok;
 Twmv9dll(void)
  {
   hdll=LoadLibrary(_l("wmvcore.dll"));
   ok=(hdll!=NULL);
   loadFunction((void**)&WMCreateWriterFileSink,"WMCreateWriterFileSink");
   loadFunction((void**)&WMCreateWriter,"WMCreateWriter");
   loadFunction((void**)&WMCreateProfileManager,"WMCreateProfileManager");
  }
 ~Twmv9dll()
  {
   if (hdll)
    FreeLibrary(hdll);
  }
 void loadFunction(void **fnc,const char *name)
  {
   *fnc=(void*)GetProcAddress(hdll,name);
   ok&=(*fnc!=NULL);
  }
 HRESULT (STDMETHODCALLTYPE *WMCreateWriterFileSink)( IWMWriterFileSink **ppSink );
 HRESULT (STDMETHODCALLTYPE *WMCreateWriter)( IUnknown* pUnkCert, IWMWriter **ppWriter );
 HRESULT (STDMETHODCALLTYPE *WMCreateProfileManager)( IWMProfileManager **ppProfileManager );
};

#endif
