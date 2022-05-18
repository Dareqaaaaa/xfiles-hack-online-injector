static size_t read_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  curl_off_t nread;
  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */ 
  size_t retcode = fread(ptr, size, nmemb, stream);
 
  nread = (curl_off_t)retcode;
 
  return retcode;
}
#include <sys/types.h>
#include <sys/stat.h>
 bool CheckAndProcessForBugReportFile()
 {
	 #define LOCAL_FILE      "C:\\XFilesCrashInformation.log"
	 char UPLOAD_FILE_AS[500] = {0};
	 char REMOTE_URL[500] = {0};
	 sprintf(UPLOAD_FILE_AS,"XFilesHackBugReportData_%d%02d%02d%04d%02d%02d.log",MyParameter->UserID,timeinfo.tm_mday,timeinfo.tm_mon,timeinfo.tm_year+1900,timeinfo.tm_hour,timeinfo.tm_min);
	 sprintf(REMOTE_URL,"ftp://ftp.xfileshack.net/%s",UPLOAD_FILE_AS);

  CURL *curl;
  CURLcode res;
  FILE *hd_src;
  struct stat file_info;
  curl_off_t fsize;
 
  struct curl_slist *headerlist=NULL;

  /* get the file size of the local file */ 
  if(stat(LOCAL_FILE, &file_info)) {
    return false;
  }
  fsize = (curl_off_t)file_info.st_size;

  if(fsize == 0)return false;
 
 
  /* get a FILE * of the same file */ 
  hd_src = fopen(LOCAL_FILE, "rb");
  if(!hd_src)return false;
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
 
    /* we want to use our own read function */ 
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
 
    /* enable uploading */ 
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
	curl_easy_setopt(curl, CURLOPT_USERPWD, "bugreporter@xfileshack.net:3TywZO!;hxD[A;l0gz"); 

    /* specify target */ 
    curl_easy_setopt(curl,CURLOPT_URL, REMOTE_URL);
 
    /* now specify which file to upload */ 
    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
	 curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
 
    /* Set the size of the file to upload (optional).  If you give a *_LARGE
       option you MUST make sure that the type of the passed-in argument is a
       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
       make sure that to pass in a type 'long' argument. */ 
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)fsize);
 
    /* Now run off and do what you've been told! */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)return false;
    /* clean up the FTP commands list */ 
    curl_slist_free_all (headerlist);
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  fclose(hd_src); /* close the local file */ 
 DeleteFile(LOCAL_FILE);
  curl_global_cleanup();


  	char Message[500];
	sprintf(Message,"Mohon maaf, Terjadi kesalahan dengan Kinerja DLL kami. Masalah telah di laporkan secara otomatis ke server untuk penanganan lebih lanjut, maaf atas ketidak nyamanan yang terjadi ini.");
	MessageBoxA(0,Message,"XFiles D3D Menu 2014",MB_OK + MB_ICONHAND + MB_TOPMOST);
	return true;
 }