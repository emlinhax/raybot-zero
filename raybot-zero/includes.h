#pragma once

#define PRNT(...) DbgPrintEx( DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[raybot-zero] " __VA_ARGS__);

//custom types
typedef unsigned char b8;
typedef int i32;
typedef unsigned int u32;
typedef float f32;
typedef unsigned __int64 u64;


//kernelmode floating point fix
extern "C" {
	unsigned int _fltused = 1;
}

#include "ntos.h"
#include "xorstr.h"
#include "utils.h"
#include "memory.h"
#include "mini_sdk.h"
//globals
namespace _
{
	static bool bStopThread = false;
	static HANDLE hSysThread = 0;

	static PEPROCESS pCsgoProc = 0;
	static client_t* pClient;
}

//for itzpax: a cycle means a loop in the main thread. 
//g_Cycle just contains info about the current cycle like localPlayer etc
namespace g_Cycle
{
	static entity_t* localPlayer;
}

#include "keyinput.h"
#include "features.h"
