#pragma once
#include <ntifs.h>
#include <intrin.h>

namespace memory
{
	inline void __wp_off()
	{
		auto cr0 = __readcr0();
		cr0 &= 0xfffffffffffeffff;
		__writecr0(cr0);
		_disable();
	}

	inline void __wp_on()
	{
		auto cr0 = __readcr0();
		cr0 |= 0x10000;
		_enable();
		__writecr0(cr0);
	}

	template <typename T>
	inline auto _read(u32 address) -> T
	{
		if (MmIsAddressValid((PVOID)address))
			return *reinterpret_cast<T*>(address);

		PRNT("INVALID READ AT ADDRESS: 0x%p\n", address);
		T zero;
		RtlZeroMemory(&zero, sizeof(T));
		return zero;
	}

	template <typename T>
	inline void _write(u32 address, T val)
	{
		if (MmIsAddressValid((PVOID)address))
			*reinterpret_cast<T*>(address) = val;
		else
			PRNT("INVALID WRITE AT ADDRESS: 0x%p\n", address);
	}

	template <typename T>
	inline auto read(u32 address) -> T
	{
		KAPC_STATE apcState;
		KeStackAttachProcess(_::pCsgoProc, &apcState);
		auto ret = _read<T>(address);
		KeUnstackDetachProcess(&apcState);

		return ret;
	}

	template <typename T>
	inline void write(u32 address, T val)
	{
		KAPC_STATE apcState;
		KeStackAttachProcess(_::pCsgoProc, &apcState);
		_write(address, val);
		KeUnstackDetachProcess(&apcState);
	}

	PVOID get_system_module_base(const char* module_name)
	{
		ULONG bytes = 0;
		NTSTATUS status = ZwQuerySystemInformation(0x0B, NULL, bytes, &bytes);

		if (!bytes)
			return NULL;

		PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x4e554c4c);

		status = ZwQuerySystemInformation(0x0B, modules, bytes, &bytes);

		if (!NT_SUCCESS(status))
			return NULL;

		PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
		PVOID module_base = 0, module_size = 0;

		for (ULONG i = 0; i < modules->NumberOfModules; i++)
		{
			if (strcmp((char*)module[i].FullPathName, module_name) == NULL)
			{
				module_base = module[i].ImageBase;
				module_size = (PVOID)module[i].ImageSize;
				break;
			}
		}

		if (modules)
			ExFreePoolWithTag(modules, NULL);

		if (module_base <= NULL)
			return NULL;

		return module_base;
	}

	PVOID get_system_module_export(const char* module_name, LPCSTR routine_name)
	{
		PVOID lpModule = get_system_module_base(module_name);

		if (!lpModule)
			return NULL;

		//crashes the system :(
		return RtlFindExportedRoutineByName(lpModule, routine_name);
	}
}
