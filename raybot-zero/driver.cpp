#include "includes.h"

void unload_driver();

void main_thread(PVOID pStartContext) {

    UNREFERENCED_PARAMETER(pStartContext);
    PRNT("main thread launched!\n");

    g_Input::initialize();

    find_csgo:
    while ((_::pCsgoProc == NULL || _::pClient == NULL) && !_::bStopThread)
    {
        PRNT("waiting for csgo...\n");
        _::pCsgoProc = utils::process_by_name(xor("csgo.exe"), 0);
        if (_::pCsgoProc != NULL)
        {
            _::pClient = (client_t*)utils::get_module_base(_::pCsgoProc, xor (L"client.dll"));
        }

        utils::kernel_sleep(1000);
    }

    while (!_::bStopThread)
    {
        utils::kernel_sleep(1);

        //for input
        g_Input::update_key_state_bitmap();

        if (utils::is_process_terminating(_::pCsgoProc))
        {
            _::pCsgoProc = NULL;
            _::pClient = NULL;
            goto find_csgo;
        }

        g_Cycle::localPlayer = reinterpret_cast<entity_t*>(memory::read<u32>((_::pClient->base() + g_Offsets::dwLocalPlayer)));
        if (g_Cycle::localPlayer->base() == NULL)
            continue;

        //**************************
        //*******RUN FEATURES*******

        cheat::aim::run_triggerbot(g_Cycle::localPlayer);
        cheat::misc::run_bhop(g_Cycle::localPlayer);
        cheat::visuals::run_glow(g_Cycle::localPlayer);

        //*******END FEATURES*******
        //**************************
    }

    PRNT("main thread terminated!\n");
    PsTerminateSystemThread(0);
}

void unload_driver()
{
    //stop main thread
    _::bStopThread = true;
    if (_::hSysThread != 0)
    {
        PRNT("stopping main-thread...\n");
		
        LARGE_INTEGER timeout;
        timeout.QuadPart = -(5000) * 10 * 1000;
        ZwWaitForSingleObject(_::hSysThread, FALSE, &timeout);
        ZwClose(_::hSysThread);
    }
	
    PRNT("successfully unloaded! byebye :)\n");
}

void mf_device_unload(DRIVER_OBJECT* pDriverObject)
{
    UNREFERENCED_PARAMETER(pDriverObject);
    unload_driver();
}

NTSTATUS mf_device_close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);

    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    UNREFERENCED_PARAMETER(pDriverObject);
    UNREFERENCED_PARAMETER(pRegistryPath);

    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = &mf_device_close;
    pDriverObject->DriverUnload = &mf_device_unload;
	
    PRNT("starting main_thread...\n");
    NTSTATUS lThreadCreationStatus = PsCreateSystemThread(&_::hSysThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, (PKSTART_ROUTINE)main_thread, NULL);

    PRNT("successfully loaded! waiting for csgo...\n");
    return STATUS_SUCCESS;
}