
#include <iostream>
#include "dump.h"
Driver::PKERNELDEVICE Device;
int main()
{
    Device = new Driver::_KERNELDEVICE("\\\\.\\torgast");

    if (Dump::DumpProcess(L"PlanetSide 2 Play Client.exe", "PS2"))
        printf("Dumped");


    while (!GetAsyncKeyState(VK_DELETE) & 1) {};

    return 0;
}
