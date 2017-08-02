//---------------------------------------------------------------------------
//
// ConsCtl.cpp
//
// SUBSYSTEM: 
//              Monitoring process creation and termination  
//
// MODULE:    
//				Control application for monitoring NT process and 
//              DLLs loading observation. 
//
// DESCRIPTION:
//
// AUTHOR:		Ivo Ivanov
//                                                                         
//---------------------------------------------------------------------------

#include "Common.h"
#include <conio.h>
#include <tchar.h>
#include "ApplicationScope.h"
#include "CallbackHandler.h"
#include "vt/VtFile.h"
#include "vt/VtResponse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "jansson\jansson.h"
#include <Psapi.h>
#include <Windows.h>
#include "picosha2.h"

//---------------------------------------------------------------------------
// 
// class CWhatheverYouWantToHold
//
// Implements a dummy class that can be used inside provide callback 
// mechanism. For example this class can manage sophisticated methods and 
// handles to a GUI Win32 Window. 
//
//---------------------------------------------------------------------------

class CWhatheverYouWantToHold
{
public:
	CWhatheverYouWantToHold()
	{
		_tcscpy(m_szName, TEXT("This could be any user-supplied data."));
		hwnd = NULL;
	}
private:
	TCHAR  m_szName[MAX_PATH];
	// 
	// You might want to use this attribute to store a 
	// handle to Win32 GUI Window
	//
	HANDLE hwnd;
};


//---------------------------------------------------------------------------
// 
// class CMyCallbackHandler
//
// Implements an interface for receiving notifications
//
//---------------------------------------------------------------------------
class CMyCallbackHandler: public CCallbackHandler
{
	//
	// Triggered when event occurs
	//
	virtual void OnProcessEvent(
		PQUEUED_ITEM pQueuedItem, 
		PVOID        pvParam
		)
	{
		char* path = NULL;
		TCHAR szFileName[MAX_PATH];
		CWhatheverYouWantToHold* pParam = static_cast<CWhatheverYouWantToHold*>(pvParam);
		
		//process notification
		if (NULL != pQueuedItem)
		{
			TCHAR szBuffer[1024];
			HANDLE processHandle = NULL;
			TCHAR path[MAX_PATH];
			::ZeroMemory(
				reinterpret_cast<PBYTE>(szBuffer),
				sizeof(szBuffer)
				);
			GetProcessName(
				reinterpret_cast<DWORD>(pQueuedItem->hProcessId), 
				szFileName, 
				MAX_PATH
				);

			processHandle = OpenProcess(
				PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_QUERY_INFORMATION,
				false, 
				reinterpret_cast<DWORD>(pQueuedItem->hProcessId)
			);

			if (GetModuleFileNameEx(processHandle, NULL, path, MAX_PATH) == 0) {
				//this means we failed to get the name! default allow execution
				//this will fail without admin privilege
				pQueuedItem->bCreate = true;
			}
			else {
				//sometimes this char cast will be unneeded
				pQueuedItem->bCreate = VTscan((char*)path);
			}
			
		} // if
	}
};

/*
//---------------------------------------------------------------------------
// Perform
//
// Thin wrapper around __try..__finally
//---------------------------------------------------------------------------
void Perform(
	CCallbackHandler*        pHandler,
	CWhatheverYouWantToHold* pParamObject
	)
{
	int MAX_TEST_PROCESSES = 0;
	DWORD processArr[MAX_TEST_PROCESSES] = {0};
	int i;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	TCHAR szBuffer[MAX_PATH];  // buffer for Windows directory
	::GetWindowsDirectory(szBuffer, MAX_PATH);
	_tcscat(szBuffer, TEXT("\\notepad.exe"));

	//
	// Create the only instance of this object
	//
	CApplicationScope& g_AppScope = CApplicationScope::GetInstance(
		pHandler     // User-supplied object for handling notifications
		);
	__try
	{
		//
		// Initiate monitoring
		//
		g_AppScope.StartMonitoring(
			pParamObject // Pointer to a parameter value passed to the object 
			);
		
		for (i = 0; i < MAX_TEST_PROCESSES; i++)
		{
			HANDLE hProcess = ::OpenProcess(
				PROCESS_ALL_ACCESS, 
				FALSE,
				processArr[i]
				);
			if (NULL != hProcess)
			{
				__try
				{
					// Close Notepad's instances
					::TerminateProcess(hProcess, 0);
				}
				__finally
				{
					::CloseHandle(hProcess);
				}
			} // if
			::Sleep(10);
		} // for
	
		while(!kbhit())
		{
		}
		_getch();
	}
	__finally
	{
		//
		// Terminate the process of observing processes
		//
		g_AppScope.StopMonitoring();
	}
}
*/

//---------------------------------------------------------------------------
// 
// Entry point
//
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	CMyCallbackHandler      myHandler;
	CWhatheverYouWantToHold myView; 
	//Perform( &myHandler, &myView );

	return 0;
}
//--------------------- End of the file -------------------------------------

/*
 *
 */
bool VTscan(char* path)
{
	int c;
	int ret = 0;
	struct VtFileDist *file_dist;
	int repeat = 3;
	int sleep_sec = 3;
	struct CallbackData cb_data = { .counter = 0 };
	char* fhash;

	//make sure file is openable
	FILE* file = fopen(path, "rb");
	if (!file)	//if not opened
	{
		printf("Error: File cannot be opened.\n");
		printf("Allowing execution by default.\n");
		return false;
	}

	std::ifstream ifs(path, std::ios::binary);
	std::vector<unsigned char> hash(32);
	picosha2::hash256(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), hash.begin(), hash.end());


	unsigned char hash[SHA256_DIGEST_LENGTH];	//unsigned fixes problems with unmatching hashes
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	char* buffer = malloc(bufSize);
	int bytesRead = 0;
	int ref = 0;

	if (!buffer) return false;

	while ((bytesRead = fread(buffer, 1, bufSize, file)))
	{
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);

	sha256_hash_string(hash, fhash);	//bad naming
	fclose(file);
	free(buffer);

	file_dist = VtFileDist_new();

	ret = VtFile_rescanHash(file_scan, fhash, 0, 0, 0, NULL, false);
	if (ret) {
		printf("Error: %d \n", ret);
	}
	else {
		response = VtFile_getResponse(file_scan);
		//we're really only interested in # of references for this application
		ref = VtResponse_get(response);
	}
	if (ref > 3) {
		printf("References found!\n");
		printf("Consider further investigation of the file at:"\n);
		printf("\t %s", *path);
		return true;
	}
	else return false;

}
