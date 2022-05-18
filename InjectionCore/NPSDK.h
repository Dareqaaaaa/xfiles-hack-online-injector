#define CountToMinutes(min)   min*60000


char TimeOut[500];


DWORD NSDK_State = 0;
//=================== Define Return Address =====================//
#define NETSDK_ERROR_FAILED_HOSTNAME_LOOKUP				0x94000101
#define NETSDK_ERROR_FAILED_HACK_DEACTIVATED			0x94000102
#define NETSDK_ERROR_FAILED_NOT_REGISTERED				0x94000103
#define NETSDK_ERROR_FAILED_INJECTOR_OUTDATED			0x94000104
//======================== HDSN Code ===========================//
#define NETSDK_ERROR_FAILED_HWID_DISABLED				0x94000201
#define NETSDK_ERROR_FAILED_HWID_BANNED					0x94000202
#define NETSDK_ERROR_FAILED_HWID_PAYMENTPROBLEM			0x94000203
//======================== WARNET Code =========================//
#define NETSDK_ERROR_FAILED_WARNET_DISABLED				0x94000204
#define NETSDK_ERROR_FAILED_WARNET_BANNED				0x94000205
#define NETSDK_ERROR_FAILED_WARNET_PAYMENTPROBLEM		0x94000206
#define NETSDK_ERROR_FAILED_WARNET_FULLSLOT				0x94000207
//======================== MEMBER Code ===========================//
#define NETSDK_ERROR_FAILED_MEMBER_DISABLED				0x94000208
#define NETSDK_ERROR_FAILED_MEMBER_BANNED				0x94000209
#define NETSDK_ERROR_FAILED_MEMBER_PAYMENTPROBLEM		0x9400020A
#define NETSDK_ERROR_FAILED_MEMBER_INUSE				0x9400020B
//======================== Success Code =========================//
#define NETSDK_SUCCESS_VALID_HWID						0x94000301
#define NETSDK_SUCCESS_VALID_WARNET						0x94000302
#define NETSDK_SUCCESS_VALID_MEMBER						0x94000303
//======================== BYTE REG Status =====================//
#define NETSDK_REGSTATUS_ENABLED						0x01
#define NETSDK_REGSTATUS_DISABLED						0x02
#define NETSDK_REGSTATUS_BANNED							0x03
#define NETSDK_REGSTATUS_PAYMENTPROBLEM					0x04
#define NETSDK_REGSTATUS_WARNET_REACHED_MAX_PC			0x05
#define NETSDK_REGSTATUS_MEMBER_INUSE					0x06
//==============================================================//
int CheckValidSystemServer(void);


DiskInfo MyDisk = NULL;



const char * Hari[] = { "Minggu", "Senin","Selasa", "Rabu","Kamis", "Jum'at", "Sabtu"};
const char * Bulan[] = { "Januari", "Februari","Maret", "April","Mei", "Juni", "Juli", "Agustus", "September", "Oktober" , "November", "Desember"};

SYSTEMTIME CurrentDate;
SYSTEMTIME ExpiredDate;
SYSTEMTIME TrialTime;




int CharToInt(char *input)
{
return atoi(input);
}
void ParseInternetTime(const char* Output, SYSTEMTIME &Time)
{

	for(int i = 0; i<=strlen(Output); i++)
{
	if(i == 0){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(Output + i);
		Time.wDay = CharToInt(streamCopy);
	}
	if(i == 3){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(Output + i);
		Time.wMonth = CharToInt(streamCopy);
	}
	if(i == 6){
		char streamCopy[5] = {0};
		*(DWORD*)(streamCopy) = *(DWORD*)(Output + i);
		Time.wYear = CharToInt(streamCopy);
	}
	if(i == 11){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(Output + i);
		Time.wHour = CharToInt(streamCopy);
	}
	if(i == 14){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(Output + i);
		Time.wMinute = CharToInt(streamCopy);
	}
	if(i == 17){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(Output + i);
		Time.wSecond = CharToInt(streamCopy);
	}
}


}


void GetExpiredLicenseDate(SYSTEMTIME TimeIn, SYSTEMTIME &TimeOut, WORD Duration)
{
	int y = TimeIn.wYear;
	int m = TimeIn.wMonth;
	int d = TimeIn.wDay;

    // Represent the date as struct tm.                                                           
    // The subtractions are necessary for historical reasons.
    struct tm  t = { 0 };
    t.tm_mday = d;
    t.tm_mon = m-1;
    t.tm_year = y-1900;

    // Add 'skip' days to the date.                                                               
    t.tm_mday += Duration;
    mktime(&t);
	TimeOut.wDay = t.tm_mday;
	TimeOut.wMonth = t.tm_mon + 1;
	TimeOut.wYear = t.tm_year + 1900;
}
ofstream pWriteFile;
void WriteStringToFile(char *Target,char *String)
{
pWriteFile.open(Target);
pWriteFile << String;
pWriteFile.close();
}


char* DeBlank(char* input)                                         
{
    int i,j;
    char *output=input;
    for (i = 0, j = 0; i<strlen(input); i++,j++)          
    {
        if (input[i]!=' ')                           
            output[j]=input[i];                     
        else
            j--;                                     
    }
    output[j]=0;
    return output;
}

char StrCurrentTime[500];
void SendUserData(char *lpSerial)
{
	char PostURL[5000] = {0};
	char ComputerName[100] = {0};
	DWORD cbComputerName = sizeof ( ComputerName );
	GetComputerNameA( ComputerName, &cbComputerName);

	sprintf(PostURL,"http://www.xfileshack.net/XFilesHack/ClientLogs/Notify.php?pcname=%s&pchwid=%s&injectversion=%s",ComputerName,lpSerial,InjectorVersion);

	CURL *curl;
	string myTIME;
	CURLcode res;
	string Result;
	curl = curl_easy_init();
    if (curl) {

	curl_easy_setopt(curl, CURLOPT_URL, DeBlank(PostURL));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Result);
	curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT,true);

    res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	}
}
string getInsideTags(string data,string start,string end)
{
		unsigned first = data.find(start) + start.length();
		unsigned last = data.find(end,first)-end.length();
		return data.substr (first,last-first+1);
}
char * CleanLines(const char* input)
{
	char *buffer = new char[100];
	strcpy(buffer,input);
	for(int i = 0; i<strlen(buffer); i++)
	{
		if(buffer[i] == '\\'){
			buffer[i] = '\n';
		}
	}
	return buffer;
}
int GetInformation(char *data, char *type,SYSTEMTIME &Time)
{
	char PostURL[5000] = {0};
	int Ret = -1;
	SYSTEMTIME AuthTime;
	DWORD AuthKey = 0;
	GetSystemTime(&AuthTime);
	AuthKey = AuthTime.wMilliseconds * AuthTime.wSecond * AuthTime.wMinute * 16;

	if(AuthKey == 0)AuthKey = 15*534;

	sprintf(PostURL,"http://www.xfileshack.net/XFilesHack/getinfo.php?data=%s&type=%s&auth=%d",data,type,AuthKey);

	CURL *curl;
	CURLcode res;
	char Input[500] = {0};
	char UserAgent[100] = {0};
	sprintf(UserAgent,"%d",AuthKey);
	curl = curl_easy_init();
    if (curl) {

	curl_easy_setopt(curl, CURLOPT_URL, DeBlank(PostURL));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_char);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Input);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent);
	curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT,true);

    res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	}
	char key[15] = {0};
	char Output[500] = {0};
	sprintf(key,"%d",AuthKey * 4);
	for(int i = 0; i < ReceivedBytes; i++)
	{
		Output[i] = Input[i] ^ key[i % strlen(key)];
	}
	string Data;
	stringstream pData;
	string line;
	pData << Output;
	getline(pData,line);
	if(!strcmp(line.c_str(),"MAINTENANCE")){
		getline(pData,line);
		strcpy(ServerMessage,line.c_str());
		return 10;
	}
	if(!strcmp(line.c_str(),"VALID")){
		
		getline(pData,line);
		if(strcmp(line.c_str(),InjectorVersion))return 2;
		
		getline(pData,line);
		strcpy(MyParameter->IP,line.c_str());

		getline(pData,line);
		ParseInternetTime(line.c_str(),CurrentDate);

		getline(pData,line);
		if(!strcmp(line.c_str(),"true"))ForceUpdate = true;
		return 1;
	}
	if(!strcmp(line.c_str(),"INVALID"))return 2;
	else if(!strcmp(line.c_str(),"NOTFOUND"))return -2;
	else if(!strcmp(line.c_str(),"SUCCESS")){
		getline(pData,line);
		MyParameter->UserID = atoi(line.c_str());
		getline(pData,line); // nama
		strcpy(MyParameter->Name,line.c_str());
		getline(pData,line); // resellername
		strcpy(MyParameter->RefName,line.c_str());
		getline(pData,line); // resellertype
		MyParameter->RefType = atoi(line.c_str());
		getline(pData,line); // reselleropt
		strcpy(MyParameter->RefOpt,getInsideTags(line,"(",")").c_str());
		getline(pData,line); // resellerinfo
		strcpy(MyParameter->RefInfo,CleanLines(getInsideTags(line,"[","]").c_str()));
		getline(pData,line); // resellerifbid
		strcpy(MyParameter->RefFBID,line.c_str());
		getline(pData,line); // resellerfbname
		strcpy(MyParameter->RefFBName,line.c_str());

		getline(pData,line);
		strcpy(MyParameter->HDSN,line.c_str());

		getline(pData,line);
		Ret = atoi(line.c_str());
		getline(pData,line);
		MyParameter->DurationHack = atoi(line.c_str());

		char ExpDate[50];
		getline(pData,line);
		strcpy(ExpDate,line.c_str());

		for(int i = 0; i<=strlen(ExpDate); i++){
	if(i == 0){
		char streamCopy[5] = {0};
		*(DWORD*)(streamCopy) = *(DWORD*)(ExpDate + i);
		Time.wYear = CharToInt(streamCopy);
	}
	if(i == 5){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(ExpDate + i);
		Time.wMonth = CharToInt(streamCopy);
	}
	if(i == 8){
		char streamCopy[5] = {0};
		*(WORD*)(streamCopy) = *(WORD*)(ExpDate + i);
		Time.wDay = CharToInt(streamCopy);
	}
		}

		getline(pData,line);
		strcpy(DllParameter->DllKey,line.c_str());
		getline(pData,line);
		MyParameter->NumberOfGame = atoi(line.c_str());
		MyParameter->GameList = new GAMELIST[MyParameter->NumberOfGame];
		for(int i = 0; i < MyParameter->NumberOfGame; i++){
			getline(pData,line);
			strcpy(MyParameter->GameList[i].GameName,line.c_str());
			getline(pData,line);
			strcpy(MyParameter->GameList[i].GameCode,line.c_str());
		}
	}
	else {
		file << "Received unrecognized response from server : " << endl << Input << endl;
	}
	return Ret;
}


struct tm  timeinfo = { 0 };
struct tm  expiredcount = { 0 };
char DateTime[30] = {0};


int CheckValidSystemServer()
{
	char *ExpiredTimeStr = new char[500];
	char *lpSerial = new char[50];
	SetProgressBarValue(ProgressBarMainValue,0);
	SetLabelText(Label1Main,"Connecting to Server...");
	SYSTEMTIME ExpiredTime = {0};
	Sleep(500);
	SetProgressBarValue(ProgressBarMainValue,15);
	SetLabelText(Label1Main,"Verifying Registration Information");
	int ServerResult = GetInformation(InjectorVersion,"server",ExpiredTime);
	if(ServerResult != 1){
		SetProgressBarValue(ProgressBarMainValue,0);
	    SetLabelText(Label1Main,"Cannot verify Injector Version!");
		if(ServerResult == 2)return NETSDK_ERROR_FAILED_INJECTOR_OUTDATED;
		return NETSDK_ERROR_FAILED_HOSTNAME_LOOKUP;
	}
	SetProgressBarValue(ProgressBarMainValue,25);
	Sleep(1000);
	timeinfo.tm_mday = CurrentDate.wDay;
	timeinfo.tm_mon  = CurrentDate.wMonth - 1;
	timeinfo.tm_year = CurrentDate.wYear - 1900;
	timeinfo.tm_hour = CurrentDate.wHour;
	timeinfo.tm_min  = CurrentDate.wMinute;
	timeinfo.tm_sec  = CurrentDate.wSecond;


	mktime(&timeinfo);
	
	sprintf(StrCurrentTime,"%s, %d %s %d %d:%d",Hari[timeinfo.tm_wday],timeinfo.tm_mday,Bulan[timeinfo.tm_mon],timeinfo.tm_year+1900,timeinfo.tm_hour,timeinfo.tm_min);
	sprintf(TimeOut,"Waktu Server (Indonesia/Jakarta) GMT + 07 : \n%s",StrCurrentTime);

	file << TimeOut << endl;

	
	MyDisk.LoadDiskInfo();
	strcpy(lpSerial,DeBlank(MyDisk.SerialNumber()));
	strcpy(MyParameter->HDSN,lpSerial);
	SetProgressBarValue(ProgressBarMainValue,30);

	SendUserData(lpSerial);

	SetProgressBarValue(ProgressBarMainValue,35);
	
	int ResultHWID = GetInformation(lpSerial,"hwid",ExpiredTime);
	if(ResultHWID == 10)return NETSDK_ERROR_FAILED_HACK_DEACTIVATED;
	if(ResultHWID == -1)return NETSDK_ERROR_FAILED_HOSTNAME_LOOKUP;
	if(ResultHWID != -2){
		if(strcmp(lpSerial,MyParameter->HDSN))return NETSDK_ERROR_FAILED_NOT_REGISTERED;
		SetProgressBarValue(ProgressBarMainValue,100);
		struct tm  expinfo = { 0 };
		expinfo.tm_mday = ExpiredTime.wDay;
		expinfo.tm_mon  = ExpiredTime.wMonth - 1;
		expinfo.tm_year = ExpiredTime.wYear - 1900;
		mktime(&expinfo);
		sprintf(MyParameter->ExpTime,"%s, %d %s %d.",Hari[expinfo.tm_wday],expinfo.tm_mday,Bulan[expinfo.tm_mon],expinfo.tm_year+1900);

		if(ResultHWID == NETSDK_REGSTATUS_ENABLED)return NETSDK_SUCCESS_VALID_HWID; 
		if(ResultHWID == NETSDK_REGSTATUS_DISABLED)return NETSDK_ERROR_FAILED_HWID_DISABLED;
		if(ResultHWID == NETSDK_REGSTATUS_BANNED)  return NETSDK_ERROR_FAILED_HWID_BANNED;
		if(ResultHWID == NETSDK_REGSTATUS_PAYMENTPROBLEM)return NETSDK_ERROR_FAILED_HWID_PAYMENTPROBLEM;
	}

	SetProgressBarValue(ProgressBarMainValue,50);
	int ResultWARNET = GetInformation(lpSerial,"clientwarnet",ExpiredTime);
	if(ResultWARNET == -1)return NETSDK_ERROR_FAILED_HOSTNAME_LOOKUP;
	if(ResultWARNET != -2){
		if(strcmp(lpSerial,MyParameter->HDSN))return NETSDK_ERROR_FAILED_NOT_REGISTERED;
		SetProgressBarValue(ProgressBarMainValue,100);
		struct tm  expinfo = { 0 };
		expinfo.tm_mday = ExpiredTime.wDay;
		expinfo.tm_mon  = ExpiredTime.wMonth - 1;
		expinfo.tm_year = ExpiredTime.wYear - 1900;
		mktime(&expinfo);
		sprintf(MyParameter->ExpTime,"%s, %d %s %d.",Hari[expinfo.tm_wday],expinfo.tm_mday,Bulan[expinfo.tm_mon],expinfo.tm_year+1900);

		if(ResultWARNET == NETSDK_REGSTATUS_ENABLED)return NETSDK_SUCCESS_VALID_WARNET;
		if(ResultWARNET == NETSDK_REGSTATUS_DISABLED)return NETSDK_ERROR_FAILED_WARNET_DISABLED;
		if(ResultWARNET == NETSDK_REGSTATUS_BANNED)  return NETSDK_ERROR_FAILED_WARNET_BANNED;
		if(ResultWARNET == NETSDK_REGSTATUS_PAYMENTPROBLEM)return NETSDK_ERROR_FAILED_WARNET_PAYMENTPROBLEM;
		if(ResultWARNET == NETSDK_REGSTATUS_WARNET_REACHED_MAX_PC)return NETSDK_ERROR_FAILED_WARNET_FULLSLOT;
	}
	
	
	SetProgressBarValue(ProgressBarMainValue,75);
	SetLabelText(Label1Main,"Silahkan Login dengan Username dan Password jika anda seorang member.");
	ShowFormLogin = true;
	while(!LoggedIn)Sleep(50);
	char LoginData[500];
	sprintf(LoginData,"%s&%s",lpSerial,LoginInformation);

	SetProgressBarValue(ProgressBarMainValue,80);
	SetLabelText(Label1Main,"Verifying Member Data...");

	int ResultMEMBER = GetInformation(LoginData,"member",ExpiredTime);
	if(ResultMEMBER == 10)return NETSDK_ERROR_FAILED_HACK_DEACTIVATED;
	if(ResultMEMBER == -1)return NETSDK_ERROR_FAILED_HOSTNAME_LOOKUP;
	if(ResultMEMBER != -2){
		if(strcmp(lpSerial,MyParameter->HDSN))return NETSDK_ERROR_FAILED_NOT_REGISTERED;
		SetProgressBarValue(ProgressBarMainValue,100);
		strcpy(MyParameter->HDSN,lpSerial);

		struct tm  expinfo = { 0 };
		expinfo.tm_mday = ExpiredTime.wDay;
		expinfo.tm_mon  = ExpiredTime.wMonth - 1;
		expinfo.tm_year = ExpiredTime.wYear - 1900;
		mktime(&expinfo);
		sprintf(MyParameter->ExpTime,"%s, %d %s %d.",Hari[expinfo.tm_wday],expinfo.tm_mday,Bulan[expinfo.tm_mon],expinfo.tm_year+1900);

		if(ResultMEMBER == NETSDK_REGSTATUS_ENABLED)return NETSDK_SUCCESS_VALID_MEMBER; 
		if(ResultMEMBER == NETSDK_REGSTATUS_DISABLED)return NETSDK_ERROR_FAILED_MEMBER_DISABLED;
		if(ResultMEMBER == NETSDK_REGSTATUS_BANNED)  return NETSDK_ERROR_FAILED_MEMBER_BANNED;
		if(ResultMEMBER == NETSDK_REGSTATUS_PAYMENTPROBLEM)return NETSDK_ERROR_FAILED_MEMBER_PAYMENTPROBLEM;
		if(ResultMEMBER == NETSDK_REGSTATUS_MEMBER_INUSE)return NETSDK_ERROR_FAILED_MEMBER_INUSE;
	}
	SetProgressBarValue(ProgressBarMainValue,100);
	return NETSDK_ERROR_FAILED_NOT_REGISTERED;
}


