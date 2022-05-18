#include "curl\curl.h"
#pragma comment (lib,"libcurl.lib")

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
int ExpiredRemain = 0;
int ReceivedBytes = 0;

size_t write_to_char(void *ptr, size_t size, size_t count, void *stream) {
  memcpy(stream,(void*)ptr,size*count);
  ReceivedBytes = count;
  return size*count;
}
size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) {
  ((string*)stream)->append((char*)ptr, 0, size*count);
  return size*count;
}
__int64 GetFileSize(char* FileName)
{
    HANDLE hFile = CreateFile(FileName, GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile==INVALID_HANDLE_VALUE)
        return -1; // error condition, could call GetLastError to find out more

    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size))
    {
        CloseHandle(hFile);
        return -1; // error condition, could call GetLastError to find out more
    }

    CloseHandle(hFile);
    return size.QuadPart;
}

bool CheckFile = false;
bool Accepted = false;
bool IsAborted = false;
bool IsComplete = false;

__int64 dwRemoteFileSize, dwLocalFileSize = 0;

float LastCallNowDownloaded = 0;
 double lTotalToDownload, lNowDownloaded = 0;
 bool hCreate = false;
bool IsChecked = false;
bool PrintSize = false;
bool bMeterTerminate = 0;

int LocalFileCrc = 0;
int RemoteFileCrc = 0;

#define CPLUSPLUS_MIN_DLL_SIZE 7.5 * 1000
bool InitializeMeterFunc = false;
void Meter_func()
{
	
		

	while(1){

		if(bMeterTerminate){
		bMeterTerminate = false;
		TerminateThread(GetCurrentThread(),0);
		}
		
		if(!InitializeMeterFunc){
		InitializeMeterFunc = true;
		dwRemoteFileSize = lTotalToDownload;

		int kbRemote= dwRemoteFileSize / 1000;
		char Console[1000];

		if(CheckFile){
		if(dwLocalFileSize == dwRemoteFileSize && LocalFileCrc == RemoteFileCrc){
			IsComplete = true;
			TerminateThread(GetCurrentThread(),0);
		}
		}
		SetProgressBarValue(ProgressBarMainValue,40);
		SetLabelText(Label1Main,"Downloading DLL...");
		ShowFormUpdate = true;
		CheckFile = false;
		file << "cURL        -> File Downloading Started." << endl;
		
		}

		double SpeedDownloaded = ((lNowDownloaded - LastCallNowDownloaded) / 1000);
		LastCallNowDownloaded = lNowDownloaded;

		double fractiondownloaded = lNowDownloaded / lTotalToDownload;
		int kbTotalToDownload = lTotalToDownload / 1000;
		int kbNowDownloaded	  = lNowDownloaded / 1000;
		int percent = fractiondownloaded*100;
	
		SetProgressBarValue(ProgressBarUpdateValue,percent);
		sprintf(Label2Updater,"%d KB of %d KB Completed with %4.2f KB/s",kbNowDownloaded,kbTotalToDownload,SpeedDownloaded);
	
		if(percent == 100){

			file << endl << "cURL        -> Remote DLL File Downloaded!" << endl;
			IsComplete = true;
			TerminateThread(GetCurrentThread(),0);
		}
		Sleep(1000);
		}
}
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, 
                    double TotalToUpload, double NowUploaded)
{
	if(IsAborted || IsComplete)return true;
	
	if(!hCreate){
		CreateThread(0,0,(LPTHREAD_START_ROUTINE)Meter_func,0,0,0);
		hCreate = true;
	}


	lTotalToDownload = DllParameter->DllSize;
	lNowDownloaded = NowDownloaded;

	return 0;
}


#include <stdio.h>

typedef unsigned long ULONG;

void InitCrcTable(void);
ULONG Reflect(ULONG , char);
int Get_CRC(unsigned char*, ULONG);
long FileSize(FILE*);

ULONG crc32_table[256];
ULONG ulPolynomial = 0x04c11db7;

void InitCrcTable()
{

    // 256 values representing ASCII character codes.
    for(int i = 0; i <= 0xFF; i++)
    {
        crc32_table[i]=Reflect(i, 8) << 24;
        for (int j = 0; j < 8; j++)
            crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
        crc32_table[i] = Reflect(crc32_table[i], 32);
    }

}

//////////////////////////////////////////////////////////////
// Reflection is a requirement for the official CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.
//////////////////////////////////////////////////////////////////////////

ULONG Reflect(ULONG ref, char ch)
{                                 // Used only by Init_CRC32_Table()

    ULONG value(0);

    // Swap bit 0 for bit 7
    // bit 1 for bit 6, etc.
    for(int i = 1; i < (ch + 1); i++)
    {
        if(ref & 1)
            value |= 1 << (ch - i);
        ref >>= 1;
    }
    return value;
}

///////////////////////////////////////////////////////////////

int Get_CRC(DWORD buffer, ULONG bufsize)
{

    ULONG  crc(0xffffffff);
    int len;
    len = bufsize;
    // Save the text in the buffer.

    // Perform the algorithm on each character
    // in the string, using the lookup table values.

    for(int i = 0; i < len; i++)
          crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *(BYTE*)(buffer + i)];
    

    // Exclusive OR the result with the beginning value.
    return crc^0xffffffff;

}

////////////////////////////////////////////////////////////

long FileSize(FILE *input)
{

  long fileSizeBytes;
  fseek(input, 0, SEEK_END);
  fileSizeBytes = ftell(input);
  fseek(input, 0, SEEK_SET);

  return fileSizeBytes;
 

}

void UnpackDllInMemory(DWORD buffer,DWORD size)
{
	 for(int i = 0; i < size; i++)
	{
		*(BYTE*)(buffer + i) = *(BYTE*)(buffer + i) ^ DllParameter->DllDecryptKey[i % 100];
	}

}

char DllPath[500];
DWORD WINAPI ThreadAntiDllMalfunction (LPVOID param)
{
	FILE *fs = fopen(DllPath, "rb");   //open file for reading 

	long bufsize = FileSize(fs), result;
	
	 fclose(fs);
	while(true)
	{
	DWORD buffer = (DWORD)VirtualAlloc(0,bufsize,MEM_COMMIT,PAGE_READWRITE);
	FILE *fs = fopen(DllPath, "rb");   //open file for reading 
	result = fread ((void*)buffer,1,bufsize,fs);
	fclose(fs);

	UnpackDllInMemory(buffer,bufsize);

	InitCrcTable(); 
	LocalFileCrc = Get_CRC(buffer, bufsize);

	if(LocalFileCrc != RemoteFileCrc){
		file << "ThreadException : DLL Malfunction detected" << endl;
		TerminateProcess(GetCurrentProcess(),0);
	}
	VirtualFree((LPVOID)buffer,0,MEM_RELEASE);
		Sleep(100);
	}
	return 0;
}



int GetDllLink(char *DllKey, char *gametype)
{
	char PostURL[5000] = {0};
	int Ret = -1;
	SYSTEMTIME AuthTime;
	DWORD AuthKey = 0;
	GetSystemTime(&AuthTime);
	AuthKey = AuthTime.wMilliseconds * AuthTime.wSecond * AuthTime.wMinute * 16;

	if(AuthKey == 0)AuthKey = 15*534;

	sprintf(PostURL,"http://www.xfileshack.net/XFilesHack/Protected/QueryFile.php?key=%s&auth=%d&hdsn=%s&gametype=%s",DllKey,AuthKey,MyParameter->HDSN,gametype);

	CURL *curl;
	CURLcode res;
	char Input[1000] = {0}; // Create 1 KB Blocks Memory

	char UserAgent[100] = {0};
	sprintf(UserAgent,"%d",AuthKey);
	curl = curl_easy_init();
    if (curl) {

	curl_easy_setopt(curl, CURLOPT_URL, DeBlank(PostURL));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_char);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Input);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent);
	curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT,true);
	curl_easy_setopt(curl, CURLOPT_USERPWD, "xfileshack:op4n[]o1y(Jr{S%)D7");
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

    res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	}
	char key[200] = {0};
	char Output[1000] = {0};// Create 1 KB Blocks Memory
	sprintf(key,"%d",AuthKey * 7);
	for(int i = 0; i < ReceivedBytes; i++)
	{
		Output[i] = Input[i] ^ key[i % strlen(key)];
	}
	string Data;
	stringstream pData;
	string line;
	pData << Output;
	getline(pData,line);
	if(!strcmp(line.c_str(),"VALID")){
		getline(pData,line);
		RemoteFileCrc = strtoul(line.substr(0, 16).c_str(), NULL, 16);
		getline(pData,line);
		strcpy(DllParameter->DllDecryptKey,line.c_str());
		getline(pData,line);
		sprintf(DllParameter->DllLink,"http://www.xfileshack.net/XFilesHack/Protected/GetFile.php?name=%s",line.c_str());
		getline(pData,line);
		DllParameter->DllSize = (double)atoi(line.c_str());
		return 1;

	}
	else if(!strcmp(line.c_str(),"TOKENEXPIRED")){
		file << "SecureDownloader -> Token expired due 60 sec inactivity. Please restart program." << endl;
		Sleep(5000);
	}
	else {
		file << "SecureDownloader -> Received unrecognized response from server : " << endl << Input << endl;
	}
	return 0;
}

void XFilesInject_UpdateDLL(char* lpLocalDLLName)
{
	SetProgressBarValue(ProgressBarMainValue,0);
	SetLabelText(Label1Main,"Harap pilih game yang ingin anda mainkan di dialog berikut.");

	SetConsoleColorText(SKYBLUE);
	file << "Injector UpdateAgent V.1.1" << endl << endl;

	SetConsoleColorText(YELLOW);
	file << "UpdateAgent -> UpdateAgent v.1.1 r6" << endl;
	file << "cURL        -> LibCurl 7.18.0 MSVC" << endl << endl;

	SetConsoleColorText(SKYBLUE);
	file << "UpdateAgent -> Please select your game on window below." << endl;
	ShowFormSelectGame = true;
	while(!GameSelected)Sleep(50);

	SetProgressBarValue(ProgressBarMainValue,20);
	SetLabelText(Label1Main,"Checking Game Update Server...");

	file << "UpdateAgent -> Selected game : " << DllGameType << endl;
	if(!GetDllLink(DllParameter->DllKey,DllGameType)){
		file << "SecureDownloader -> Request rejected by server." << endl;
		exit(0);
	}
	

	
	SetProgressBarValue(ProgressBarMainValue,25);
	SetLabelText(Label1Main,"Initializing Updater...");

	file << "UpdateAgent -> Initializing Parameter..." << endl;

	LPSTR lpModulePath = (LPSTR)GlobalAlloc(GPTR,MAX_PATH);


    GetModuleFileNameA(NULL,lpModulePath,MAX_PATH);
    for(int i = strlen(lpModulePath); i > 0; i--) { if(lpModulePath[i] == '\\') { lpModulePath[i+1] = 0; break;}}
    lstrcat(lpModulePath, lpLocalDLLName); 

	strcpy(DllPath,lpModulePath);
    WIN32_FIND_DATA WFD;

	CURL *curl;
	FILE *fp;
	CURLcode res;
	file << "UpdateAgent -> Searching for Local DLL File..." << endl;

	if(FindFirstFileA(lpModulePath,&WFD) == INVALID_HANDLE_VALUE) {
	file << "UpdateAgent -> Local DLL File not Found" << endl;
	file << "UpdateAgent -> Local DLL File will be Downloaded" << endl;
		CheckFile = false;
	}
	else {
	file << "UpdateAgent -> Local DLL File found!" << endl;
	file << "UpdateAgent -> Local DLL File will be Compared with Remote DLL File." << endl;
	

	FILE *fs = fopen(lpModulePath, "rb");   //open file for reading 
	
	
	long bufsize = FileSize(fs), result;
	DWORD buffer = (DWORD)VirtualAlloc(0,bufsize,MEM_COMMIT,PAGE_READWRITE);

	 // copy the file into the buffer:
	 result = fread ((void*)buffer,1,bufsize,fs);
	 fclose(fs);

	 UnpackDllInMemory(buffer,bufsize);

	InitCrcTable(); 
	LocalFileCrc = Get_CRC(buffer, bufsize);
	VirtualFree((LPVOID)buffer,0,MEM_RELEASE);

		dwLocalFileSize = GetFileSize(lpModulePath);
		CheckFile = true;
	}


	char Console[100];
	char fOpenBack[MAX_PATH+2000];
	strcpy(fOpenBack,lpModulePath);
	lstrcat(fOpenBack,".tmp");

	file << "cURL        -> Initializing cURL..." << endl;
	SetProgressBarValue(ProgressBarMainValue,30);
	SetLabelText(Label1Main,"Preparing to Download DLL...");
RetryInit:
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(fOpenBack,"wb");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true); 
//		curl_easy_setopt(curl, CURLOPT_VERBOSE, true); 
        curl_easy_setopt(curl, CURLOPT_URL, DllParameter->DllLink);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_USERPWD, "xfileshack:op4n[]o1y(Jr{S%)D7");
		curl_easy_setopt(curl,  CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		
Retry:

        res = curl_easy_perform(curl);

		if(res == CURLE_ABORTED_BY_CALLBACK){
		
	SetProgressBarValue(ProgressBarMainValue,70);
	SetLabelText(Label1Main,"DLL File Already Up to Date, skipping.");
	Sleep(500);
	file << "UpdateAgent -> Local DLL File is Already Up to Date" << endl;
			goto Cleanup; // Skip saat di batalkan oleh Callback
		}
		if(res != CURLE_OK){
			sprintf(Console,"curl_easy_perform() Failed!. Code : %d\nRetry ?",res);
			int Msg = MessageBoxA(0,Console,"Error!",MB_YESNO + MB_ICONHAND);
			if(Msg == IDYES)goto Retry;
			ExitProcess(0);
		}
		while(!IsComplete)Sleep(100);
Cleanup:
        /* always cleanup */
	file << "cURL        -> Performing curl_easy_cleanup()" << endl;
        curl_easy_cleanup(curl);
        fclose(fp);
		Sleep(1000);

	}
	else {
			IsAborted = false;
			IsComplete = false;
			hCreate = false;
			bMeterTerminate = true;

			sprintf(Console,"curl_easy_init() Failed!. Code : %d\nRetry ?",curl);
			int Msg = MessageBoxA(0,Console,"Error!",MB_YESNO + MB_ICONHAND);
			if(Msg == IDYES)goto RetryInit;
			ExitProcess(0);
		}

	if(!CheckFile){
		SetProgressBarValue(ProgressBarMainValue,70);
	    SetLabelText(Label1Main,"Copying downloaded DLL File...");
		file << "UpdateAgent -> Copying Downloaded DLL File..." << endl;
		dwLocalFileSize = dwRemoteFileSize;
		bool nRet = CopyFile(fOpenBack,lpModulePath,false);
		if(!nRet){
			DWORD dwError = GetLastError();
			sprintf(Console,"Failed to Copy DLL File! Error : %d\nRetry?",dwError);
			int Msg = MessageBoxA(0,Console,"Error!",MB_YESNO + MB_ICONHAND);
			if(Msg == IDYES)goto RetryInit;
			ExitProcess(0);
		}
	}

	DeleteFile(fOpenBack);
	CheckThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ThreadAntiDllMalfunction,0,0,0);
	file << "UpdateAgent -> Operation Completed!";
	SetProgressBarValue(ProgressBarMainValue,100);
	SetLabelText(Label1Main,"Update Progress Completed!");
	Sleep(2000);
	file << endl << endl;
}