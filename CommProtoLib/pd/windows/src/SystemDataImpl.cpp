#include <SystemDataImpl.h>
#include <windows.h>
#include <TCHAR.h>
#include <pdh.h>
#include <psapi.h>


namespace commproto
{
	namespace diagnostics
	{

		static PDH_HQUERY cpuQuery;
		static PDH_HCOUNTER cpuTotal;

		static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
		static int numProcessors;
		static HANDLE self;

		SystemDataImpl::SystemDataImpl()
		{
			PdhOpenQuery(NULL, NULL, &cpuQuery);
			// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
			PdhAddEnglishCounterW(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
			PdhCollectQueryData(cpuQuery);

			SYSTEM_INFO sysInfo;
			FILETIME ftime, fsys, fuser;

			GetSystemInfo(&sysInfo);
			numProcessors = sysInfo.dwNumberOfProcessors;

			GetSystemTimeAsFileTime(&ftime);
			memcpy(&lastCPU, &ftime, sizeof(FILETIME));

			self = GetCurrentProcess();
			GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
			memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
			memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
		}

		uint64_t SystemDataImpl::getTotalMemBytes()
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);
			uint64_t total = memInfo.ullTotalPhys;
			return total;
		}

		uint64_t SystemDataImpl::getUsedMemBytes()
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);
			uint64_t used = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
			return used;
		}

		uint64_t SystemDataImpl::getProcessMemBytes()
		{
			PROCESS_MEMORY_COUNTERS_EX pmc;
			GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
			SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
			uint64_t usedByMe = pmc.WorkingSetSize;
			return usedByMe;
		}

		float SystemDataImpl::getProcessorUsage()
		{
			PDH_FMT_COUNTERVALUE counterVal;

			PdhCollectQueryData(cpuQuery);
			PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
			return static_cast<float>(counterVal.doubleValue);
		}

		float SystemDataImpl::getProcessProcessorUsage()
		{
			FILETIME ftime, fsys, fuser;
			ULARGE_INTEGER now, sys, user;

			GetSystemTimeAsFileTime(&ftime);
			memcpy(&now, &ftime, sizeof(FILETIME));

			GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
			memcpy(&sys, &fsys, sizeof(FILETIME));
			memcpy(&user, &fuser, sizeof(FILETIME));
			double percent = (sys.QuadPart - lastSysCPU.QuadPart) +
				(user.QuadPart - lastUserCPU.QuadPart);
			percent /= (now.QuadPart - lastCPU.QuadPart);
			percent /= numProcessors;
			lastCPU = now;
			lastUserCPU = user;
			lastSysCPU = sys;

			return static_cast<float>(percent * 100);
		}
	}
}