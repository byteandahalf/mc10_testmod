#include "stdafx.h"
#include <Windows.h>
#include <iostream>

// base address; TODO: Find this dynamically
static uintptr_t* const BaseAddress = (uintptr_t*)0x7ff6fa800000;

static void** mBlocks;


uintptr_t* SlideAddress(uintptr_t offset) {
	return (uintptr_t*)((uintptr_t)BaseAddress + offset);
}


uint32_t changeBedrockColor()
{
	return 0x00FF00FF; // purple
}

bool bedrockBlocksChests()
{
	return false;
}

bool removeBedrockCollision()
{
	return false;
}



bool minecraftH4x0r() {

	mBlocks = (void**) SlideAddress(0xA75750);
	// ptr to _ZTV12BedrockBlock
	uintptr_t** const vtable = (uintptr_t** const) (*((void***) mBlocks[7]));

	DWORD procId = GetCurrentProcessId();
	if (procId == NULL)
	{
		return false;
	}

	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, procId);


	if (GetLastError() != ERROR_SUCCESS)
	{
		CloseHandle(process);
		return false;
	}

	// Store old and new protections
	DWORD protection = PAGE_READWRITE;
	DWORD oldProtection;

	// Attempt to set read/write protections
	if (::VirtualProtectEx(process, vtable, sizeof(DWORD_PTR) * 60, protection, &oldProtection) == 0) {
		CloseHandle(process);
		return false;
	}
	else {
		// setup vtable hooks
		vtable[5] = (uintptr_t*)&bedrockBlocksChests;
		vtable[14] = (uintptr_t*)&removeBedrockCollision;
		vtable[59] = (uintptr_t*)&changeBedrockColor;
	}

	// Revert back to previous permissions
	if (::VirtualProtectEx(process, vtable, sizeof(DWORD_PTR) * 60, oldProtection, &oldProtection) == 0) {
		CloseHandle(process);
		return false;
	}

	CloseHandle(process);
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if(!minecraftH4x0r())
		{
			; // TODO: Popup that we failed
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
