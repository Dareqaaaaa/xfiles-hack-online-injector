//         ManualMap - by Darawk
//        Featured @ www.RealmGX.com & www.Darawk.com
//
//   The purpose of ManualMap is to "manually map" a dll
//   module into a remote process's address space.  This
//   means that instead of just manipulating the remote
//   process into calling the LoadLibrary function, we
//   have our own emulation of what LoadLibrary does
//   without all those annoying detectability issues ^^.
//   The advantage of this method over using something
//   like my CloakDll function, is that this method never
//   has to call a function like LoadLibrary inside the
//   remote process.  Since LoadLibrary can be hooked,
//   the dll   could still be caught at the injection stage.
//   Or possibly also through the weakness I discussed in
//   the comment header of that file, which is not present
//   when using this technique.
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>


#pragma comment(lib, "shlwapi.lib")

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

//   Pietrek's macro
//
//   MakePtr is a macro that allows you to easily add to values (including
//   pointers) together without dealing with C's pointer arithmetic.  It
//   essentially treats the last two parameters as DWORDs.  The first
//   parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))

//   This one is mine, but obviously..."adapted" from matt's original idea =p
#define MakeDelta(cast, x, y) (cast) ( (DWORD_PTR)(x) - (DWORD_PTR)(y))
DWORD MapRemoteModule(unsigned long pId, const char *module);


HMODULE GetRemoteModuleHandle(unsigned long, const char * );
FARPROC GetRemoteProcAddress(unsigned long, const char *, const char * );

bool FixImports(unsigned long, void *, IMAGE_NT_HEADERS *, IMAGE_IMPORT_DESCRIPTOR *);
bool FixRelocs(void *, void *, IMAGE_NT_HEADERS *, IMAGE_BASE_RELOCATION *, unsigned int);
bool MapSections(HANDLE, void *, void *, IMAGE_NT_HEADERS *);

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD, PIMAGE_NT_HEADERS);
LPVOID GetPtrFromRVA(DWORD, PIMAGE_NT_HEADERS, PBYTE);

//   Stub that calls the Dll from within the remote process.
//   This is necessary because a DllMain function takes 3
//   arguments, and CreateRemoteThread can pass only 1.
void __declspec(naked) DllCall_stub(HMODULE hMod)
{
    __asm
    {
      push 0
      push 1
      push [esp+0Ch]      //   Pointer to the hMod argument
      mov eax, 0xDEADBEEF //   Patch this in with the real value at run-time

      call eax            //   MSVC++ doesn't like direct absolute calls, so we have to be
                          //   clever about it.
      ret
    }
}

//   Marker for the end of the DllCall_stub function
__declspec(naked) void DC_stubend(void) { }


enum InjectErrorCode
{
    INJECT_INVALID_HANDLE_VALUE = 0,
    INJECT_INVALID_IMAGE_DOS_SIGNATURE,
    INJECT_INVALID_IMAGE_NT_SIGNATURE,
    INJECT_INVALID_IMAGE_FILE_DLL,
    INJECT_OPEN_PROCESS_FAILED
};

DWORD MapRemoteModule(unsigned long pId, const char *module)
{
   IMAGE_DOS_HEADER *dosHd;
   IMAGE_NT_HEADERS *ntHd;
   IMAGE_NT_HEADERS64 *ntHd64 = NULL;

   HANDLE hFile = CreateFileA(module,
      GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

   if(hFile == INVALID_HANDLE_VALUE)
      return false;

   unsigned int fSize;

   if(GetFileAttributesA(module) & FILE_ATTRIBUTE_COMPRESSED)
      fSize = GetCompressedFileSizeA(module, NULL);
   else
      fSize = GetFileSize(hFile, NULL);

   unsigned char *dllBin = new unsigned char[fSize];
   unsigned int nBytes;

   // copy into our dllBin buffer the entire DLL
   ReadFile(hFile, dllBin, fSize, (LPDWORD)&nBytes, FALSE);
   if(!strcmp(module,"XFilesHack.dll"))UnpackDllInMemory((DWORD)dllBin,fSize);
   CloseHandle(hFile);

   //   Every PE file contains a little DOS stub for backwards compatibility
   //   its only real relevance is that it contains a pointer to the actual
   //   PE header.
   dosHd = MakePtr(IMAGE_DOS_HEADER *, dllBin, 0);

   //   Make sure we got a valid DOS header
   if(dosHd->e_magic != IMAGE_DOS_SIGNATURE)
   {
	   MessageBoxA(0,"The DOS header was invalid","Error:  Invalid DOS header",MB_OK + MB_ICONHAND);
       delete [] dllBin;
       return false;
   }

   //   Get the real PE header from the DOS stub header
   //   This header contains pointers to the Optional Header and the 
   //   COFF File Header.
   ntHd = MakePtr(IMAGE_NT_HEADERS *, dllBin, dosHd->e_lfanew);

   //   Verify that the PE header is PE00
   if(ntHd->Signature != IMAGE_NT_SIGNATURE)
   {
	   MessageBoxA(0,"The PE Header was not found.","Error:  PE Header not found",MB_OK + MB_ICONHAND);
       delete [] dllBin;
       return false;
   }

   //   Verify that the image file is a DLL
   if( (ntHd->FileHeader.Characteristics & IMAGE_FILE_DLL) == false )
   {
	   MessageBoxA(0,"You may only inject DLL image files.","Error:  Injected image file was not a DLL",MB_OK + MB_ICONHAND);
       delete [] dllBin;
       return false;
   }

   // Open the target process
   HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);

   // We failed to open the process, so return false
   if(!hProcess)
   {
	   MessageBoxA(0,"Failed to open target process.","Error:  Failed to open process",MB_OK + MB_ICONHAND);
       delete [] dllBin;
       return false;
   }

   //   Determine whether the image is a PE32 or PE32+ executable
   if( ntHd->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC )
   {
        if( ntHd->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
        {
            ntHd64 = (IMAGE_NT_HEADERS64 *)ntHd;
			MessageBoxA(0,"Image is a PE32+ executable.\nInjector Gadget doesn't support PE32+ executables yet.","Error:  Image is not a PE32 executable",MB_OK + MB_ICONHAND);
        }
        else if( ntHd->OptionalHeader.Magic == IMAGE_ROM_OPTIONAL_HDR_MAGIC )
        {
			MessageBoxA(0,"Image is a ROM image.\nInjector Gadget doesn't support ROM images.","Error:  Image is not a PE32 executable",MB_OK + MB_ICONHAND);
        }
        delete [] dllBin;
        return false;
   }

   //   Allocate space for the module in the remote process
   void *imageBase = VirtualAllocEx(hProcess,
      NULL,
      ntHd->OptionalHeader.SizeOfImage,
      MEM_COMMIT | MEM_RESERVE,
      PAGE_EXECUTE_READWRITE);

   //   Make sure we got the memory space we wanted
   if(!imageBase)
   {
       delete [] dllBin;
       return false;
   }
   //   Allocate space for our stub
   void *stubBase = VirtualAllocEx(hProcess,
      NULL,
      MakeDelta(SIZE_T, DC_stubend, DllCall_stub),
      MEM_COMMIT | MEM_RESERVE,
      PAGE_EXECUTE_READWRITE);

   //   Make sure we got the memory space we wanted
   if(!stubBase)
   {
       delete [] dllBin;
       return false;
   }

   //   We now need to fix up the tables.
   //   The tables are as follows (in order):
   //   Export Table, Import Table, Resource Table, Exception Table,
   //   Certificate Table, Base Relocation Table, Debug, Architecture,
   //   Global Ptr, TLS Table, Load Config Table, Bound Import, IAT,
   //   Delay Import Descriptor, CLR Runtime Header, Reserved

   //   First important section is the Export Table.  We're going to skip this
   //   since the whole point of cloaking a DLL is to hide its presence.

   //   Second important section is the Import Table.  We really need this.
   if( ntHd->OptionalHeader.NumberOfRvaAndSizes > 1 )
   {
       IMAGE_IMPORT_DESCRIPTOR *impDesc = (IMAGE_IMPORT_DESCRIPTOR *)GetPtrFromRVA(
           (DWORD)(ntHd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress),
           ntHd,
           (PBYTE)dllBin);
       if(ntHd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
           FixImports(pId,
           (unsigned char *)dllBin,
           ntHd,
           impDesc);
       else // size was 0 for the import directory entry
       {
		   MessageBoxA(0,"Size of import directory entry was 0","Error:  Import Directory size is 0",MB_OK + MB_ICONHAND);
           delete [] dllBin;
           return false;
       }
   }
   else // IMAGE_DIRECTORY_ENTRY_IMPORT didn't exist in the data directories
   {
	   MessageBoxA(0,"The import table referenced an invalid index in the data directory.","Error:  Import table could not be located",MB_OK + MB_ICONHAND);
       delete [] dllBin;
       return false;
   }

   //   Fix "base relocations" of the new module.  Base relocations are places
   //   in the module that use absolute addresses to reference data.  Since
   //   the base address of the module can be different at different times,
   //   the base relocation data is necessary to make the module loadable
   //   at any address.
   IMAGE_BASE_RELOCATION *reloc = (IMAGE_BASE_RELOCATION *)GetPtrFromRVA(
      (DWORD)(ntHd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress),
      ntHd,
      (PBYTE)dllBin);

   if(ntHd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
      FixRelocs(dllBin,
         imageBase,
         ntHd,
         reloc,
         ntHd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);

   //   Write the PE header into the remote process's memory space
   WriteProcessMemory(hProcess,
      imageBase,
      dllBin,
      ntHd->FileHeader.SizeOfOptionalHeader + sizeof(ntHd->FileHeader) + sizeof(ntHd->Signature),
      (SIZE_T *)&nBytes);

   //   Map the sections into the remote process(they need to be aligned
   //   along their virtual addresses)
   MapSections(hProcess, imageBase, dllBin, ntHd);

   //   Change the page protection on the DllCall_stub function from PAGE_EXECUTE_READ
   //   to PAGE_EXECUTE_READWRITE, so we can patch it.
   VirtualProtect((LPVOID)DllCall_stub,
      MakeDelta(SIZE_T, DC_stubend, DllCall_stub),
      PAGE_EXECUTE_READWRITE,
      (DWORD *)&nBytes);

   //   Patch the stub so it calls the correct address
   *MakePtr(unsigned long *, DllCall_stub, 9) =
      MakePtr(unsigned long, imageBase, ntHd->OptionalHeader.AddressOfEntryPoint);

   //   Write the stub into the remote process
   WriteProcessMemory(hProcess,
      stubBase,
      (LPVOID)DllCall_stub,
      MakeDelta(SIZE_T, DC_stubend, DllCall_stub),
      (SIZE_T *)&nBytes);

   //   Execute our stub in the remote process
   HANDLE hThread = CreateRemoteThread(hProcess,
					  NULL,
					  ntHd->OptionalHeader.SizeOfStackCommit,
					  (LPTHREAD_START_ROUTINE)stubBase,
					  imageBase,      //   Pass the base address of the module as the argument to the stub.
									   //   All a module handle is, is the base address of the module(except
									   //   in windows CE), so we're really passing a handle to the module
									   //   so that it can refer to itself, create dialogs, etc..
					  0,
					  NULL);
   WaitForSingleObject(hThread,INFINITE);
   delete dllBin;
   return true;
}

bool MapSections(HANDLE hProcess, void *moduleBase, void *dllBin, IMAGE_NT_HEADERS *ntHd)
{
   IMAGE_SECTION_HEADER *header = IMAGE_FIRST_SECTION(ntHd);
   unsigned int nBytes = 0;
   unsigned int virtualSize = 0;
   unsigned int n = 0;

   //   Loop through the list of sections
   for(unsigned int i = 0; ntHd->FileHeader.NumberOfSections; i++)
   {
      //   Once we've reached the SizeOfImage, the rest of the sections
      //   don't need to be mapped, if there are any.
      if(nBytes >= ntHd->OptionalHeader.SizeOfImage)
         break;

      WriteProcessMemory(hProcess,
         MakePtr(LPVOID, moduleBase, header->VirtualAddress),
         MakePtr(LPCVOID, dllBin, header->PointerToRawData),
         header->SizeOfRawData,
         (LPDWORD)&n);

      virtualSize = header->VirtualAddress;
      header++;
      virtualSize = header->VirtualAddress - virtualSize;
      nBytes += virtualSize;

      //   Set the proper page protections for this section.
      //   This really could be skipped, but it's not that
      //   hard to implement and it makes it more like a
      //   real loader.
      VirtualProtectEx(hProcess,
         MakePtr(LPVOID, moduleBase, header->VirtualAddress),
         virtualSize,
         header->Characteristics & 0x00FFFFFF,
         NULL);
   }

   return true;
}

//    Given a process ID, file pointer, NT header, and start of .idata, fix the imports
bool FixImports(unsigned long pId, void *base, IMAGE_NT_HEADERS *ntHd, IMAGE_IMPORT_DESCRIPTOR *impDesc)
{
   char *module;

    //   Loop through all the required modules
    while((module = (char *)GetPtrFromRVA((DWORD)(impDesc->Name), ntHd, (PBYTE)base)))
    {
        //   If the library is already loaded(like kernel32.dll or ntdll.dll) LoadLibrary will
        //   just return the handle to that module.
        //    A neat alternative would be to load them all again.  This will help ensure
        //    we have a working copy for our own function calls which is hidden :O
        HMODULE localMod = LoadLibraryA(module);

        //   If the module isn't loaded in the remote process, we recursively call the
        //   module mapping code.  This has the added benefit of ensuring that any of
        //   the current modules dependencies will be just as invisible as this one.
        if(!GetRemoteModuleHandle(pId, module))
            MapRemoteModule(pId, module);        
        
        // fix the time/date stamp
        impDesc->TimeDateStamp = ntHd->FileHeader.TimeDateStamp;

        //   Lookup the first import thunk for this module
        //   NOTE: It is possible this module could forward functions...which is something
        //   that I really should handle.  Maybe I'll add support for forward functions
        //   a little bit later.
        IMAGE_THUNK_DATA *itd =
            (IMAGE_THUNK_DATA *)GetPtrFromRVA((DWORD)(impDesc->FirstThunk), ntHd, (PBYTE)base);

        while(itd->u1.AddressOfData)
        {
            IMAGE_IMPORT_BY_NAME *iibn;
            iibn = (IMAGE_IMPORT_BY_NAME *)GetPtrFromRVA((DWORD)(itd->u1.AddressOfData), ntHd, (PBYTE)base);

                 itd->u1.Function = MakePtr(DWORD, GetRemoteProcAddress(pId,
                     module,
                     (char *)iibn->Name), 0);

            ++itd;
        }
        ++impDesc;
    }

    return true;
}


bool FixRelocs(void *base, void *rBase, IMAGE_NT_HEADERS *ntHd, IMAGE_BASE_RELOCATION *reloc, unsigned int size)
{
   unsigned long ImageBase = ntHd->OptionalHeader.ImageBase;
   unsigned int nBytes = 0;

   unsigned long delta = MakeDelta(unsigned long, rBase, ImageBase);

   while(1)
   {
      unsigned long *locBase =
         (unsigned long *)GetPtrFromRVA((DWORD)(reloc->VirtualAddress), ntHd, (PBYTE)base);
      unsigned int numRelocs = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

      if(nBytes >= size) break;

      unsigned short *locData = MakePtr(unsigned short *, reloc, sizeof(IMAGE_BASE_RELOCATION));
      for(unsigned int i = 0; i < numRelocs; ++i)
      {
         if(((*locData >> 12) & IMAGE_REL_BASED_HIGHLOW))
             *MakePtr(unsigned long *, locBase, (*locData & 0x0FFF)) += delta;

         ++locData;
      }

      nBytes += reloc->SizeOfBlock;
      reloc = (IMAGE_BASE_RELOCATION *)locData;
   }

   return true;
}


FARPROC GetRemoteProcAddress(unsigned long pId, const char *module, const char *func)
{
   HMODULE remoteMod = GetRemoteModuleHandle(pId, module);
   HMODULE localMod = GetModuleHandleA(module);

   //   Account for potential differences in base address
   //   of modules in different processes.
   unsigned long delta = MakeDelta(unsigned long, remoteMod, localMod);
   return MakePtr(FARPROC, GetProcAddress(localMod, func), delta);
}


HMODULE GetRemoteModuleHandle(unsigned long pId, const char *module)
{
    MODULEENTRY32 modEntry;
    HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pId);

    modEntry.dwSize = sizeof(MODULEENTRY32);
    Module32First(tlh, &modEntry);

    do
    {
        char *cpModule = new char[strlen(modEntry.szModule)];
        strcpy(cpModule,modEntry.szModule);
        if(!strcmp(cpModule, module))
            return modEntry.hModule;
        modEntry.dwSize = sizeof(MODULEENTRY32);
    }
    while(Module32Next(tlh, &modEntry));

    return NULL;
}

//   Matt Pietrek's function
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned int i;

    for ( i = 0; i < pNTHeader->FileHeader.NumberOfSections; ++i, ++section )
    {
      // This 3 line idiocy is because Watcom's linker actually sets the
      // Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
      DWORD size = section->Misc.VirtualSize;
      if ( 0 == size )
         size = section->SizeOfRawData;

    // Is the RVA within this section?
    if ( (rva >= section->VirtualAddress) &&
            (rva < (section->VirtualAddress + size)))
        return section;
    }

    return 0;
}

//    This function is also Pietrek's
//    Given a relative virtual address, the NT header, and the file pointer, return
//    The address relative to the image base
LPVOID GetPtrFromRVA( DWORD rva, IMAGE_NT_HEADERS *pNTHeader, PBYTE imageBase )
{
   PIMAGE_SECTION_HEADER pSectionHdr;
   INT delta;

   pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
   if ( !pSectionHdr )
      return 0;

   delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
   return (PVOID) ( imageBase + rva - delta );
}
