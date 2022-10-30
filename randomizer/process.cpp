#include <iostream>
#include <sstream>
#include <Windows.h>
#include <TlHelp32.h>

void throwWithGetLastError(std::string message) {
	DWORD error = GetLastError();
	std::ostringstream oss;
	oss << message << " Error code is " << error;
	throw std::runtime_error(oss.str());
}

int getProcess() {
	std::string processName = "ys1plus.exe";

	// Retrieve the process ID
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(processEntry);
	HANDLE processListSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!processListSnapshot) {
		throw std::runtime_error("Unable to list running processes.");
	}
	for ( ;; ) {
		if (!Process32Next(processListSnapshot, &processEntry)) {
			throw std::runtime_error("Unable to find Ys 1 Process. Is the game running?");
		}
		if (processName == processEntry.szExeFile) {
			CloseHandle(processListSnapshot);
			return processEntry.th32ProcessID;
		}
	}
}

void loadDll(DWORD pID) {
	// Get the full path of the dll
	char dll[] = "archipelago.dll";
	char dllPath[MAX_PATH];
	GetFullPathName(dll, MAX_PATH, dllPath, NULL);

	// Open the process
	DWORD dwDesiredAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;
	HANDLE ysProcess = OpenProcess(dwDesiredAccess, TRUE /*bInheritHandle*/, pID);
	if (!ysProcess) { throwWithGetLastError("Failed to obtain Ys process handle."); }

	// Allocate and write the dll path to the game process memory
	LPVOID remoteFile = VirtualAllocEx(
		ysProcess,
		NULL /*lpAddress (starting address for pages)*/,
		strlen(dllPath) + 1,
		MEM_COMMIT | MEM_RESERVE, /*flAllocationType*/
		PAGE_READWRITE /*flProtect*/
	);
	if (!remoteFile) { throwWithGetLastError("Failed to allocate dll path memory."); }
	if (!WriteProcessMemory(ysProcess, remoteFile, dllPath, strlen(dllPath) + 1, NULL /*lpNumberOfBytesWritten*/)) {
		throwWithGetLastError("Failed to write dll path memory.");
	}

	// Create a new thread in the YsProcess which loads the dll.
	LPVOID loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (!loadLibraryAddress) { throwWithGetLastError("Failed to retrieve LoadLibrary address."); }
	HANDLE dllExecutorThread = CreateRemoteThread(
		ysProcess,
		NULL, /*lpThreadAttributes (security attributes)*/
		0, /*dwStackSize*/
		(LPTHREAD_START_ROUTINE)loadLibraryAddress, /*lpStartAddress*/
		remoteFile,	/*lpParameter*/
		0, /*dwCreationFlags (thread creation flags)*/
		NULL /*lpThreadId*/
	);
	if (!dllExecutorThread) { throwWithGetLastError("Failed to create LoadLibrary thread."); }

	// Wait for execution and cleanup
	WaitForSingleObject(dllExecutorThread, INFINITE);
	CloseHandle(dllExecutorThread);
	VirtualFreeEx(ysProcess, dllPath, 0, MEM_RELEASE);
	CloseHandle(ysProcess);
}

int main(int argc, char* argv[]) {
	int pID = getProcess();
	loadDll(pID);
	return 0;
}