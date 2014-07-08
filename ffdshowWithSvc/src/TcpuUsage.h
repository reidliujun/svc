#ifndef _TCPUUSAGE_H_
#define _TCPUUSAGE_H_

class TcpuUsage
{
public:
 TcpuUsage(void):_W9xCpuKey(NULL),_Counters(NULL),_PrevCounters(NULL),_PerfData(NULL)
  {
   usage_initialization();
  }
 ~TcpuUsage()
  {
   usage_finalization();
  }
 void CollectCPUData(void);  // Call CollectCPUData to refresh information about CPU usage
 int GetCPUCount(void);      // Call it to obtain the number of CPU's in the system
 int GetCPUUsage(int Index); // Call it to obtain the % of usage for given CPU
 int GetCPUUsageForPP(void);
 void ReleaseCPUData(void);  // For Win9x only: call it to stop CPU usage monitoring and free system resources
private:
 void usage_initialization(void),usage_finalization(void);
 typedef int64_t TInt64;
 typedef TInt64 *PInt64;

 typedef PERF_DATA_BLOCK TPERF_DATA_BLOCK;
 typedef PERF_OBJECT_TYPE TPERF_OBJECT_TYPE;
 typedef PERF_COUNTER_DEFINITION TPERF_COUNTER_DEFINITION;
 typedef PERF_COUNTER_BLOCK TPERF_COUNTER_BLOCK;
 typedef PERF_INSTANCE_DEFINITION TPERF_INSTANCE_DEFINITION;

 typedef TInt64 TInt64F;// = TInt64;
 typedef TInt64F FInt64;
 typedef TInt64 Int64D;
 //type
 typedef TInt64F AInt64F[0xffff+1];//    AInt64F = array[0..$FFFF] of TInt64F;
 typedef TInt64F *PAInt64F;//PAInt64F = ^AInt64F;
 //var
 PPERF_DATA_BLOCK    _PerfData;
 int    _BufferSize;
 PPERF_OBJECT_TYPE   _POT;
 PPERF_COUNTER_DEFINITION _PCD;
 int    _ProcessorsCount;
 PAInt64F    _Counters;
 PAInt64F    _PrevCounters;
 TInt64F    _SysTime;
 TInt64F    _PrevSysTime;

 bool       _W9xCollecting;
 DWORD    _W9xCpuUsage;
 HKEY    _W9xCpuKey;
};

#endif
