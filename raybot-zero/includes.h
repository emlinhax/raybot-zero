#pragma once

#define PRNT(...) DbgPrintEx( DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[raybot-zero] " __VA_ARGS__);

//custom types to give the code some uniform
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

//all the global variables.
namespace _
{
	static bool bStopThread = false;
	static HANDLE hSysThread = 0;

	static PEPROCESS pCsgoProc = 0;
	static client_t* pClient;
}

//for anyone wondering: a cycle means a loop in the main thread. 
//g_Cycle just contains info about the current "mainthread-while-loop-tick" (ex: localPlayer etc).
//could also be referred to as a tick but i thought it might get confused with the actual ingame tick
//---
//TL;DR just put everything here that might change (round-end, switching servers etc)
namespace g_Cycle
{
	static entity_t* localPlayer;
}

//yes. you wanna complain? thought so.
#include "keyinput.h"
#include "features.h"
