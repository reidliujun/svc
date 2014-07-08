/*
 * Copyright (c) 2006 h.yamagata
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

/*
 * ffSpkCfg.dll is called from setup script.
 * getSpeakerConfig returns the speaker configuration(number of speakers) in Control Panel.
 * This requires directX 8.0 or later.
 * Link runtime library staticly because the dll is used in setup program.
 *
 * The dll depends on dsound.dll. If dsound.dll is not found, loading this dll fails.
 * In that case, setup program has to handle the exception.
 *
 * Sample setup script for inno setup
 *
 * function ffSpkCfg(): Integer;
 * external 'getSpeakerConfig@files:ffSpkCfg.dll stdcall delayload';
 *
 * function getSpeakerConfig(): Integer;
 * begin
 *   try
 *     Result := ffSpkCfg();
 *   except
 *     Result := 4; // DSSPEAKER_STEREO
 *   end;
 * end;
 *
 */

#include "dsound.h"
extern "C" int __stdcall getSpeakerConfig(void)
{
 DWORD dwSpeakerConfig;
 IDirectSound8 *pDS8;
 HRESULT hr = DirectSoundCreate8(NULL, &pDS8, NULL);
 if(hr!=DS_OK || !pDS8)
  return DSSPEAKER_STEREO;

 hr = pDS8->GetSpeakerConfig(&dwSpeakerConfig);
 pDS8->Release();pDS8=NULL;
 if(hr!=DS_OK)
  return DSSPEAKER_STEREO;
 return DSSPEAKER_CONFIG(dwSpeakerConfig);
}
