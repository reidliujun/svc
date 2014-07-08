/*
{* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
CPU Usage Measurement routines for Delphi and C++ Builder

Author:       Alexey A. Dynnikov
EMail:        aldyn@chat.ru
WebSite:      http://www.aldyn.ru/
Support:      Use the e-mail aldyn@chat.ru
                          or support@aldyn.ru

Creation:     Jul 8, 2000
Version:      1.02

Legal issues: Copyright (C) 2000 by Alexey A. Dynnikov <aldyn@chat.ru>

              This software is provided 'as-is', without any express or
              implied warranty.  In no event will the author be held liable
              for any  damages arising from the use of this software.

              Permission is granted to anyone to use this software for any
              purpose, including commercial applications, and to alter it
              and redistribute it freely, subject to the following
              restrictions:

              1. The origin of this software must not be misrepresented,
                 you must not claim that you wrote the original software.
                 If you use this software in a product, an acknowledgment
                 in the product documentation would be appreciated but is
                 not required.

              2. Altered source versions must be plainly marked as such, and
                 must not be misrepresented as being the original software.

              3. This notice may not be removed or altered from any source
                 distribution.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
USAGE:

1. Include this unit into project.

2. Call GetCPUCount to obtain the numbr of processors in the system
   If you have 2 cpus, GetCPUCount return 3. (0,1 for each core, 2 for Total)
3. Each time you need to know the value of CPU usage call the CollectCPUData
   to refresh the CPU usage information. Then call the GetCPUUsage to obtain
   the CPU usage for given processor. Note that succesive calls of GetCPUUsage
   without calling CollectCPUData will return the same CPU usage value.

Example:

procedure TTestForm.TimerTimer(Sender: TObject);
var i: Integer;
begin
    CollectCPUData; // Get the data for all processors

    for i:=0 to GetCPUCount-1 do // Show data for each processor
        MInfo.Lines[i]:=Format('CPU #%d - %5.2f%%',[i,GetCPUUsage(i)*100]);
end;
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *}
*/
#include "stdafx.h"
#include "TcpuUsage.h"
#include "Tconfig.h"

//------------------------------------------------------------------------------
int TcpuUsage::GetCPUCount(void)
{
 if (Tconfig::winNT)
  {
   if (_ProcessorsCount < 0)
    CollectCPUData();
   return _ProcessorsCount;
  }
 else
  return 1;
}

//------------------------------------------------------------------------------
void TcpuUsage::ReleaseCPUData(void)
{
 HKEY H;
 DWORD    R;
 DWORD    dwDataSize, dwType;
 if (Tconfig::winNT)
  return;
 if (!_W9xCollecting)
  return;
 _W9xCollecting=false;
 if (_W9xCpuKey) RegCloseKey(_W9xCpuKey);

 R=RegOpenKeyEx(HKEY_DYN_DATA,_l("PerfStats\\StopStat"), 0, KEY_ALL_ACCESS, &H );

 if (R!=ERROR_SUCCESS)
  return;

 dwDataSize=sizeof(DWORD);
 RegQueryValueEx( H, _l("KERNEL\\CPUUsage"), NULL, &dwType, (unsigned char*)&_W9xCpuUsage, &dwDataSize);

 RegCloseKey(H);

}

//------------------------------------------------------------------------------
int TcpuUsage::GetCPUUsage(int Index)
{
 int result;
 if (Tconfig::winNT)
  {
   if (_ProcessorsCount < 0)
    CollectCPUData();
   if (Index >= _ProcessorsCount || Index < 0)
    return 0;//raise Exception.Create('CPU index out of bounds');
   if (_PrevSysTime == _SysTime)
    result=0;
   else
    result=int(100-(100*(_Counters[Index] - _PrevCounters[Index]))/(_SysTime-_PrevSysTime));
  }
 else
  {
   if (Index!=0)
    return 0;//raise Exception.Create('CPU index out of bounds');
   if (!_W9xCollecting)
    CollectCPUData();
   result=_W9xCpuUsage ;
  }
 return result;
}

//------------------------------------------------------------------------------
int TcpuUsage::GetCPUUsageForPP(void)
{
 if (Tconfig::winNT && _ProcessorsCount>2)
  {
   int result=0;
   for(int i=0;i<_ProcessorsCount-1;i++)
    {
     int x=GetCPUUsage(i);
     if(x>result) result=x;
    }
   return result;
  }
 else
  return GetCPUUsage(0);
}

//------------------------------------------------------------------------------
void TcpuUsage::CollectCPUData(void)
{
 DWORD BS;
 PPERF_COUNTER_BLOCK    _PCB_Instance;
 PPERF_INSTANCE_DEFINITION    _PID_Instance;
 FILETIME ST;

 HKEY H;
 DWORD R;
 DWORD dwDataSize, dwType;

 if (Tconfig::winNT)
  {
   BS=_BufferSize;
   //const
   static const char_t *Processor_IDX_Str=_l("238");
   static const int Processor_IDX=238;
   static const int CPUUsageIDX=6;
   LONG ret;
   while ((ret=RegQueryValueEx( HKEY_PERFORMANCE_DATA, Processor_IDX_Str,NULL,NULL,
                           (unsigned char*)_PerfData, &BS )) == ERROR_MORE_DATA)
    {
     // Get a buffer that is big enough.
     _BufferSize+=0x1000;
     BS=_BufferSize;
     _PerfData=(PERF_DATA_BLOCK*)realloc( _PerfData, _BufferSize );
    }
   if (ret!=ERROR_SUCCESS)
    {
     _ProcessorsCount=0;
     return;
    }

   // Locate the performance object
   _POT = PPERF_OBJECT_TYPE(intptr_t(_PerfData) + _PerfData->HeaderLength);
   for (DWORD i1=0;i1<_PerfData->NumObjectTypes;i1++)
    {
     if (_POT->ObjectNameTitleIndex == Processor_IDX)
      break;
     _POT = PPERF_OBJECT_TYPE(intptr_t(_POT) + _POT->TotalByteLength);
    }

   // Check for success
   if (_POT->ObjectNameTitleIndex != Processor_IDX)
    return ;//raise Exception.Create('Unable to locate the "Processor" performance object');

   if (_ProcessorsCount < 0)
    {
     _ProcessorsCount=_POT->NumInstances;
     _Counters=(PAInt64F)calloc(_ProcessorsCount,sizeof(TInt64));
     _PrevCounters=(PAInt64F)calloc(_ProcessorsCount,sizeof(TInt64));
     _SysTime=0;
    }

   // Locate the "% CPU usage" counter definition
   _PCD = PPERF_COUNTER_DEFINITION(intptr_t(_POT) + _POT->HeaderLength);
   for (DWORD i2 = 0;i2< _POT->NumCounters;i2++)
    {
     if (_PCD->CounterNameTitleIndex==CPUUsageIDX)
      break;
     _PCD = PPERF_COUNTER_DEFINITION(intptr_t(_PCD) + _PCD->ByteLength);
    }

   // Check for success
   if (_PCD->CounterNameTitleIndex != CPUUsageIDX)
    return ;//raise Exception.Create('Unable to locate the "% of CPU usage" performance counter');

   // Collecting coutners
   _PID_Instance = PPERF_INSTANCE_DEFINITION(intptr_t(_POT) + _POT->DefinitionLength);
   for (int i=0;i<_ProcessorsCount;i++)
    {
     _PCB_Instance = PPERF_COUNTER_BLOCK(intptr_t(_PID_Instance) + _PID_Instance->ByteLength );

     _PrevCounters[i]=_Counters[i];
     _Counters[i]=FInt64(*PInt64(intptr_t(_PCB_Instance) + _PCD->CounterOffset));

     _PID_Instance = PPERF_INSTANCE_DEFINITION(intptr_t(_PCB_Instance) + _PCB_Instance->ByteLength);
    }

   _PrevSysTime=_SysTime;
   SystemTimeToFileTime(&_PerfData->SystemTime, &ST);
   memcpy(&_SysTime,&ST,sizeof(ST));//_SysTime=FInt64(TInt64(ST));
  }
 else
  {
   if (!_W9xCollecting)
    {
     R=RegOpenKeyEx( HKEY_DYN_DATA, _l("PerfStats\\StartStat"), 0, KEY_ALL_ACCESS, &H );
     if (R != ERROR_SUCCESS)
      return;//raise Exception.Create('Unable to start performance monitoring');

     dwDataSize=sizeof(DWORD);

     RegQueryValueEx( H, _l("KERNEL\\CPUUsage"),NULL, &dwType, (unsigned char *)&_W9xCpuUsage, &dwDataSize );

     RegCloseKey(H);

     R=RegOpenKeyEx( HKEY_DYN_DATA, _l("PerfStats\\StatData"), 0,KEY_READ, &_W9xCpuKey );

     if (R != ERROR_SUCCESS)
      return;//raise Exception.Create('Unable to read performance data');

     _W9xCollecting=true;
    }

   dwDataSize=sizeof(DWORD);
   RegQueryValueEx( _W9xCpuKey, _l("KERNEL\\CPUUsage"),NULL,&dwType, (unsigned char*)&_W9xCpuUsage, &dwDataSize );
  }
}


void TcpuUsage::usage_initialization(void)
{
 _ProcessorsCount= -1;
 _BufferSize= 0x2000;
 _PerfData = (PERF_DATA_BLOCK*)malloc(_BufferSize);
}
void TcpuUsage::usage_finalization(void)
{
 ReleaseCPUData();
 if (_PerfData) free(_PerfData);
 if (_Counters) free(_Counters);
 if (_PrevCounters) free(_PrevCounters);
}
