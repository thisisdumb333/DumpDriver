#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "Kernel.h"
extern Driver::PKERNELDEVICE Device;
namespace Dump {

    template<typename T>
    T ReadMemory(DWORD procID, uintptr_t address)
    {
        T buffer;
        Device->Read(procID, reinterpret_cast<PVOID>(address), &buffer, sizeof(buffer));
        return buffer;
    }

    DWORD GetProcID(const wchar_t* ProcName) {

        DWORD procID = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// Get a handle to the specified snapshot, we specified SNAPPROCESS, so it will get include all processes

        if (hSnap == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32 procEntry; //Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) { //PROCESS32 returns TRUE if the first process in our snapshot (supplied via handle) has been copied into the buffer procEntry
            do {
                if (!wcscmp(procEntry.szExeFile, ProcName)) { //strcmp returns 0 (FALSE)  if STRING1 is equal to STRING 2, its not a bool return, but it might as well be

                    procID = procEntry.th32ProcessID; //we've found the process, copy the ID into the proc ID variable
                    break;
                }
            } while (Process32Next(hSnap, &procEntry)); //move on to the next process in the snapshot and do it again

        }
        CloseHandle(hSnap); // Close handle to snapshot, always close handles
        return procID;

    };

    uintptr_t GetModuleBase(DWORD ProcID) {

        MODULEENTRY32 modentry;
        modentry.dwSize = sizeof(modentry);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);

        if (snapshot == INVALID_HANDLE_VALUE)
            return NULL;

        Module32First(snapshot, &modentry);

        return reinterpret_cast<uintptr_t>(modentry.modBaseAddr);
    }


    bool DumpProcess(const wchar_t* ProcName, std::string name) {

        DWORD ProcID = (GetProcID(ProcName));
        uintptr_t modbase = GetModuleBase(ProcID);

        if (!modbase) {
            printf("[-] ModBase is null\n");
            return false;
        }

        printf("[+] ModBase is [%u]\n", modbase);

        IMAGE_DOS_HEADER DosHeader = ReadMemory<IMAGE_DOS_HEADER>(ProcID, modbase); //The Image DOS Header is the first few bytes of a process. So, if we have the modbase of a process, all we have to do is read that address into a DosHeader Structure

        if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE) { // Just as a check, if this is not the same, it means we didn't read the right address.

            printf("[-] Invalid Dos Header\n"); return false;                           
        }

        IMAGE_NT_HEADERS NTHeader = ReadMemory<IMAGE_NT_HEADERS>(ProcID, modbase);

        printf("[+] DosHeader is [%p]\n", DosHeader);

        PBYTE GameBuffer = reinterpret_cast<PBYTE>(VirtualAlloc(NULL, NTHeader.OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)); // Allocated memory somewhere for the buffer. We will copy the game into this buffer, so we use the NTheader to get the size of the image.
       
        size_t PageSize = 0x1000; // Size of a page, we will be reading it by each page;

        for (UINT page = 0x0; page < NTHeader.OptionalHeader.SizeOfImage; page += PageSize) {

            Device->Read(ProcID, reinterpret_cast<PVOID>(modbase + page), reinterpret_cast<PVOID>(GameBuffer + page), PageSize);
        }

        PIMAGE_DOS_HEADER pCopiedHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(GameBuffer); //The first bytes of this buffer, if we are sucessful in copying it, should equal the Dos header, so we simply cast it to that, we dont need to read memory because we already have access to all the bytes.
        PIMAGE_NT_HEADERS pCopiedNT     = reinterpret_cast<PIMAGE_NT_HEADERS>(GameBuffer + pCopiedHeader->e_lfanew);

        if (pCopiedHeader == nullptr) return false;

        if (pCopiedHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            printf("[-] Invalid pDos Header\n"); return false;
        }

        printf("[INFO] Image Dos Header [%p]\n", pCopiedHeader);
        printf("[INFO] Image NT Header [%p]\n",  pCopiedNT);

        auto CurrentSection = IMAGE_FIRST_SECTION(pCopiedNT);

        for (int i = 0; i < pCopiedNT->FileHeader.NumberOfSections; i++, ++CurrentSection) {

            std::string SectionName = reinterpret_cast<PCHAR>(CurrentSection->Name);

            printf("[+] fixing section [%s] address [%p] size [%i]\n", SectionName, CurrentSection->VirtualAddress, CurrentSection->Misc.VirtualSize);

            CurrentSection->PointerToRawData = CurrentSection->VirtualAddress;
            CurrentSection->SizeOfRawData = CurrentSection->Misc.VirtualSize;
        }

        std::string DumpName = "Dump" + name;

        DeleteFileA(DumpName.c_str());

        HANDLE DumpFile = CreateFileA(DumpName.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

        if (!DumpFile)
        {
            printf("[-] failed to create file\n");
            return FALSE;
        }

        if (WriteFile(DumpFile, GameBuffer, pCopiedNT->OptionalHeader.SizeOfImage, NULL, NULL)) {

            printf("[+] wrote dumped process to file [%s]\n", DumpName.c_str());
            return TRUE;
        }

        return FALSE;
    }


}

