// cpuid.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#if _MSC_VER >=1400    // VC2005��֧��intrin.h
#include <intrin.h>    // ����Intrinsics����
#endif
#include "cpu.h"
#include <iostream>
using namespace std;

//��32λģʽ�£����ǿ���ʹ����Ƕ���������cpuidָ�����64λģʽ�£�VC��������֧����Ƕ��ࡣ
//����������˵��
//
//VC��������32/64λ��֧���ԡ���
//  32λ��VC6������֧�ֱ���32λIntrinsics�����ġ�
//  64λ��VC2005������֧�ֱ���64λIntrinsics�����ġ�
//
//���ķ�����32λ�������µļ����ԡ���
//	__cpuid������VC6������ߣ���
//	__cpuidex������VC6������ߣ���
//
//���ķ�����64λ�������µļ����ԡ���
//	  __cpuid������VC2005������ߣ���
//	  __cpuidex������VC2010������ߣ���

#if defined(_WIN64)
// 64λ�²�֧���������. Ӧʹ��__cpuid��__cpuidex��Intrinsics������
#else
#if _MSC_VER < 1600    // VS2010. ��˵VC2008 SP1֮���֧��__cpuidex
void __cpuidex(INT32 CPUInfo[4], INT32 InfoType, INT32 ECXValue)
{
	if (NULL==CPUInfo)    return;
	_asm{
		// load. ��ȡ�������Ĵ���
		mov edi, CPUInfo;    // ׼����ediѰַCPUInfo
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID
		cpuid;
		// save. ���Ĵ������浽CPUInfo
		mov    [edi], eax;
		mov    [edi+4], ebx;
		mov    [edi+8], ecx;
		mov    [edi+12], edx;
	}
}
#endif    // #if _MSC_VER < 1600    // VS2010. ��˵VC2008 SP1֮���֧��__cpuidex

#if _MSC_VER < 1400    // VC2005��֧��__cpuid
void __cpuid(INT32 CPUInfo[4], INT32 InfoType)
{
	__cpuidex(CPUInfo, InfoType, 0);
}
#endif    // #if _MSC_VER < 1400    // VC2005��֧��__cpuid

#endif    // #if defined(_WIN64)

const char* szFeatures[] =
{
	"x87 FPU On Chip",
	"Virtual-8086 Mode Enhancement",
	"Debugging Extensions",
	"Page Size Extensions",
	"Time Stamp Counter",
	"RDMSR and WRMSR Support",
	"Physical Address Extensions",
	"Machine Check Exception",
	"CMPXCHG8B Instruction",
	"APIC On Chip",
	"Unknown1",
	"SYSENTER and SYSEXIT",
	"Memory Type Range Registers",
	"PTE Global Bit",
	"Machine Check Architecture",
	"Conditional Move/Compare Instruction",
	"Page Attribute Table",
	"Page Size Extension",
	"Processor Serial Number",
	"CFLUSH Extension",
	"Unknown2",
	"Debug Store",
	"Thermal Monitor and Clock Ctrl",
	"MMX Technology",
	"FXSAVE/FXRSTOR",
	"SSE Extensions",
	"SSE2 Extensions",
	"Self Snoop",
	"Hyper-threading Technology",
	"Thermal Monitor",
	"Unknown4",
	"Pend. Brk. EN."
};

void ToHex(const unsigned char * szOrigin, int nSize, char * szHex)
{
	char szTemp[10];
	for(int nIndex = 0; nIndex < nSize; nIndex ++)
	{
		sprintf(szTemp, "%02X", szOrigin[nIndex]);
		if(nIndex == 0)
		{
			strcpy(szHex, szTemp);
		}
		else
		{
			strcat(szHex, szTemp);
		}
	}
}

bool DetectCPU()
{
	char szCPUDesc[13];
	memset(szCPUDesc, 0, 13);

	unsigned char szCPUSN[12];
	memset(szCPUSN, 0, 12);

	unsigned long ulEAX = 0U, ulEBX = 0U, ulECX = 0U, ulEDX = 0U;

	__try
	{
		_asm
		{
			mov eax, 1
				cpuid
				mov ulEDX, edx
				mov ulEAX, eax
		}

		//����Ƿ���CPU���к�
		//ע�⣬Intel�ĵ���˵���edx�ĵ�18λ�Ǵӵ�0λ��ʼ�����
		if(!(ulEDX & (1 << 18)))
			return false;
		//��ȡ���кŵĺ�����WORD
		memcpy(&szCPUSN[8], &ulEAX, 4);

		_asm
		{
			mov eax, 3
				cpuid
				mov ulECX, ecx
				mov ulEDX, edx
		}
		//��ȡ���кŵ�ǰ4��WORD
		memcpy(&szCPUSN[0], &ulECX, 4);
		memcpy(&szCPUSN[4], &ulEDX, 4);

		//��ȡCPU OEM��Ϣ
		_asm
		{
			mov eax, 0
				cpuid
				mov dword ptr szCPUDesc[0], ebx
				mov dword ptr szCPUDesc[4], edx
				mov dword ptr szCPUDesc[8], ecx
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	char szCPUSNHex[25];
	ToHex(szCPUSN, 12, szCPUSNHex);

	char szCPUID[37];
	sprintf(szCPUID, "%s%s", szCPUDesc, szCPUSNHex);
	printf("%s\n", szCPUID);
	return true;
}

void GetCPUID()
{
	unsigned long s1,s2;
	unsigned char vendor_id[]="------------";
	//char vendor[64]={0},cpuid1[64]={0},cpuid2[64]={0};
	__asm{
		xor eax,eax//eax=0:ȡVendor��Ϣ
			cpuid//ȡcpu idָ�����Ring3��ʹ��
			mov dword ptr vendor_id,ebx
			mov dword ptr vendor_id[+4],edx
			mov dword ptr vendor_id[+8],ecx
	}
	printf("%s-",vendor_id);
	__asm{
		mov eax,01h//eax=1:ȡCPU���к�
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,eax
	}
	printf("%08X%08X",s1,s2);
	__asm{
		mov eax,03h
			xor ecx,ecx
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,ecx
	}
	printf("%08X%08X",s1,s2);
}

int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_INFO  sysInfo;
	OSVERSIONINFOEX osvi;

	GetSystemInfo(&sysInfo);

	printf("OemId : %d\n", sysInfo.dwOemId);
	printf("�������ܹ� : %d\n", sysInfo.wProcessorArchitecture);
	printf("ҳ���С : %d\n", sysInfo.dwPageSize);
	printf("Ӧ�ó�����С��ַ : %d\n", sysInfo.lpMinimumApplicationAddress);
	printf("Ӧ�ó�������ַ : %d\n", sysInfo.lpMaximumApplicationAddress);
	printf("���������� : %d\n", sysInfo.dwActiveProcessorMask);
	printf("���������� : %d\n", sysInfo.dwNumberOfProcessors);
	printf("���������� : %d\n", sysInfo.dwProcessorType);
	printf("�����ڴ�������� : %d\n", sysInfo.dwAllocationGranularity);
	printf("���������� : %d\n", sysInfo.wProcessorLevel);
	printf("�������汾 : %d\n", sysInfo.wProcessorRevision);

	osvi.dwOSVersionInfoSize=sizeof(osvi);
	if (GetVersionEx((LPOSVERSIONINFOW)&osvi))
	{
		printf("Version     : %d.%d\n", osvi.dwMajorVersion, osvi.dwMinorVersion);
		printf("Build       : %d\n", osvi.dwBuildNumber);
		printf("Service Pack: %d.%d\n", osvi.wServicePackMajor, osvi.wServicePackMinor);
	}

	printf("\n");

	base::CPU cpu;

	cout << "vendor_name:" << cpu.vendor_name() << " \n" 
		<< "signature:" << cpu.signature() << " \n"
		<< "stepping:" << cpu.stepping() << " \n"
		<< "model:" << cpu.model() << " \n"
		<< "family:" << cpu.family() << " \n"
		<< "type:" << cpu.type() << " \n"
		<< "extended_model:" << cpu.extended_model() << " \n"
		<< "extended_family:" << cpu.extended_family() << " \n"
		<< "has_mmx:" << cpu.has_mmx() << " \n"
		<< "has_sse:" << cpu.has_sse() << " \n"
		<< "has_sse2:" << cpu.has_sse2() << " \n"
		<< "has_sse3:" << cpu.has_sse3() << " \n"
		<< "has_ssse3:" << cpu.has_ssse3() << " \n"
		<< "has_sse41:" << cpu.has_sse41() << " \n"
		<< "has_sse42:" << cpu.has_sse42() << " \n"
		<< "has_avx:" << cpu.has_avx() << " \n"
		<< "has_avx2:" << cpu.has_avx2() << " \n"
		<< "has_aesni:" << cpu.has_aesni() << " \n"
		<< "has_non_stop_time_stamp_counter:" << cpu.has_non_stop_time_stamp_counter() << " \n"
		<< "cpu_brand:" << cpu.cpu_brand() << endl;

	printf("\n");

	char CPUString[0x20];
	char CPUBrandString[0x40];
	int CPUInfo[4] = {-1};
	int nSteppingID = 0;
	int nModel = 0;
	int nFamily = 0;
	int nProcessorType = 0;
	int nExtendedmodel = 0;
	int nExtendedfamily = 0;
	int nBrandIndex = 0;
	int nCLFLUSHcachelinesize = 0;
	int nAPICPhysicalID = 0;
	int nFeatureInfo = 0;
	int nCacheLineSize = 0;
	int nL2Associativity = 0;
	int nCacheSizeK = 0;
	int nRet = 0;
	unsigned    nIds, nExIds, i;
	bool    bSSE3NewInstructions = false;
	bool    bMONITOR_MWAIT = false;
	bool    bCPLQualifiedDebugStore = false;
	bool    bThermalMonitor2 = false;


	// __cpuid with an InfoType argument of 0 returns the number of
	// valid Ids in CPUInfo[0] and the CPU identification string in
	// the other three array elements. The CPU identification string is
	// not in linear order. The code below arranges the information 
	// in a human readable form.
	__cpuid(CPUInfo, 0);
	nIds = CPUInfo[0];
	memset(CPUString, 0, sizeof(CPUString));
	*((int*)CPUString) = CPUInfo[1];
	*((int*)(CPUString+4)) = CPUInfo[3];
	*((int*)(CPUString+8)) = CPUInfo[2];

	// Get the information associated with each valid Id
	for (i=0; i<=nIds; ++i)
	{
		__cpuid(CPUInfo, i);
		printf_s("\nFor InfoType %d\n", i); 
		printf_s("CPUInfo[0] = 0x%x\n", CPUInfo[0]);
		printf_s("CPUInfo[1] = 0x%x\n", CPUInfo[1]);
		printf_s("CPUInfo[2] = 0x%x\n", CPUInfo[2]);
		printf_s("CPUInfo[3] = 0x%x\n", CPUInfo[3]);

		// Interpret CPU feature information.
		if  (i == 1)
		{
			nSteppingID = CPUInfo[0] & 0xf;
			nModel = (CPUInfo[0] >> 4) & 0xf;
			nFamily = (CPUInfo[0] >> 8) & 0xf;
			nProcessorType = (CPUInfo[0] >> 12) & 0x3;
			nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
			nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
			nBrandIndex = CPUInfo[1] & 0xff;
			nCLFLUSHcachelinesize = ((CPUInfo[1] >> 8) & 0xff) * 8;
			nAPICPhysicalID = (CPUInfo[1] >> 24) & 0xff;
			bSSE3NewInstructions = (CPUInfo[2] & 0x1) || false;
			bMONITOR_MWAIT = (CPUInfo[2] & 0x8) || false;
			bCPLQualifiedDebugStore = (CPUInfo[2] & 0x10) || false;
			bThermalMonitor2 = (CPUInfo[2] & 0x100) || false;
			nFeatureInfo = CPUInfo[3];
		}
	}

	// Calling __cpuid with 0x80000000 as the InfoType argument
	// gets the number of valid extended IDs.
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	memset(CPUBrandString, 0, sizeof(CPUBrandString));

	// Get the information associated with each extended ID.
	for (i=0x80000000; i<=nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		printf_s("\nFor InfoType %x\n", i); 
		printf_s("CPUInfo[0] = 0x%x\n", CPUInfo[0]);
		printf_s("CPUInfo[1] = 0x%x\n", CPUInfo[1]);
		printf_s("CPUInfo[2] = 0x%x\n", CPUInfo[2]);
		printf_s("CPUInfo[3] = 0x%x\n", CPUInfo[3]);

		// Interpret CPU brand string and cache information.
		if  (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if  (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if  (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		else if  (i == 0x80000006)
		{
			nCacheLineSize = CPUInfo[2] & 0xff;
			nL2Associativity = (CPUInfo[2] >> 12) & 0xf;
			nCacheSizeK = (CPUInfo[2] >> 16) & 0xffff;
		}
	}

	// Display all the information in user-friendly format.

	printf_s("\n\nCPU String: %s\n", CPUString);

	if  (nIds >= 1)
	{
		if  (nSteppingID)
			printf_s("Stepping ID = %d\n", nSteppingID);
		if  (nModel)
			printf_s("Model = %d\n", nModel);
		if  (nFamily)
			printf_s("Family = %d\n", nFamily);
		if  (nProcessorType)
			printf_s("Processor Type = %d\n", nProcessorType);
		if  (nExtendedmodel)
			printf_s("Extended model = %d\n", nExtendedmodel);
		if  (nExtendedfamily)
			printf_s("Extended family = %d\n", nExtendedfamily);
		if  (nBrandIndex)
			printf_s("Brand Index = %d\n", nBrandIndex);
		if  (nCLFLUSHcachelinesize)
			printf_s("CLFLUSH cache line size = %d\n",
			nCLFLUSHcachelinesize);
		if  (nAPICPhysicalID)
			printf_s("APIC Physical ID = %d\n", nAPICPhysicalID);

		if  (nFeatureInfo || bSSE3NewInstructions ||
			bMONITOR_MWAIT || bCPLQualifiedDebugStore ||
			bThermalMonitor2)
		{
			printf_s("\nThe following features are supported:\n");

			if  (bSSE3NewInstructions)
				printf_s("\tSSE3 New Instructions\n");
			if  (bMONITOR_MWAIT)
				printf_s("\tMONITOR/MWAIT\n");
			if  (bCPLQualifiedDebugStore)
				printf_s("\tCPL Qualified Debug Store\n");
			if  (bThermalMonitor2)
				printf_s("\tThermal Monitor 2\n");

			i = 0;
			nIds = 1;
			while (i < (sizeof(szFeatures)/sizeof(const char*)))
			{
				if  (nFeatureInfo & nIds)
				{
					printf_s("\t");
					printf_s(szFeatures[i]);
					printf_s("\n");
				}

				nIds <<= 1;
				++i;
			}
		}
	}

	if  (nExIds >= 0x80000004)
		printf_s("\nCPU Brand String: %s\n", CPUBrandString);

	if  (nExIds >= 0x80000006)
	{
		printf_s("Cache Line Size = %d\n", nCacheLineSize);
		printf_s("L2 Associativity = %d\n", nL2Associativity);
		printf_s("Cache Size = %dK\n", nCacheSizeK);
	}

	printf("\nProcessorID:\n");
	INT32 dwBuf[4] = {0};
	__cpuidex(dwBuf, 1, 1);
	char szTmp[33]={NULL};
	sprintf(szTmp, "%08X%08X", dwBuf[3], dwBuf[0]);
	printf("%s\n",szTmp);

	printf("\nDetectCPU:\n");
	DetectCPU();

	printf("\nGetCPUID:\n");
	GetCPUID();

	getchar();

	return 0;
}

