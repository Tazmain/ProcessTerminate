#include "stdafx.h"
#include <string>
#include <string_view>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <chrono>
#include <tchar.h>
#include <thread>
#include <psapi.h>
#include <Wtsapi32.h>

using namespace std;


BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode)
{
	DWORD dwDesiredAccess = PROCESS_TERMINATE;
	BOOL  bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (hProcess == NULL)
		return FALSE;

	BOOL result = TerminateProcess(hProcess, uExitCode);

	CloseHandle(hProcess);

	return result;
}

void Collect(string_view procName) {
	WTS_PROCESS_INFO* pWPIs = NULL;
	DWORD dwProcCount = 0;
	if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
	{
		//Go through all processes retrieved
		for (DWORD i = 0; i < dwProcCount; i++)
		{
			string name = string(pWPIs[i].pProcessName);
			if (name == procName)
			{
				cout << "found it " << endl;
				TerminateMyProcess(pWPIs[i].ProcessId, 1);
			}
		}
	}

	//Free memory
	if (pWPIs)
	{
		WTSFreeMemory(pWPIs);
		pWPIs = NULL;
	}
}

void WatchCo(string_view processName) {
	while (true)
	{
		Collect(processName);

		std::this_thread::sleep_for(15s);
	}

}


int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		cout << "Please enter the process name you want to watch for and terminate" << endl;
		return 0;
	}
	string proc = argv[1];

	std::thread t1(WatchCo, proc);

	t1.join();
	int z;
	cin >> z;
	return 0;
}

