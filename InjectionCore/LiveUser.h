void GenerateRandomString(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}
/*
Logging channel type.
0 = Event Server
1 = Public Server
2 = Challenge Server
3 = Challenge Server
4 = GWarnet Clan Server
5 = Expert Server
6 = Expert Server
7 = Competition Server
8 = Public Server
9 = Clan Server

ServerCommand.
0 = No Action
1 = Safe Disconnect
2 = Force Disconnect
3 = Show message
*/

typedef struct _XFILESUSERINFOPARAM {
	char Nickname[20];
	char Reseller[50];
	char HDSN[50];
	

} XFILESUSERINFOPARAM, *PXFILESUSERINFOPARAM;
typedef struct _GAMEINFOPARAM {
	char Nickname[20];
	char ServerInfo[50];
	char RoomInfo[50];
	bool IsPlaying;
	char ServerMessage[500];
	DWORD ServerCommand;
	int SizeOfAvailUserInfo;
	XFILESUSERINFOPARAM XFilesUserInfo[15];

} GAMEINFOPARAM, *PGAMEINFOPARAM;

int SetLiveUserInformation(char *Nickname, char *ServerInfo, char *RoomInfo, bool IsPlaying)
{
	char PostURL[5000] = {0};
	int Ret = -1;
	SYSTEMTIME AuthTime;
	DWORD AuthKey = 0;
	GetSystemTime(&AuthTime);
	AuthKey = AuthTime.wMilliseconds * AuthTime.wSecond * AuthTime.wMinute * 16;

	if(AuthKey == 0)AuthKey = 15*534;

	sprintf(PostURL,"http://www.xfileshack.net/XFilesHack/LiveUser.php?type=set&username=%s&reseller=%s&hdsn=%s&nickname=%s&serverinfo=%s&roominfo=%s&isplaying=%d&gametype=%s&auth=%d",MyParameter->Name,MyParameter->RefName,MyParameter->HDSN,Nickname,ServerInfo,RoomInfo,IsPlaying,DllGameType,AuthKey);
	CURL *curl;
	CURLcode res;
	char Input[500] = {0};
	char UserAgent[100] = {0};
	sprintf(UserAgent,"%d",AuthKey);
	curl = curl_easy_init();
    if (curl) {

	curl_easy_setopt(curl, CURLOPT_URL, PostURL);
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
	Output[ReceivedBytes] = '\0';
	string Data;
	stringstream pData;
	string line;
	pData << Output;
	getline(pData,line);
	if(!strcmp(line.c_str(),"SUCCESS"))return 1;
	else return 0;
}


int GetLiveUserInformation(GAMEINFOPARAM* GameInfo)
{
	char PostURL[5000] = {0};
	int Ret = -1;
	SYSTEMTIME AuthTime;
	DWORD AuthKey = 0;
	GetSystemTime(&AuthTime);
	AuthKey = AuthTime.wMilliseconds * AuthTime.wSecond * AuthTime.wMinute * 16;

	if(AuthKey == 0)AuthKey = 15*534;

	sprintf(PostURL,"http://www.xfileshack.net/XFilesHack/LiveUser.php?type=get&hdsn=%s&auth=%d",MyParameter->HDSN,AuthKey);

	CURL *curl;
	CURLcode res;
	char Input[500] = {0};
	char UserAgent[100] = {0};
	sprintf(UserAgent,"%d",AuthKey);
	curl = curl_easy_init();
    if (curl) {

	curl_easy_setopt(curl, CURLOPT_URL, PostURL);
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
	Output[ReceivedBytes] = '\0';
	string Data;
	stringstream pData;
	string line;
	pData << Output;
	getline(pData,line);
	if(!strcmp(line.c_str(),"SUCCESS")){
		getline(pData,line);
		GameInfo->ServerCommand = atoi(line.c_str());
		getline(pData,line);
		strcpy(GameInfo->ServerMessage,line.c_str());
		getline(pData,line);
		GameInfo->SizeOfAvailUserInfo = atoi(line.c_str());
		if(GameInfo->SizeOfAvailUserInfo > 0){
		for(int i = 0; i < GameInfo->SizeOfAvailUserInfo; i++){
			getline(pData,line);
			strcpy(GameInfo->XFilesUserInfo[i].Nickname,line.c_str());
			getline(pData,line);
			strcpy(GameInfo->XFilesUserInfo[i].Reseller,line.c_str());
			getline(pData,line);
			strcpy(GameInfo->XFilesUserInfo[i].HDSN,line.c_str());
		}
		}
		return 1;
	}
	 return 0;
}

DWORD WINAPI LiveUserThread_SetGameInfo (LPVOID param)
{

	char GameInfoBuffer[5000] = {0};
	char Last_Nickname[20] = {0};
	char Last_ServerInfo[50] = {0};
	char Last_RoomInfo[50] = {0}; 
	bool Last_IsPlaying = false;
	DWORD Delay_Response = 0;

	GAMEINFOPARAM *GameInfo = (GAMEINFOPARAM*)GameInfoBuffer;

	HANDLE pipe = CreateNamedPipeA(
        "\\\\.\\pipe\\LiveUser_SetGameInfo", // name of the pipe
        PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
        PIPE_TYPE_BYTE, // send data as a byte stream
        1, 
        5000, // no outbound buffer
        5000, // no inbound buffer
        NMPWAIT_USE_DEFAULT_WAIT, // use default wait time
        NULL // use default security attributes
		);

	    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {\
		DWORD LastError = GetLastError();
        file << "Failed to create outbound pipe instance. " << LastError;
        bTerminate = true;
    }

	BOOL bClientConnected = ConnectNamedPipe(pipe, NULL);

	Sleep(1000);
	while(true){

		DWORD numBytesRead = 0;
		BOOL result = ReadFile(
        pipe,
        GameInfoBuffer, // the data from the pipe will be put here
        5000, // number of bytes allocated
        &numBytesRead, // this will store number of bytes actually read
        NULL // not using overlapped IO
		);
		bool s1 = strcmp(Last_Nickname,DeBlank(GameInfo->Nickname));
		bool s2 = strcmp(Last_ServerInfo,DeBlank(GameInfo->ServerInfo));
		bool s3 = strcmp(Last_RoomInfo,DeBlank(GameInfo->RoomInfo));
		bool s4 = (Last_IsPlaying == GameInfo->IsPlaying);
		bool s5 = (Delay_Response < 280);
		if(!s1 && !s2 && !s3 && s4 && s5){ 
			Delay_Response+=1;
		}
		else {

		Delay_Response = 0;
		SetLiveUserInformation(DeBlank(GameInfo->Nickname),DeBlank(GameInfo->ServerInfo),DeBlank(GameInfo->RoomInfo),GameInfo->IsPlaying);
		}

		strcpy(Last_Nickname,DeBlank(GameInfo->Nickname));
		strcpy(Last_ServerInfo,DeBlank(GameInfo->ServerInfo));
		strcpy(Last_RoomInfo,DeBlank(GameInfo->RoomInfo));
		Last_IsPlaying = GameInfo->IsPlaying;


		Sleep(1000);
	}
	return 0;
}


DWORD WINAPI LiveUserThread_GetGameInfo (LPVOID param)
{
	char GameInfoBuffer[5000] = {0};
	GAMEINFOPARAM *GameInfo = (GAMEINFOPARAM*)GameInfoBuffer;
	HANDLE pipe = CreateNamedPipeA(
        "\\\\.\\pipe\\LiveUser_GetGameInfo", // name of the pipe
        PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
        PIPE_TYPE_BYTE, // send data as a byte stream
        1, 
        5000, // no outbound buffer
        5000, // no inbound buffer
        NMPWAIT_USE_DEFAULT_WAIT, // use default wait time
        NULL // use default security attributes
		);

	    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {\
		DWORD LastError = GetLastError();
        file << "Failed to create outbound pipe instance. " << LastError;
        bTerminate = true;
    }
		
	BOOL bClientConnected = ConnectNamedPipe(pipe, NULL);

	while(true){
		DWORD numBytesWritten = 0;

		GetLiveUserInformation(GameInfo);
		BOOL result = WriteFile(
        pipe, // handle to our outbound pipe
        GameInfoBuffer, // data to send
        5000, // length of data to send (bytes)
        &numBytesWritten, // will store actual amount of data sent
        NULL // not using overlapped IO
		);
		Sleep(60000);
	}
	return 0;
}
BOOL bConnected = false;
DWORD WINAPI LiveUserThread_TimeoutTimer (LPVOID param)
{
	int TimeoutCounter = 0;
	while(true){

		if(bConnected)TerminateThread(GetCurrentThread(),0);
		if(TimeoutCounter > 600){ // 10 min elapsed and still not connected
			file << "Timeout while Connecting Client Process. Exiting..." << endl;
			bTerminate = true;
			TerminateThread(GetCurrentThread(),0);
		}
		
		TimeoutCounter+=1;
		Sleep(1000);
	}
	return 0;
}

DWORD WINAPI LiveUserThread_CheckRunning (LPVOID param)
{

	HANDLE pipe = CreateNamedPipeA(
        "\\\\.\\pipe\\LiveUser_CheckRunningClient", // name of the pipe
        PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
        PIPE_TYPE_BYTE, // send data as a byte stream
        1, 
        1, // no outbound buffer
        1, // no inbound buffer
        NMPWAIT_WAIT_FOREVER, // use default wait time
        NULL // use default security attributes
		);

	    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {\
		DWORD LastError = GetLastError();
        file << "Failed to create outbound pipe instance. " << LastError;
        bTerminate = true;
    }

		CreateThread(0,0,(LPTHREAD_START_ROUTINE)LiveUserThread_TimeoutTimer,0,0,0);
	BOOL bClientConnected = ConnectNamedPipe(pipe, NULL);

	bConnected = true;
	Sleep(1000);
		HANDLE Openpipe = CreateFile(
        "\\\\.\\pipe\\LiveUser_CheckRunningHost",
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
	char DummyBufferHost[1];
	char DummyBufferClient[1];
	while(true)
	{
		DWORD numBytesRead = 0;
		DWORD numBytesWritten = 0;

		BOOL result = ReadFile(
        pipe,
        DummyBufferClient, // the data from the pipe will be put here
        1, // number of bytes allocated
        &numBytesRead, // this will store number of bytes actually read
        NULL // not using overlapped IO
		);
		if(!result){
			file << "Lost Connection with Client Process. Exiting...(R)" << endl;
			bTerminate = true;
			TerminateThread(GetCurrentThread(),0);
		}
		result = WriteFile(
        Openpipe,
        DummyBufferHost, // the data from the pipe will be put here
        1, // number of bytes allocated
        &numBytesWritten, // this will store number of bytes actually read
        NULL // not using overlapped IO
		);
		if(!result){
			file << "Lost Connection with Client Process. Exiting...(W)" << endl;
			bTerminate = true;
			TerminateThread(GetCurrentThread(),0);
		}
		Sleep(1000);
	}
}