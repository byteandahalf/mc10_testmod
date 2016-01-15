#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
using namespace std;

DWORD_PTR GetProcessBaseAddress(DWORD processID)
{
    DWORD_PTR   baseAddress = 0x00000000;
    HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    HMODULE     *moduleArray;
    LPBYTE      moduleArrayBytes;
    DWORD       bytesRequired;

    if(processHandle)
    {
        if(EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
        {
            if(bytesRequired)
            {
                moduleArrayBytes = (LPBYTE) LocalAlloc(LPTR, bytesRequired);

                if(moduleArrayBytes)
                {
                    unsigned int moduleCount;

                    moduleCount = bytesRequired / sizeof(HMODULE);
                    moduleArray = (HMODULE*) moduleArrayBytes;

                    if(EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
                    {
                        baseAddress = (DWORD_PTR) moduleArray[0];
                    }

                    LocalFree(moduleArrayBytes);
                }
            }
        }

        CloseHandle(processHandle);
    }

    return baseAddress;
}

int main()
{
	/* this base address is nothing so far */
    DWORD_PTR address = 0x00000000;
    HWND hwnd = FindWindow(0, L"Minecraft: Windows 10 Edition Beta");
    DWORD pid;
    int data = 0;
    int newData = 0;

    if(hwnd)
    {
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
        if(phandle)
        {
            DWORD_PTR base = GetProcessBaseAddress(pid);
            cout << "Base address located: " << (uintptr_t) base << endl;

            ReadProcessMemory(phandle, (LPCVOID)(base + address), &data, sizeof(data), 0);
        }
        else
        {
            cout << "Handle not found" << endl;
        }
    }
    else
    {
        cout << "Couldn't find window" << endl;
    }

    cin.get();

    cout << "Code successfully injected!" << endl;
    return 0;
}
