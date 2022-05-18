HINTERNET ConnectInternet()
{
	return InternetOpen("Some USER-AGENT",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
}
bool DisconnectInternet(HINTERNET Internet)
{
	if(InternetCloseHandle(Internet))return true;
	return false;
}
HINTERNET OpenURL (HINTERNET InternetSession,char *url)
{
	return InternetOpenUrlA(InternetSession,url,NULL,NULL,INTERNET_FLAG_RELOAD,NULL);
}
HINTERNET ConnectFTPSession(HINTERNET InternetSession,char *HostServer, char *Username, char *Password)
{
	return InternetConnect(InternetSession, HostServer, INTERNET_DEFAULT_FTP_PORT, Username, Password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
}
bool ReadInternetData(char *url,LPVOID pBuffer,DWORD Size)
{
	 bool Result = false;
	 HINTERNET MyServer = ConnectInternet();
	 if(InternetReadFile(OpenURL(MyServer,url),pBuffer,Size,&Size))Result = true;
	 DisconnectInternet(MyServer);
	 return Result;
}
bool WriteInternetData(HINTERNET OpenURL,LPVOID pBuffer,DWORD Size)
{
	if(InternetWriteFile(OpenURL,pBuffer,Size,&Size))return true;
	return false;
}
