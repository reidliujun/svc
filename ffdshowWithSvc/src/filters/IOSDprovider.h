#ifndef _IOSDPROVIDER_H_
#define _IOSDPROVIDER_H_

DECLARE_INTERFACE(IOSDprovider)
{
 STDMETHOD_(const char_t*,getInfoItemName)(int type) PURE;
 STDMETHOD (getInfoItemValue)(int id,const char_t* *value,int *wasChange,int *splitline) PURE; 
 STDMETHOD_(const char_t*,getStartupFormat)(int *duration) PURE;
 STDMETHOD_(const char_t*,getFormat)(void) PURE;
 STDMETHOD_(bool,isOSD)(void) PURE;
 STDMETHOD (getSave)(int *isSave,const char_t* *flnm) PURE;
 STDMETHOD_(int,getInfoShortcutItem)(const char_t *s,int *toklen) PURE;
};

#endif
