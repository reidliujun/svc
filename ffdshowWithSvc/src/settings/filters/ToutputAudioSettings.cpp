/*
 * Copyright (c) 2005,2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "stdafx.h"
#include "ToutputAudioSettings.h"
#include "TsampleFormat.h"
#include "TaudioFilterOutput.h"

#ifdef VISTA_SPDIF
#include <InitGuid.h>
#include <IffMmdevice.h> // Vista header import (MMDeviceAPI.h)
#endif

const char_t* ToutputAudioSettings::connetTos[]=
{
 _l("any filter"),
 _l("DirectSound"),
 _l("WaveOut"),
 NULL
};

TdevicesList ToutputAudioSettings::devicesList;

const TfilterIDFF ToutputAudioSettings::idffs=
{
 /*name*/      _l("Output"),
 /*id*/        IDFF_filterOutputAudio,
 /*is*/        0,
 /*order*/     0,
 /*show*/      0,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     0,
};

ToutputAudioSettings::ToutputAudioSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs,false)
{
 static const TintOptionT<ToutputAudioSettings> iopts[]=
  {
   IDFF_aoutpassthroughAC3     ,&ToutputAudioSettings::passthroughAC3        ,0,1,_l(""),1,
     _l("passthroughAC3"),0,
   IDFF_aoutpassthroughDTS     ,&ToutputAudioSettings::passthroughDTS        ,0,1,_l(""),1,
     _l("passthroughDTS"),0,
   IDFF_outsfs                 ,&ToutputAudioSettings::outsfs                ,1,1,_l(""),1,
     _l("outsfs"),TsampleFormat::SF_PCM16,
   IDFF_outAC3bitrate          ,&ToutputAudioSettings::outAC3bitrate         ,32,640,_l(""),1,
     _l("outAC3bitrate"),640,
   IDFF_aoutConnectTo          ,&ToutputAudioSettings::connectTo             ,0,2,_l(""),1,
     _l("connectTo"),0,
   IDFF_aoutConnectToOnlySpdif ,&ToutputAudioSettings::connectToOnlySpdif    ,0,0,_l(""),1,
     _l("connectToOnlySpdif"),1,
   IDFF_aoutAC3EncodeMode      ,&ToutputAudioSettings::outAC3EncodeMode      ,0,0,_l(""),1,
     _l("outAC3EncodeMode"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_aoutMultichannelDevice   ,(TstrVal)&ToutputAudioSettings::multichannelDevice    ,255,0, _l(""),1,
     _l("multichannelDevice"),_l(""),
   IDFF_aoutMultichannelDeviceId ,(TstrVal)&ToutputAudioSettings::multichannelDeviceId  ,255,0, _l(""),1,
     _l("multichannelDeviceId"),_l(""),
   0
  };
 addOptions(sopts);
 static const TcreateParamList1 listAoutConnectTo(connetTos);setParamList(IDFF_aoutConnectTo,&listAoutConnectTo);
 devicesList = getDevices();
}

void ToutputAudioSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 queueFilter<TaudioFilterOutput>(filtersorder,filters,queue);
}

const int* ToutputAudioSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_aoutpassthroughAC3, IDFF_aoutpassthroughDTS, IDFF_outsfs,IDFF_outAC3bitrate,IDFF_aoutConnectTo,IDFF_aoutConnectToOnlySpdif,0};
 return idResets;
}

TdevicesList ToutputAudioSettings::getDevices(void)
{
  TdevicesList devicesList;
  devicesList[L"Use default device"] = L"";

#ifdef VISTA_SPDIF
  IMMDeviceEnumerator *deviceEnumerator = NULL;
  // Enumerate audio devices
  HRESULT hr = CoCreateInstance(
	  __uuidof(MMDeviceEnumerator), 
	  NULL, 
	  CLSCTX_INPROC_SERVER,
	  __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
  if (FAILED(hr))
  {
	  return devicesList;
  }

  // Getting default render device
  IMMDevice* defaultRenderer=0;
  hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &defaultRenderer);
  if (FAILED(hr))
  {
	  return devicesList;
  }
  WCHAR *default_deviceIdp = NULL;
  WCHAR default_deviceId[255] = L"";
  hr = defaultRenderer->GetId(&default_deviceIdp);
  if (FAILED(hr))
  {
	  return devicesList;
  }
  ff_strncpy(default_deviceId, default_deviceIdp, 254);
  CoTaskMemFree(default_deviceIdp);
  defaultRenderer->Release();

  // Enumerate audio end points
  IMMDeviceCollection* deviceEndPoints=0;
  hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceEndPoints);
  if (FAILED(hr))
  {
	  return devicesList;
  }
  deviceEnumerator->Release();
  deviceEnumerator = NULL;

  unsigned int deviceCount;
  deviceEndPoints->GetCount(&deviceCount);

  for (unsigned int i=0; i<deviceCount; i++)
  {
	  IMMDevice *device;
	  deviceEndPoints->Item(i, &device);
	  DWORD state=0;
	  device->GetState(&state);
	  if (state != DEVICE_STATE_ACTIVE)
	  {
		  device->Release();
		  continue;
	  }
	  WCHAR *deviceIdp = NULL;
	  WCHAR deviceId[255] = L"";
	  hr = device->GetId(&deviceIdp);
	  ff_strncpy(deviceId, deviceIdp, 254);

	  // Retrieve properties
	  IPropertyStore* deviceProperties;
	  device->OpenPropertyStore(STGM_READ, &deviceProperties);
	  PROPVARIANT value;
	  PropVariantInit(&value);
	  hr = deviceProperties->GetValue(PKEY_Device_FriendlyName, &value);
	  WCHAR deviceName[255] = L"";
	  if (!strcmp(deviceId, default_deviceId))
	  {
		  if (value.pwszVal != NULL)
		  {
			ff_strncpy(deviceName, value.pwszVal, 240);
			strncat_s(deviceName, countof(deviceName), L" (default)", _TRUNCATE);
		  }
		  else
			wsprintfW(deviceName, L"Device %d (default)", i);
	  }
	  else
	  {
		  if (value.pwszVal != NULL)
			ff_strncpy(deviceName, value.pwszVal, 254);
		  else
			wsprintfW(deviceName, L"Device %d", i);
	  }
	// Get the DirectSound or DirectSoundCapture device GUID
    // (in WCHAR string format) for the endpoint device.
	PROPVARIANT var;
	PropVariantInit(&var);

	// Redefinition of property key PKEY_AudioEndpoint_GUID (won't compile if it is used directly)
	const PROPERTYKEY key = {0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 4};
	hr = deviceProperties->GetValue(key, &var);

	devicesList[deviceName] = deviceId;//var.pwszVal;

	PropVariantClear(&value);
	PropVariantClear(&var);
	CoTaskMemFree(deviceIdp);
	device->Release();
  }
#endif
  return devicesList;
}
