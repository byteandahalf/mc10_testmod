#include "stdafx.h"
#include <Windows.h>

// base address; TODO: Find this dynamically
static uintptr_t* const BaseAddress = (uintptr_t*)0x7ff6b24c0000;


uintptr_t* slideAddress(uintptr_t offset) {
	return (uintptr_t*)((uintptr_t)BaseAddress + offset);
}

bool shouldRenderBlock()
{
	return false;
}

bool minecraftH4x0r() {

	uintptr_t* const vtable_address = slideAddress(0x9965E8);
	// ptr to _ZTV5Block
	uintptr_t** const vtable = (uintptr_t** const)vtable_address;

	DWORD procId;
	HWND hWnd = FindWindow(0, L"Minecraft.Win10.DX11.exe");
	if (hWnd == NULL)
	{
		return false;
	}

	GetWindowThreadProcessId(hWnd, &procId);
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, procId);
	
	// Attempt to allow SE_DEBUG privileges to this process
	HANDLE hToken;
	TOKEN_PRIVILEGES _privileges;
	OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &_privileges.Privileges[0].Luid);

	_privileges.PrivilegeCount = 1;
	_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &_privileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
	{
		CloseHandle(process);
		return false;
	}

	// Store old and new protections
	DWORD protection = PAGE_READWRITE;
	DWORD oldProtection;

	// Index of Block::shouldRenderFace in the vtable
	int index = 6;

	// Attempt to set read/write protections
	if (::VirtualProtectEx(process, &vtable[index], sizeof(DWORD_PTR), protection, &oldProtection) == 0) {
		CloseHandle(process);
		return false;
	}
	else // Replace the vtable pointer for Block::shouldRenderFace
		vtable[index] = (uintptr_t*) &shouldRenderBlock;

	// Revert back to previous permissions
	if (::VirtualProtectEx(process, &vtable[index], sizeof(DWORD_PTR), oldProtection, &oldProtection) == 0) {
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
		if (!minecraftH4x0r())
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
