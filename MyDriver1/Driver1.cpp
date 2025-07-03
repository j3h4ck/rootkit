#include <ntddk.h>
#include <stdlib.h>

#define DRIVER_NAME "Driver1"
#define IOCTL_SEND_DATA CTL_CODE(0x8000, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PROCESS_TERMINATE (0x0001)

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS DriverCreateClose (_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DriverDeviceIoControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


extern "C"{
	NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

		UNREFERENCED_PARAMETER(RegistryPath);

		DriverObject->DriverUnload = UnloadDriver;
		DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDeviceIoControl;

		DbgPrintEx(0, 0, "[%s] Driver Loaded!.\n", DRIVER_NAME);

		UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\Driver1");

		PDEVICE_OBJECT DeviceObject;

		NTSTATUS status = IoCreateDevice(DriverObject,
			0,
			&devName,
			FILE_DEVICE_UNKNOWN,
			FILE_DEVICE_SECURE_OPEN,
			FALSE,
			&DeviceObject);

		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "[%s] Failed to create device. 0x%08X\n", DRIVER_NAME, status);
			return status;
		}

		DbgPrintEx(0, 0, "[%s] Device \\Driver\\Driver1 Created.\n", DRIVER_NAME);


		DeviceObject->Flags |= DO_BUFFERED_IO;

		UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Driver1");

		status = IoCreateSymbolicLink(&symLink, &devName);


		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "[%s] Failed to create symbolic link. 0x%08X\n", DRIVER_NAME, status);
			IoDeleteDevice(DeviceObject);
			return status;
		}

		DbgPrintEx(0, 0, "[%s] Device symlink \\??\\Driver1 created.\n", DRIVER_NAME);


		return STATUS_SUCCESS;

	}
}

VOID UnloadDriver(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symName = RTL_CONSTANT_STRING(L"\\??\\Driver1");

	// Delete the symbolic link
	IoDeleteSymbolicLink(&symName);

	// Delete the device
	if (DriverObject->DeviceObject != NULL) {
		IoDeleteDevice(DriverObject->DeviceObject);
	}

	DbgPrintEx(0, 0, "[%s] Driver Unloaded and resources cleaned up.\n", DRIVER_NAME);
}


NTSTATUS DriverCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}




NTSTATUS DriverDeviceIoControl(PDEVICE_OBJECT, PIRP Irp) {
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	char* data = (char*)Irp->AssociatedIrp.SystemBuffer;

	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
	case IOCTL_SEND_DATA:
		if (!data || data[0] == '\0') {
			DbgPrintEx(0, 0, "[%s] Empty input.\n", DRIVER_NAME);
			break;
		}

		if (_strnicmp(data, "print ", 6) == 0) {
			DbgPrintEx(0, 0, "[%s] [print] %s\n", DRIVER_NAME, data + 6);
		}
		else if (_strnicmp(data, "kill ", 5) == 0) {
			ULONG pid = (ULONG)atoi(data + 5);
			DbgPrintEx(0, 0, "[%s] [kill] Attempting to kill PID %lu\n", DRIVER_NAME, pid);

			HANDLE hProcess = NULL;
			CLIENT_ID cid = { 0 };
			OBJECT_ATTRIBUTES objAttr;

			cid.UniqueProcess = (HANDLE)(ULONG_PTR)pid;
			cid.UniqueThread = NULL;

			InitializeObjectAttributes(&objAttr, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

			status = ZwOpenProcess(&hProcess, PROCESS_TERMINATE, &objAttr, &cid);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(0, 0, "[%s] Failed to open process: 0x%08X\n", DRIVER_NAME, status);
				break;
			}

			status = ZwTerminateProcess(hProcess, 0);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(0, 0, "[%s] Failed to terminate process: 0x%08X\n", DRIVER_NAME, status);
			}
			else {
				DbgPrintEx(0, 0, "[%s] Successfully terminated PID %lu\n", DRIVER_NAME, pid);
			}

			ZwClose(hProcess);
		}
		else {
			DbgPrintEx(0, 0, "[%s] Unknown command: %s\n", DRIVER_NAME, data);
		}
		break;

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


/*
NTSTATUS DriverDeviceIoControl(PDEVICE_OBJECT, PIRP Irp) {

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION CurrentStackLocation = IoGetCurrentIrpStackLocation(Irp);

	char* data = (CHAR *)Irp->AssociatedIrp.SystemBuffer;

	switch (CurrentStackLocation->Parameters.DeviceIoControl.IoControlCode) {

	case IOCTL_SEND_DATA:
	{
		DbgPrintEx(0, 0, "[%s] Data from usermode : %s.\n", DRIVER_NAME, data);
	}
	break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;

	}


	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;


}
*/