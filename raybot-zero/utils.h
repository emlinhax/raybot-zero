#pragma once

#include <ntifs.h>
#include <ntddk.h>

namespace utils
{
    inline void kernel_sleep(int ms)
    {
        LARGE_INTEGER time;
        time.QuadPart = -(ms) * 10 * 1000;
        KeDelayExecutionThread(KernelMode, TRUE, &time);
    }

    inline uintptr_t get_module_base(PEPROCESS peProc, WCHAR* wcModuleName)
    {
        fnPsGetProcessWow64Process fPsGetProcessWow64Process;
        UNICODE_STRING sRoutineName;
        RtlInitUnicodeString(&sRoutineName, L"PsGetProcessWow64Process");
        fPsGetProcessWow64Process = (fnPsGetProcessWow64Process)MmGetSystemRoutineAddress(&sRoutineName);

        KAPC_STATE apcAttachState;
        KeStackAttachProcess(peProc, &apcAttachState);

        PPEB32 pPeb32 = (PPEB32)fPsGetProcessWow64Process(peProc);
        if (!pPeb32 || !pPeb32->Ldr)
        {
            KeUnstackDetachProcess(&apcAttachState);
            return 0;
        }

        for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList.Flink; pListEntry != &((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList; pListEntry = (PLIST_ENTRY32)pListEntry->Flink)
        {
            PLDR_DATA_TABLE_ENTRY32 pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
            if (wcscmp((PWCH)pEntry->BaseDllName.Buffer, wcModuleName) == 0)
            {
                uintptr_t pModuleBase = pEntry->DllBase;
                KeUnstackDetachProcess(&apcAttachState);
                return pModuleBase;
            }
        }

        KeUnstackDetachProcess(&apcAttachState);
        return 0;
    }

    inline PEPROCESS process_by_name(CHAR* cProcessName, int index)
    {
        PEPROCESS peSystemProcess = PsInitialSystemProcess;
        PEPROCESS peCurrentEntry = peSystemProcess;

        CHAR cImageName[15];

        b8 iterationFound = 0;
        bool firstItteration = true;
        while (true)
        {
            if ((peCurrentEntry == peSystemProcess) && !firstItteration)
                break;

            firstItteration = false;
            RtlCopyMemory((PVOID)(&cImageName), (PVOID)((uintptr_t)peCurrentEntry + 0x5a8), sizeof(cImageName));
            
            if (strstr(cImageName, cProcessName))
            {
                DWORD32 iActiveThreads;
                RtlCopyMemory((PVOID)&iActiveThreads, (PVOID)((uintptr_t)peCurrentEntry + 0x5f0), sizeof(iActiveThreads));
				
                if (iActiveThreads)
                {
                    if(iterationFound == index)
                        return peCurrentEntry;

                    iterationFound++;
                }
            }

            PLIST_ENTRY pList = (PLIST_ENTRY)((uintptr_t)(peCurrentEntry)+0x448);
            peCurrentEntry = (PEPROCESS)((uintptr_t)pList->Flink - 0x448);
        }

        return NULL;
    }

    inline BOOLEAN is_process_terminating(PEPROCESS pProcess)
    {
        LARGE_INTEGER zeroTime = { 0 };
        return KeWaitForSingleObject(pProcess, Executive, KernelMode, FALSE, &zeroTime) == STATUS_WAIT_0;
    }
}