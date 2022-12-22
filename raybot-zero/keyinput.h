#pragma once

//thanks to yazzn https://www.unknowncheats.me/forum/c-and-c-/327461-kernel-mode-key-input.html
namespace g_Input {

    PEPROCESS csrssProc;
    PVOID gafAsyncKeyState;

    UINT8 uksBitmap[64] = { 0 };
    UINT8 uksRecentBitmap[32] = { 0 };

    bool initialize() {

        csrssProc = utils::process_by_name("csrss.exe", 1);

        //PVOID X = memory::get_system_module_base("\\SystemRoot\\System32\\win32kbase.sys");
		//PRNT("X: %p", X)

        //we have to hardcode it for now since RtlFindExportedRoutineByName keeps bluescreening me :(
        gafAsyncKeyState = (PVOID)((u64)
            memory::get_system_module_base("\\SystemRoot\\System32\\win32kbase.sys")
            + 0x24B8A0);

        if (!gafAsyncKeyState) {
            return false;
        }
    }

    bool is_key_down(UINT8 const vk) {
        uksRecentBitmap[vk / 8] &= ~(1 << vk % 8);
        return uksBitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2;
    }

    bool was_key_pressed(UINT8 const vk) {
        bool const result = uksRecentBitmap[vk / 8] & 1 << vk % 8;
        uksRecentBitmap[vk / 8] &= ~(1 << vk % 8);
        return result;
    }

    void update_key_state_bitmap() {

        auto previousKsBitmap = uksBitmap;
        SIZE_T size = 0;
        NTSTATUS status = MmCopyVirtualMemory(csrssProc,
            (void*)gafAsyncKeyState,
            PsGetCurrentProcess(), &uksBitmap,
            sizeof(UINT8[64]), KernelMode,
            &size);

        for (auto vk = 0u; vk < 256; ++vk) {
            if ((uksBitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2) &&
                !(previousKsBitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2))
                uksRecentBitmap[vk / 8] |= 1 << vk % 8;
        }
    }
}
