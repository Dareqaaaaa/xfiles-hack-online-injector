
#include "stdafx.h"
#include "InjectionCore.h"

//================================================================
int ProgressBarMainValue = 0;
int ProgressBarUpdateValue = 0;
char Label1Main[200] = "Harap Tunggu...";
char Label2Updater[200] = "Waiting for Information...";
bool ShowFormLogin = false;
bool ShowFormSelectGame = false;
bool ShowFormUpdate = false;
bool MainForm = true;
char LoginInformation[200] = {0};
bool LoggedIn = false;
bool GameSelected = false;
//=================================================================

#define SetLabelText(label,text) strcpy(label,text)
#define SetProgressBarValue(progress,value) progress = value;

#include "XorStr.h"
#include "SMART.h"
using namespace std;
#pragma warning (disable:4800 4244 4018 4101)
#pragma comment(lib, "Wininet.lib")
char Message[500];
#define BLUE 		9
#define GREEN 		10
#define SKYBLUE 	11
#define RED			12
#define PINK		13
#define YELLOW		14
#define WHITE		15
HANDLE  hConsole;
void SetConsoleColorText(int Color)
{
	if(!hConsole)hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, Color);
}

bool ForceUpdate = false;
#define InjectorVersion "01082015"

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream);
void KillGarenaProcess();
typedef struct _GAMELIST{
	char GameName[50];
	char GameCode[50];
} GAMELIST,*PGAMELIST;
typedef struct _MYPARAM {
    DWORD Checker;
	DWORD UserID;
	DWORD RefType;
	char Name[50];
	char RefName[50];
	char RefFBID[100];
	char RefFBName[100];
	char RefOpt[50];
	char RefInfo[100];
	char HDSN[100];
	char IP[20];
	DWORD State;
	char ExpTime[100];
	DWORD DurationHack;
	int NumberOfGame;
	GAMELIST *GameList;


} MYPARAM, *PMYPARAM;


typedef struct _DLLPARAM {

	char DllKey[20];
	char DllDecryptKey[150];
	char DllLink[500];
	double DllSize;

} DLLPARAM, *PDLLPARAM;
MYPARAM *MyParameter;
DLLPARAM *DllParameter;
ofstream file;
char InjectorApiKey[500] = {0};
char DllGameType[100] = {0};
char ServerMessage[1000] = {0};
bool bTerminate = false;
char* DeBlank(char* input);
string getInsideTags(string data,string start,string end);
void hexDump (char *desc, void *addr, int len,char *szLogBuff);
HANDLE CheckThread;

#include "UpdateAgent.h"
#include "NPSDK.h"
#include "InjectAgent.h"
#include "LiveUser.h"
#include "BugReporter.h"


TCHAR szWnd[400];

char ContactName [200];

char ContactNameStr[50] = {0};
char DLLName[50] = {0};


BOOL TerminateProcess(DWORD dwProcessId, UINT uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle  = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL)
        return FALSE;

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}

void KillGarenaProcess()
{
	DWORD GarenaMSG = GetPID("GarenaMessenger.exe");
   if(GarenaMSG)TerminateProcess(GarenaMSG,0);
      PROCESSENTRY32 pe32;
   HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
      //Tangkap beberapa proses yang sedang running
   hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
   if( hProcessSnap == INVALID_HANDLE_VALUE )return;

   pe32.dwSize = sizeof(PROCESSENTRY32);
   if( Process32First(hProcessSnap, &pe32) == 0 ) {
         CloseHandle( hProcessSnap ); 
         return;
   }
   do {//Loop untuk mencari apakah process yang akan diinject telah ditemukan ?
	    if(lstrcmp("ggdllhost.exe",pe32.szExeFile)== 0){
			TerminateProcess(pe32.th32ProcessID,255);
		}
   }while(Process32Next(hProcessSnap,&pe32) );
}

DWORD CheckMultipleRunning(){
   DWORD CurrentProcessId = GetCurrentProcessId();
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
	    if(lstrcmp("XFilesHack.exe",pe32.szExeFile)== 0){
			if(pe32.th32ProcessID != CurrentProcessId)TerminateProcess(pe32.th32ProcessID,255);
		}
   }while(Process32Next(hProcessSnap,&pe32) );

   DWORD PointBlankPID = GetPID("PointBlank.exe");
   DWORD HSUpdatePID = GetPID("HSUpdate.exe");
  
   if(PointBlankPID)TerminateProcess(PointBlankPID,0);
   if(HSUpdatePID)TerminateProcess(HSUpdatePID,0);
   

   CloseHandle( hProcessSnap );
   return (0);
}

DWORD CheckingOnServer()
{
MyParameter = new MYPARAM;
DllParameter = new DLLPARAM;
strcpy(ContactNameStr,"XFiles Hack");
SetLabelText(Label1Main,"Checking Registration...");
int	nRet = CheckValidSystemServer();
SetLabelText(Label1Main,"Registration Check Complete.");
if(MyParameter->RefType == 3){//OEM Reseller Type
	strcpy(ContactNameStr,MyParameter->RefOpt);
}
strcpy(ContactName,ContactNameStr);
TCHAR szMsg[1000];
sprintf(szWnd,"%s D3DMenu 2015",ContactName);
	switch(nRet)
	{
/*=================== Error Code =====================*/
		case NETSDK_ERROR_FAILED_HOSTNAME_LOOKUP:
			sprintf ( szMsg, "Terjadi masalah saat menyambungkan ke database server\nSilahkan periksa koneksi internet anda.\nJika masalah berkelanjutan, Silahkan hubungi Reseller masing-masing atau di Fan Page XFilesHack untuk Informasi Lebih Lanjut.");
			break;
		case NETSDK_ERROR_FAILED_HACK_DEACTIVATED:
			sprintf ( szMsg, "Mohon maaf, Cheat telah di matikan dari pusat server.\nPesan dari Server :\n\n%s\n\nSilahkan hubungi Reseller masing-masing atau di Fan Page XFilesHack untuk Informasi Lebih Lanjut.",ServerMessage);
			break;
		case NETSDK_ERROR_FAILED_NOT_REGISTERED:
			sprintf ( szMsg, "%s\n\nSerial Harddisk Anda : %s\nIP Anda %s\nTidak terdaftar dalam Server kami.\nSilahkan hubungi Reseller pada Fan Page XFilesHack untuk pembelian.",TimeOut,MyParameter->HDSN, MyParameter->IP);
			break;
		case NETSDK_ERROR_FAILED_INJECTOR_OUTDATED:
			sprintf ( szMsg, "Injector yang anda gunakan telah kadaluarsa.\nSilahkan hubungi Reseller masing-masing untuk Mendapatkan Injector Terbaru.");
			break;
		//========= Error HWID ===========//
		case NETSDK_ERROR_FAILED_HWID_DISABLED:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nTelah di matikan di Server kami.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_HWID_BANNED:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nTelah di Banned di Server kami.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_HWID_PAYMENTPROBLEM:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nDurasi anda telah habis.\nSilahkan hubungi %s untuk menambah Durasi HWID" ,MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
		//========= Error IP ===========//
		case NETSDK_ERROR_FAILED_WARNET_DISABLED:
			sprintf ( szMsg, "Nama Warnet : %s\nReseller Pendaftar : %s\nIP Anda : %s\nDurasi Cheat Anda Sampai : %s\nTelah di matikan di Server kami.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->IP,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_WARNET_BANNED:
			sprintf ( szMsg, "Nama Warnet : %s\nReseller Pendaftar : %s\nIP Anda : %s\nDurasi Cheat Anda Sampai : %s\nTelah di Banned di Server kami.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->IP,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_WARNET_PAYMENTPROBLEM:
			sprintf ( szMsg, "Nama Warnet : %s\nReseller Pendaftar : %s\nIP Anda : %s\nDurasi Cheat Anda Sampai : %s\nDurasi anda telah habis.\nSilahkan hubungi %s untuk menambah Durasi Warnet anda" ,MyParameter->Name,MyParameter->RefName,MyParameter->IP,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_WARNET_FULLSLOT:
			sprintf ( szMsg, "Nama Warnet : %s\nReseller Pendaftar : %s\nIP Anda : %s\nDurasi Cheat Anda Sampai : %s\nPC ini tidak di setujui server karena anda telah melewati batas client yang terdaftar.\nSilahkan hubungi %s untuk menambah Jumlah PC Warnet" ,MyParameter->Name,MyParameter->RefName,MyParameter->IP,MyParameter->ExpTime,MyParameter->RefName);
			break;
		//========= Error MEMBER ===========//
		case NETSDK_ERROR_FAILED_MEMBER_DISABLED:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nTelah di matikan di Server kami.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_MEMBER_BANNED:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nTelah di Banned di Server kami.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_MEMBER_PAYMENTPROBLEM:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nDurasi anda telah habis.\nSilahkan hubungi %s untuk menambah Durasi Member Anda" ,MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
		case NETSDK_ERROR_FAILED_MEMBER_INUSE:
			sprintf ( szMsg, "Username Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nDurasi Cheat Anda Sampai : %s\nAkun ini sedang digunakan. Silahkan coba beberapa saat lagi.\nInformasi lebih lanjut silahkan hubungi %s",MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,MyParameter->ExpTime,MyParameter->RefName);
			break;
/*=================== Success Code =====================*/
		case NETSDK_SUCCESS_VALID_HWID:
			sprintf ( szMsg, "%s\n\nUsername Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nTelah terdaftar di Server kami.\nDurasi Cheat Anda Sampai : %s\nTerima kasih telah membeli produk kami ^^",TimeOut,MyParameter->Name,MyParameter->RefName,MyParameter->HDSN, MyParameter->ExpTime);
			break;
		case NETSDK_SUCCESS_VALID_WARNET:
			sprintf ( szMsg, "%s\n\nNama Warnet : %s\nReseller Pendaftar : %s\nIP Anda : %s\nTelah terdaftar di Server kami.\nDurasi Cheat Anda Sampai : %s\nTerima kasih telah membeli produk kami ^^",TimeOut,MyParameter->Name,MyParameter->RefName,MyParameter->IP, MyParameter->ExpTime);
			break;
		case NETSDK_SUCCESS_VALID_MEMBER:
			sprintf ( szMsg, "%s\n\nUsername Pengguna : %s\nReseller Pendaftar : %s\nSerial Harddisk Anda : %s\nTelah terdaftar di Server kami.\nDurasi Cheat Anda Sampai : %s\nTerima kasih telah membeli produk kami ^^",TimeOut,MyParameter->Name,MyParameter->RefName,MyParameter->HDSN, MyParameter->ExpTime);
			break;

	}

MyParameter->State = nRet;


MessageBoxA(NULL, szMsg,szWnd,MB_OK + MB_ICONINFORMATION + MB_TOPMOST);
return  nRet;

}

DWORD WINAPI MainThread(LPVOID param)
{

	

CheckMultipleRunning();
ZeroMemory(&MyParameter,sizeof(MyParameter));
file.open("C:\\XFilesInjectorOutput.log"); 

SetConsoleTitle("X-Files Auto Update Injector V.1.1");
//FreeConsole();
SetConsoleColorText(RED);
file << "XFiles Auto Update Injector by FahmyXFiles" << endl << endl;
SetConsoleColorText(YELLOW);
file << "NPSDK -> NProtectSDK V.2.6 r22 by X-Files" << endl;
file << "NPSDK -> Checking Registration Information..." << endl;

DWORD Code = CheckingOnServer();

	if(Code == NETSDK_SUCCESS_VALID_HWID			 ||
	   Code == NETSDK_SUCCESS_VALID_WARNET			 ||
	   Code == NETSDK_SUCCESS_VALID_MEMBER             ){ 
		   	

file << "NPSDK -> CheckingOnServer() : Success. Your Computer Is Registered." << endl << endl;


//XFilesInject_UpdateDLL("XFilesHack.dll");

bool nRet =	InjectDLL("PointBlank.exe","XFilesHack.dll"); // berikan suntikan ke PB :D

if(nRet == true){
	MessageBoxA(0,Message,"XFiles Injector Error!",MB_OK + MB_ICONHAND);
	ExitProcess(-1);
}

	}	
	ExitProcess(0);
	return 0;
}

//============== Export Library ============//
int GetProgressBarMainValue() {
    return ProgressBarMainValue;
}
int GetProgressBarUpdateValue() {
    return ProgressBarUpdateValue;
}
char *GetLabel1MainText(){
	return Label1Main;
}
char *GetLabel2UpdaterText(){
	return Label2Updater;
}

bool GetFormLoginState(){
	return ShowFormLogin;
}
bool GetFormSelectGameState(){
	return ShowFormSelectGame;
}
bool GetFormUpdateState(){
	return ShowFormUpdate;
}
bool GetFormMainState(){
	return MainForm;
}

void SetFormLoginState(bool value){
	ShowFormLogin = value;
}
void SetFormSelectGameState(bool value){
	ShowFormSelectGame = value;
}
void SetFormUpdateState(bool value){
	ShowFormUpdate = value;
}
void SetFormMainState(bool value){
	MainForm = value;
}

void SetMemberLoginInformation (char* text){
	strcpy(LoginInformation,text);
}
void SetLoggedInState(bool value){
	LoggedIn = value;
}
void SetGameSelectedState(bool value){
	GameSelected = value;
}

int GetNumberOfGames()
{
	return MyParameter->NumberOfGame;
}

char *GetGameNamebyIdx(int i){
	return MyParameter->GameList[i].GameName;
}

char *GetGameCodebyIdx(int i){
	return MyParameter->GameList[i].GameCode;
}

void SetGameCode(char *GameCode){
	strcpy(DllGameType,GameCode);
}

void InitializeInjectionCore (char* api_key){
	strcpy(InjectorApiKey,api_key);
	CreateThread(0,0,(LPTHREAD_START_ROUTINE) MainThread,0,0,0);
}