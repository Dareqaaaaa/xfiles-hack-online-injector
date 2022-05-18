#include "ManualMapDll.h"
//#include "ManualInject.h"
char szExeFile[500] = {0};
//gunakan tlhelp api untuk mendapatkan nama target kita, ingat ! besar kecil dibedakan !
DWORD GetPID(LPSTR ProcName)
{
   PROCESSENTRY32 pe32;
   HANDLE hProcessSnap = INVALID_HANDLE_VALUE;

   //Tangkap beberapa proses yang sedang running
   hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
   if( hProcessSnap == INVALID_HANDLE_VALUE )return (0);

   pe32.dwSize = sizeof(PROCESSENTRY32);
   if( Process32First(hProcessSnap, &pe32) == 0 ) {
         CloseHandle( hProcessSnap ); 
         return (0);
   }
   do {//Loop untuk mencari apakah process yang akan diinject telah ditemukan ?
	    if(lstrcmp(ProcName,pe32.szExeFile)== 0){
			strcpy(szExeFile,pe32.szExeFile);
           return pe32.th32ProcessID;
		}
   }while(Process32Next(hProcessSnap,&pe32) );

   CloseHandle( hProcessSnap );
   SetLastError(0);
   return (0);
}

void SuspendProcess(DWORD processId, bool Suspend)
{
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);

    Thread32First(hThreadSnapshot, &threadEntry);

    do
    {
        if (threadEntry.th32OwnerProcessID == processId)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			
			if(Suspend == true)SuspendThread(hThread);
			else ResumeThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));

    CloseHandle(hThreadSnapshot);
}

HWND g_HWND=NULL;
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd,LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd,&lpdwProcessId);
    if(lpdwProcessId==lParam)
    {
        g_HWND=hwnd;
        return FALSE;
    }
    return TRUE;
}


typedef struct _DECRYPTDLLPARAM {
	char DllDecryptKey[150];
	int PE_EntryPoint;
	int PE_Raw_MainSection_Address;
	int PE_Raw_MainSection_Size;
	int PE_Virtual_MainSection_Address;
	int PE_Virtual_MainSection_Size;

} DECRYPTDLLPARAM, *PDECRYPTDLLPARAM;
BOOL WINAPI XHookDllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved){
	DECRYPTDLLPARAM *pDllParameter;
	__asm mov [pDllParameter],0
	
	if(dwReason == DLL_PROCESS_ATTACH){
	char *buffer = (char*)hModule;
	int a = pDllParameter->PE_Raw_MainSection_Address;
	for(int i = pDllParameter->PE_Virtual_MainSection_Address; i < pDllParameter->PE_Virtual_MainSection_Address + pDllParameter->PE_Raw_MainSection_Size; i++){	
		buffer[i] = buffer[i] ^ pDllParameter->DllDecryptKey[a % strlen(pDllParameter->DllDecryptKey)];
	a++;
	}
	
	}
	typedef BOOL (WINAPI * tDllMain)(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved);
	tDllMain DllMain;
	DllMain = (tDllMain)((DWORD)hModule + pDllParameter->PE_EntryPoint);
	*(DWORD*)((DWORD)hModule + pDllParameter->PE_EntryPoint + 0xC + 0x1) = (DWORD)hModule + pDllParameter->PE_Virtual_MainSection_Address;
	return DllMain(hModule,dwReason,lpvReserved);
}

BOOL InjectDLL(char* lpTargetName,char* lpDLLName)
{
	SetProgressBarValue(ProgressBarMainValue,0);
	SetLabelText(Label1Main,"Preparing to Inject DLL...");
	Sleep(1000);
	SetProgressBarValue(ProgressBarMainValue,10);

	bool nRet = false;
	DWORD dwTargetProcId,dwError = 0;
	char wsOut[200];
	LPSTR lpModulePath = (LPSTR)GlobalAlloc(GPTR,MAX_PATH);

	PIMAGE_DOS_HEADER pIDH;
    PIMAGE_NT_HEADERS pINH;
    PIMAGE_SECTION_HEADER pISH;
     
    HANDLE hProcess,hThread,hFile,hToken;
    PVOID buffer,image,mem;
    DWORD i,FileSize,ProcessId,ExitCode,read,dwLoadLibraryA,dwGetProcAddress;
	DWORD dwDllUnlockEP, dwDLLUnlockParam,dwAllocAddress,dwWritten,dwBaseRemoteDLL,dwNewThreadId = 0;

	HMODULE hKernel = GetModuleHandle("kernel32.dll");
	if(hKernel == NULL) 
		hKernel = LoadLibrary("kernel32.dll");


	dwLoadLibraryA = (DWORD)GetProcAddress(hKernel,"LoadLibraryA");


	SetProgressBarValue(ProgressBarMainValue,20);

	SetConsoleColorText(GREEN);
	file << "Injector InjectAgent V.1.1" << endl << endl;
	SetConsoleColorText(YELLOW);
	file << "InjectAgent -> LightInjector by RCD" << endl << endl;
	SetConsoleColorText(GREEN);
	sprintf(wsOut,"InjectAgent -> Initialize Process : %s, DLL : %s",lpTargetName,lpDLLName);
	file << wsOut << endl;
	file << "InjectAgent -> Initializing Parameter..." << endl;
	
	SetProgressBarValue(ProgressBarMainValue,25);

    GetModuleFileNameA(NULL,lpModulePath,MAX_PATH);
    for(int i = strlen(lpModulePath); i > 0; i--) { if(lpModulePath[i] == '\\') { lpModulePath[i+1] = 0; break;}}
    lstrcat(lpModulePath, lpDLLName); 
    WIN32_FIND_DATA WFD;
	file << "InjectAgent -> Searching for Local DLL..." << endl;

	SetProgressBarValue(ProgressBarMainValue,30);

    hFile=CreateFile(lpModulePath,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL); // Open the DLL
    if(hFile==INVALID_HANDLE_VALUE) {
        sprintf(Message,"DLL %s Not Found!",lpDLLName);
	    return 1;
	}
	CloseHandle(hFile);
	sprintf(wsOut,"InjectAgent -> DLL %s Found!",lpDLLName);
    file << wsOut << endl;

	SetProgressBarValue(ProgressBarMainValue,35);




	file << "InjectAgent -> Waiting for Process..." << endl;
	
	SetProgressBarValue(ProgressBarMainValue,60);
	SetLabelText(Label1Main,"Harap jalankan game anda setelah inject tertutup.");
	Sleep(3000);
	MainForm = false;
	do {
	   dwTargetProcId = GetPID(lpTargetName);
	   Sleep(20);
	}while(!dwTargetProcId);

	SetProgressBarValue(ProgressBarMainValue,80);
	SetLabelText(Label1Main,"Injecting DLL...");

	file << "InjectAgent -> Process Found!" << endl;

	file << "InjectAgent -> Opening Process..." << endl;

	

	//======================================== Lanjutttt =============================================================//
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwTargetProcId);
	if(hProcess)goto ProcessSuccess;

	SetConsoleColorText(RED);
	file << "InjectAgent -> OpenProcess() : Failed with PROCESS_ALL_ACCESS." << endl;

	file << "InjectAgent -> Opening Process in Lower Access..." << endl;
	
	SetConsoleColorText(GREEN);

	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE,FALSE,dwTargetProcId);
	
    if( hProcess == NULL) {
		dwError = GetLastError();
		sprintf(Message,"Can't Open The Process : %s Error : %d",lpTargetName,dwError);
	    return 1;
	}
ProcessSuccess:

	file << "InjectAgent -> OpenProcess() : Opened Successfully." << endl;
	while(!g_HWND){
		EnumWindows(EnumWindowsProcMy,dwTargetProcId);
		Sleep(0);
	}
	
	SuspendProcess(dwTargetProcId,true);
	/*
	dwDllUnlockEP =  (DWORD)VirtualAllocEx(hProcess,0,0x200,MEM_COMMIT,PAGE_READWRITE);
	
	
    if( dwDllUnlockEP == NULL) {
		dwError = GetLastError();
	    sprintf(Message,"Cannot Allocate Memory to Inject DLL, Error : %d",dwError);
	    return 1;
	}


	dwDLLUnlockParam =  (DWORD)VirtualAllocEx(hProcess,0,sizeof(DECRYPTDLLPARAM),MEM_COMMIT,PAGE_READWRITE);
	
	
    if( dwDLLUnlockParam == NULL) {
		dwError = GetLastError();
	    sprintf(Message,"Cannot Allocate Memory to Inject DLL, Error : %d",dwError);
	    return 1;
	}
	BYTE JumpPatch[7] = {0xB8, 0x00, 0x00, 0x00, 0x00,0xFF,0xE0};
	DWORD EntryPointPTR = DllParameter.PE_Reserved_Offset;
	FILE* fileHandle = fopen(lpModulePath, "r+b"); // r+ if you need char mode
	if(!fileHandle){
		DWORD dwError = GetLastError();
		sprintf(Message,"Cannot Open Memory to Inject DLL, Error : %d",dwError);
	    return 1;
	}
	

	fseek(fileHandle, DllParameter.PE_Reserved_Offset, SEEK_SET);
	fwrite(JumpPatch, 7, 1, fileHandle); // ? salah paham
	
	fseek(fileHandle, 0x0, SEEK_SET);

	fseek(fileHandle, DllParameter.PE_Reserved_Offset + 1, SEEK_SET);
	fwrite(&dwDllUnlockEP, 4, 1, fileHandle); // ? salah paham

	fseek(fileHandle, 0x0, SEEK_SET);


	fseek(fileHandle, DllParameter.PE_EntryPoint_Offset, SEEK_SET);
	fwrite(&EntryPointPTR, 4, 1, fileHandle); // fixed

	fclose(fileHandle);


	DWORD dwOldProtect = 0;
	VirtualProtect((LPVOID)XHookDllMain,0x7A,PAGE_EXECUTE_READWRITE,&dwOldProtect);
	DWORD Ofs_HookDllMain = (DWORD)XHookDllMain;
	*(DWORD*)(Ofs_HookDllMain + 0x5 + 0x3) = dwDLLUnlockParam;


	DECRYPTDLLPARAM WriteDllParam;
	strcpy(WriteDllParam.DllDecryptKey,DllParameter->DllDecryptKey);
	WriteDllParam.PE_EntryPoint = DllParameter.PE_EntryPoint;
	WriteDllParam.PE_Raw_MainSection_Address = DllParameter.PE_Raw_MainSection_Address;
	WriteDllParam.PE_Raw_MainSection_Size = DllParameter.PE_Raw_MainSection_Size;
	WriteDllParam.PE_Virtual_MainSection_Address = DllParameter.PE_Virtual_MainSection_Address;
	WriteDllParam.PE_Virtual_MainSection_Size = DllParameter.PE_Virtual_MainSection_Size;

	nRet = WriteProcessMemory(hProcess,(LPVOID)dwDllUnlockEP,(LPVOID)XHookDllMain,0x200,&dwWritten);

	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to Write Memory in Address : 0x%X at Process : %s Error : %d",dwDllUnlockEP,lpTargetName,dwError);
	    return 1;
	}

	nRet = WriteProcessMemory(hProcess,(LPVOID)dwDLLUnlockParam,&WriteDllParam,sizeof(DECRYPTDLLPARAM),&dwWritten);

	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to Write Memory in Address : 0x%X at Process : %s Error : %d",dwDLLUnlockParam,lpTargetName,dwError);
	    return 1;
	}
	ZeroMemory(&WriteDllParam,sizeof(DECRYPTDLLPARAM));

	
	*/

		cout << "InjectAgent -> Creating Virtual Memory..." << endl;
    dwAllocAddress =  (DWORD)VirtualAllocEx(hProcess,0,lstrlen(lpModulePath),MEM_COMMIT,PAGE_READWRITE);
	
	
    if( dwAllocAddress == NULL) {
		dwError = GetLastError();
	    sprintf(Message,"Cannot Allocate Memory to Inject DLL, Error : %d",dwError);
	    return 1;
	}


	nRet = WriteProcessMemory(hProcess,(LPVOID)dwAllocAddress,lpModulePath,lstrlen(lpModulePath),&dwWritten);

	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to Write Memory in Address : 0x%X at Process : %s Error : %d",dwAllocAddress,lpTargetName,dwError);
	    return 1;
	}
	
	
	

	cout << "InjectAgent -> WriteProcessMemory() : Memory Writing Success." << endl;
	
	
	
	cout << "InjectAgent -> Creating Remote Thread..." << endl;
//=========== SUSPEND DLU ============//
    hThread = CreateRemoteThread(hProcess,0,0,(LPTHREAD_START_ROUTINE)
		                dwLoadLibraryA,(LPVOID)dwAllocAddress,0,&dwNewThreadId);
						
	if( hThread == NULL) {
		dwError = GetLastError();
		sprintf(Message,"Failed to CreateThread on LoadLibrary! Error : %d",dwError);
        return 1;
	}

	cout << "InjectAgent -> CreateRemoteThread() : Thread Successfully Created." << endl;

	cout << "InjectAgent -> Waiting for LoadLibrary Return Code Execution..." << endl;

	WaitForSingleObject(hThread,INFINITE);

	bool gRet = GetExitCodeThread(hThread,&dwBaseRemoteDLL);
	
	if(dwBaseRemoteDLL == NULL || !gRet){
		dwError = GetLastError();
		sprintf(Message,"Failed to Get Last Injected module Base Address Error : %d",dwError);
	    return 1;
	}
	
//	cout << "InjectAgent -> GetExitCodeThread() : Success! (0x" << dwBaseRemoteDLL << ") Clearing Allocated Memory..." << endl;

	nRet = VirtualFreeEx(hProcess,(LPVOID)dwAllocAddress,0,MEM_RELEASE);
	
	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to FreeMemory in Address : 0x%X at Process : %s Error : %d",dwAllocAddress,lpTargetName,dwError);
	    return 1;
	}

	/*
	DWORD dwParamAddress,ArgToPass;
	HANDLE hSendCallback;
	bool IsUnpacked = false;
	SYSTEMTIME ArgTime;
 
	dwParamAddress =  (DWORD)VirtualAllocEx(hProcess,0,4096,MEM_COMMIT,PAGE_READWRITE);
	
    if( dwParamAddress == NULL) {
		dwError = GetLastError();
	    sprintf(Message,"Cannot Allocate Memory to Inject DLL, Error : %d",dwError);
	    return 1;
	}

	char CallbackBuffer[10] = {0};
	DWORD MyOnInject = 0;
	while(!IsUnpacked)
	{

		nRet = ReadProcessMemory(hProcess,(LPVOID)((DWORD)image + 0x649),(LPVOID)CallbackBuffer,10,0);
	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to Read Memory in Address : 0x%X at Process : %s Error : %d",((DWORD)image + 0x1000),lpTargetName,dwError);
	    return 1;
	}

	if(!memcmp((void*)(CallbackBuffer),(void*)"\xC9",1)){
		MyOnInject = *(DWORD*)(CallbackBuffer + 1);
		IsUnpacked = true;
	}

	}

	GetSystemTime(&ArgTime);
	ArgToPass = ArgTime.wDay;
	ArgToPass = ArgToPass + ArgTime.wHour;
	ArgToPass = ArgToPass * ArgTime.wYear;
	ArgToPass = ArgToPass * 7;
	
	MyParameter->Checker = ArgToPass;


	nRet = WriteProcessMemory(hProcess,(LPVOID)dwParamAddress,MyParameter,4096,0);

	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to Write Memory in Address : 0x%X at Process : %s Error : %d",dwParamAddress,lpTargetName,dwError);
	    return 1;
	}

	hSendCallback = CreateRemoteThread(hProcess,0,0,(LPTHREAD_START_ROUTINE)
		                MyOnInject,(LPVOID)dwParamAddress,0,0);

		if( hSendCallback == NULL) {
		dwError = GetLastError();
		sprintf(Message,"Failed to CreateThread on CallBack! Error : %d",dwError);
        return 1;
	}

	WaitForSingleObject(hSendCallback,INFINITE);

	nRet = VirtualFreeEx(hProcess,(LPVOID)dwParamAddress,0,MEM_RELEASE);
	
	if( nRet == false) {
		dwError = GetLastError();
		sprintf(Message,"Failed to FreeMemory in Address : 0x%X at Process : %s Error : %d",dwParamAddress,lpTargetName,dwError);
	    return 1;
	}
	
	*/

    CloseHandle(hProcess);
	

	SuspendProcess(dwTargetProcId,false);


	file << "InjectAgent -> Injection Success!..." << endl;
	SetProgressBarValue(ProgressBarMainValue,100);
	SetLabelText(Label1Main,"Injection Complete!");

	return 0;
}