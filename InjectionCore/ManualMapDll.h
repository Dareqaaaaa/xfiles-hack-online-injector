typedef HMODULE (WINAPI *pLoadLibraryA)(LPCSTR);
typedef FARPROC (WINAPI *pGetProcAddress)(HMODULE,LPCSTR);
typedef BOOL (WINAPI *PDLL_MAIN)(HMODULE,DWORD,PVOID);


 
typedef struct _MANUAL_INJECT
{
    PVOID ImageBase;
    PIMAGE_NT_HEADERS NtHeaders;
    PIMAGE_BASE_RELOCATION BaseRelocation;
    PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;
    pLoadLibraryA fnLoadLibraryA;
    pGetProcAddress fnGetProcAddress;
}MANUAL_INJECT,*PMANUAL_INJECT;
 
DWORD WINAPI LoadDll(PVOID p)
{
    PMANUAL_INJECT ManualInject;
 
    HMODULE hModule;
    DWORD i,Function,count,delta;
 
    PDWORD ptr;
    PWORD list;
 
    PIMAGE_BASE_RELOCATION pIBR;
    PIMAGE_IMPORT_DESCRIPTOR pIID;
    PIMAGE_IMPORT_BY_NAME pIBN;
    PIMAGE_THUNK_DATA FirstThunk,OrigFirstThunk;
 
    PDLL_MAIN EntryPoint;
 
    ManualInject=(PMANUAL_INJECT)p;
 
    pIBR=ManualInject->BaseRelocation;
    delta=(DWORD)((LPBYTE)ManualInject->ImageBase-ManualInject->NtHeaders->OptionalHeader.ImageBase); // Calculate the delta
 
    // Relocate the image
 
    while(pIBR->VirtualAddress)
    {
        if(pIBR->SizeOfBlock>=sizeof(IMAGE_BASE_RELOCATION))
        {
            count=(pIBR->SizeOfBlock-sizeof(IMAGE_BASE_RELOCATION))/sizeof(WORD);
            list=(PWORD)(pIBR+1);
 
            for(i=0;i<count;i++)
            {
                if(list[i])
                {
                    ptr=(PDWORD)((LPBYTE)ManualInject->ImageBase+(pIBR->VirtualAddress+(list[i] & 0xFFF)));
                    *ptr+=delta;
                }
            }
        }
 
        pIBR=(PIMAGE_BASE_RELOCATION)((LPBYTE)pIBR+pIBR->SizeOfBlock);
    }
 
    pIID=ManualInject->ImportDirectory;
 
    // Resolve DLL imports
 
    while(pIID->Characteristics)
    {
        OrigFirstThunk=(PIMAGE_THUNK_DATA)((LPBYTE)ManualInject->ImageBase+pIID->OriginalFirstThunk);
        FirstThunk=(PIMAGE_THUNK_DATA)((LPBYTE)ManualInject->ImageBase+pIID->FirstThunk);
 
        hModule=ManualInject->fnLoadLibraryA((LPCSTR)ManualInject->ImageBase+pIID->Name);
		
        if(!hModule)
        {
            return FALSE;
        }
		
 
        while(OrigFirstThunk->u1.AddressOfData)
        {
            if(OrigFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
            {
                // Import by ordinal
                 
                Function=(DWORD)ManualInject->fnGetProcAddress(hModule,(LPCSTR)(OrigFirstThunk->u1.Ordinal & 0xFFFF));
				/*
                if(!Function)
                {
                    return FALSE;
                }
				*/
 
                FirstThunk->u1.Function=Function;
            }
 
            else
            {
                // Import by name
 
                pIBN=(PIMAGE_IMPORT_BY_NAME)((LPBYTE)ManualInject->ImageBase+OrigFirstThunk->u1.AddressOfData);
                Function=(DWORD)ManualInject->fnGetProcAddress(hModule,(LPCSTR)pIBN->Name);
				/*
                if(!Function)
                {
                    return FALSE;
                }
				*/
 
                FirstThunk->u1.Function=Function;
            }
 
            OrigFirstThunk++;
            FirstThunk++;
        }
 
        pIID++;
    }
 
    if(ManualInject->NtHeaders->OptionalHeader.AddressOfEntryPoint)
    {
        EntryPoint=(PDLL_MAIN)((LPBYTE)ManualInject->ImageBase+ManualInject->NtHeaders->OptionalHeader.AddressOfEntryPoint);
		DWORD Image = (DWORD)(ManualInject->ImageBase);
		for(int i = 0; i < 0x1000; i++)*(BYTE*)(Image + i) = *(BYTE*)(Image + i) ^ i * 14 - (i%5);
        return EntryPoint((HMODULE)ManualInject->ImageBase,DLL_PROCESS_ATTACH,NULL); // Call the entry point
    }
 
    return TRUE;
}
 
DWORD WINAPI LoadDllEnd()
{
    return 0;
}