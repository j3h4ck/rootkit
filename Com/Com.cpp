#include <windows.h>
#include <stdio.h>
#include <string.h>

#define IOCTL_SEND_DATA CTL_CODE(0x8000, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)


int main()
{
    HANDLE hDriver = CreateFileW(L"\\\\.\\Driver1",
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hDriver == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to open the driver. Error code %d\n", GetLastError());
        return 1;
    }

    char input[1024];
    DWORD bytesReturned;

    printf("[*] Type messages to send to the driver. Type 'close' to exit.\n");

    while (1) {
        printf(">> ");
        fgets(input, sizeof(input), stdin);

        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
            input[len - 1] = '\0';

        if (_stricmp(input, "close") == 0) {
            printf("[*] Closing communication.\n");
            break;
        }

        BOOL success = DeviceIoControl(hDriver,
            IOCTL_SEND_DATA,
            input,
            (DWORD)strlen(input) + 1,
            NULL,
            0,
            &bytesReturned,
            NULL);

        if (!success) {
            printf("[-] Failed to send data. Error code %d\n", GetLastError());
        }
        else {
            printf("[+] Sent: \"%s\"\n", input);
        }
    }

    CloseHandle(hDriver);
    return 0;
}