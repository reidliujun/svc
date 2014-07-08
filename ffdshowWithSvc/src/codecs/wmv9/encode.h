#ifndef _ENCODE_H_
#define _ENCODE_H_

class CHandlingMediaBuffer : public IMediaBuffer
#ifdef SUPPORT_INTERLACE
   , public INSSBuffer3
#endif //SUPPORT_INTERLACE

 {
  BYTE *m_pData;
  ULONG m_ulSize;
  ULONG m_ulData;
  ULONG m_cRef;
  BYTE bInterlaceProperty;
 public:
  CHandlingMediaBuffer(): bInterlaceProperty( 0 )
  {
   m_cRef = 1;
  }

  void set_Buffer(BYTE *pData, DWORD ulData, DWORD ulSize)
  {
   m_pData = pData;
   m_ulSize = ulSize;
   m_ulData = ulData;
  }

  //
  // IUnknown
  //
  STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
  {
   if (riid == IID_IUnknown)
    {
     AddRef();
     *ppv = /*reinterpret_cast<IUnknown*>*(IUnknown*)( this );
                  return ( NOERROR );
              } else if (riid == IID_IMediaBuffer) {
                  AddRef();
                  *ppv = reinterpret_cast<IMediaBuffer*>*/(IMediaBuffer*)( this );
     return ( NOERROR );
#ifdef SUPPORT_INTERLACE

    }
   else if ( riid == IID_INSSBuffer3 )
    {
     AddRef();
     *ppv = /*reinterpret_cast<INSSBuffer3*>*/(INSSBuffer3*)( this );
     return ( NOERROR );
#endif //SUPPORT_INTERLACE

    }
   else
    {
     return ( E_NOINTERFACE );
    }
  }

  STDMETHODIMP_(ULONG) AddRef()
  {
   return InterlockedIncrement((long*)&m_cRef);
  }
  STDMETHODIMP_(ULONG) Release()
  {
   long l = InterlockedDecrement((long*)&m_cRef);
   if (l == 0)
    delete this;
   return l;
  }

  //
  // IMediaBuffer
  //
  STDMETHODIMP GetLength(DWORD *pulLength )
  {
   if( NULL == pulLength )
    {
     return ( E_POINTER );
    }
   *pulLength = m_ulData;
   return ( NOERROR );
  }

  STDMETHODIMP SetLength(DWORD ulLength )
  {
   m_ulData = ulLength;
   return ( NOERROR );
  }

  STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength)
  {
   *pcbMaxLength = m_ulSize;
   return ( NOERROR );
  }
  STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength)
  {
   if( NULL != ppBuffer )
    {
     *ppBuffer = m_pData;
    }
   if( NULL != pcbLength )
    {
     *pcbLength = m_ulData;
    }
   return ( NOERROR );
  }

#ifdef SUPPORT_INTERLACE
  //
  // INSSBufferx members
  //
  HRESULT STDMETHODCALLTYPE GetBuffer( BYTE**  ppdwBuffer )
  {
   if( NULL == ppdwBuffer )
    {
     return ( E_POINTER );
    }
   *ppdwBuffer = m_pData;
   return ( S_OK );
  }

  HRESULT STDMETHODCALLTYPE GetSampleProperties( DWORD cbProperties,
    BYTE *pbProperties )
  {
   return ( E_NOTIMPL );
  }

  HRESULT STDMETHODCALLTYPE SetSampleProperties( DWORD cbProperties,
    BYTE *pbProperties )
  {
   return ( E_NOTIMPL );
  }

  HRESULT STDMETHODCALLTYPE GetProperty(  GUID   guidBufferProperty,
                                          void*  pvBufferProperty,
                                          DWORD* pdwBufferPropertySize)
  {
   if( NULL == pdwBufferPropertySize )
    {
     return ( E_POINTER );
    }

   if( WM_SampleExtension_ContentType == guidBufferProperty )
    {
     if( NULL == pvBufferProperty )
      {
       *pdwBufferPropertySize = WM_SampleExtension_ContentType_Size;
       return ( S_OK );
      }
     else
      {
       if( *pdwBufferPropertySize < WM_SampleExtension_ContentType_Size )
        {
         return ( E_INVALIDARG );
        }
       else
        {
         *(BYTE*)pvBufferProperty = bInterlaceProperty;
         return ( S_OK );
        }
      }
    }
   return ( NS_E_UNSUPPORTED_PROPERTY );
  }

  HRESULT STDMETHODCALLTYPE SetProperty(  GUID   guidBufferProperty,
                                          void*  pvBufferProperty,
                                          DWORD  dwBufferPropertySize)
  {
   if( NULL == pvBufferProperty )
    {
     return ( E_POINTER );
    }

   if( WM_SampleExtension_ContentType == guidBufferProperty )
    {
     if( dwBufferPropertySize != WM_SampleExtension_ContentType_Size )
      {
       return ( E_INVALIDARG );
      }
     else
      {
       bInterlaceProperty = *(BYTE*)pvBufferProperty;
       return ( S_OK );
      }
    }
   return ( NS_E_UNSUPPORTED_PROPERTY );
  }
#endif //SUPPORT_INTERLACE
 };

#endif
