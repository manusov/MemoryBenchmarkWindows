; Test scenario for DLL, variant 32-bit.

;--- Start code section ---
include 'win32a.inc'
format PE GUI 4.0
entry start
section '.text' code readable executable
start:


;--- Pre-blank registers ---
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
xor esi,esi
xor edi,edi
xor ebp,ebp

;--- Test call,return EAX = status ---
call [CheckCpuid] 


;--- Start build report text, push registers ---
push edi esi ebp
lea ebp,[esp+4*3]            ; EBP as ESP
push ebp edx ecx ebx eax     
;--- Pop registers and built ASCII strings ---
cld
mov ecx,8
lea esi,[Message]
lea edi,[Buffer]
DumpRegs:
movsw
movsb
mov eax,' =  '
stosd
pop eax
call HexPrint32
mov ax,0D0Ah
stosw
loop DumpRegs
mov al,0
stosb
;--- Visualization ---
push 0                       ; Parm #4 = Message flags
push Caption                ; Parm #3 = Caption (upper message)
push Buffer                 ; Parm #2 = Message
push 0                       ; Parm #1 = Parent window
call [MessageBoxA]           ; Call target function - show window
;--- Exit ---
push 0                       ; Parm#1 = 0, exit code
call [ExitProcess]           ; Exit from application

;--- Subroutines ---
;---------- Print 32-bit Hex Number ---------;
; INPUT:  EAX = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint32:
push eax
ror eax,16
call HexPrint16
pop eax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------;
; INPUT:  AX  = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint16:
push eax
xchg al,ah
call HexPrint8
pop eax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------;
; INPUT:  AL  = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint8:
push eax
ror al,4
call HexPrint4
pop eax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------;
; INPUT:  AL  = Number (bits 0-3)            ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint4:
cld
push eax
and al,0Fh
cmp al,9
ja HP4AF
add al,'0'
jmp HP4Store
HP4AF:
add al,'A'-10
HP4Store:
stosb
pop eax
ret

;==- Data section ---
section '.data' data readable writeable
Caption  DB  '  GPR dump',0
Message  DB  'EAXEBXECXEDXESPEBPESIEDI'
Buffer   DB  1024 DUP (?)
Data1    DB  16384 DUP (?)
Data2    DB  16384 DUP (?)

;--- Import section ---
section '.idata' import data readable writeable
library kernel32,'KERNEL32.DLL',user32,'USER32.DLL',mpe_w_32,'MPE_W_32.DLL'
;--- OS functions ---
include 'api\kernel32.inc'    ; KERNEL32.DLL required because ExitProcess used
include 'api\user32.inc'      ; USER32.DLL required because MessageBoxA used
;--- MPE library functions ---
import mpe_w_32 ,\
GetDllStrings   , 'GetDllStrings'   ,\
CheckCpuid      , 'CheckCpuid'      ,\
ExecuteCpuid    , 'ExecuteCpuid'    ,\
ExecuteRdtsc    , 'ExecuteRdtsc'    ,\
ExecuteXgetbv   , 'ExecuteXgetbv'   ,\
MeasureTsc      , 'MeasureTsc'      ,\
PerformanceGate , 'PerformanceGate'  

