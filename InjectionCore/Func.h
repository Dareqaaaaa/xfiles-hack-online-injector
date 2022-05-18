
void HideModule(HINSTANCE hModule)
{
        DWORD dwPEB_LDR_DATA = 0;
        _asm
        {
                pushad;
                pushfd;
                mov eax, fs:[30h]
                mov eax, [eax+0Ch]
                mov dwPEB_LDR_DATA, eax 

                InLoadOrderModuleList:
                        mov esi, [eax+0Ch]
                        mov edx, [eax+10h]

                LoopInLoadOrderModuleList: 
                    lodsd
                        mov esi, eax
                        mov ecx, [eax+18h]
                        cmp ecx, hModule
                        jne SkipA
                    mov ebx, [eax]
                    mov ecx, [eax+4]
                    mov [ecx], ebx
                    mov [ebx+4], ecx
                        jmp InMemoryOrderModuleList

                SkipA:
                        cmp edx, esi
                        jne LoopInLoadOrderModuleList

                InMemoryOrderModuleList:
                        mov eax, dwPEB_LDR_DATA
                        mov esi, [eax+14h]
                        mov edx, [eax+18h]

                LoopInMemoryOrderModuleList:
                        lodsd
                        mov esi, eax
                        mov ecx, [eax+10h]
                        cmp ecx, hModule
                        jne SkipB
                        mov ebx, [eax]
                        mov ecx, [eax+4]
                        mov [ecx], ebx
                        mov [ebx+4], ecx
                        jmp InInitializationOrderModuleList

                SkipB:
                        cmp edx, esi
                        jne LoopInMemoryOrderModuleList

                InInitializationOrderModuleList:
                        mov eax, dwPEB_LDR_DATA
                        mov esi, [eax+1Ch]
                        mov edx, [eax+20h]

                LoopInInitializationOrderModuleList: 
                        lodsd
                        mov esi, eax
                        mov ecx, [eax+08h]
                        cmp ecx, hModule
                        jne SkipC
                        mov ebx, [eax]
                        mov ecx, [eax+4]
                        mov [ecx], ebx
                        mov [ebx+4], ecx
                        jmp Finished

                SkipC:
                        cmp edx, esi
                        jne LoopInInitializationOrderModuleList

                Finished:
                        popfd;
                        popad;
        }
}
void EraseHeaders(HINSTANCE hModule)
{
        /* 
        * just a func to erase headers by Croner.
        * keep in mind you wont be able to load 
        * any resources after you erase headers.
        */
        PIMAGE_DOS_HEADER pDoH; 
        PIMAGE_NT_HEADERS pNtH;
        DWORD i, ersize, protect;

        if (!hModule) return;
        
        // well just to make clear what we doing
        pDoH = (PIMAGE_DOS_HEADER)(hModule);
		pNtH = (PIMAGE_NT_HEADERS)((LONG)hModule + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew);
        ersize = sizeof(IMAGE_DOS_HEADER);
        if ( VirtualProtect(pDoH, ersize, PAGE_READWRITE, &protect) )
        {
                for ( i=0; i < ersize; i++ )
					*(BYTE*)((BYTE*)pDoH + i) = 0;
        }

        ersize = sizeof(IMAGE_NT_HEADERS);
        if ( pNtH && VirtualProtect(pNtH, ersize, PAGE_READWRITE, &protect) )
        {
                for ( i=0; i < ersize; i++ )
					*(BYTE*)((BYTE*)pNtH + i) = 0;
        }
        return;

}

VOID ChangeMEM(void *MemoryTarget, char* CCD,int len)
{
    DWORD dwOldProtect, dwBkup;
    VirtualProtect(MemoryTarget, len, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy((void*)MemoryTarget,(void*)CCD,len);
    VirtualProtect(MemoryTarget, len, dwOldProtect, &dwBkup);
}
