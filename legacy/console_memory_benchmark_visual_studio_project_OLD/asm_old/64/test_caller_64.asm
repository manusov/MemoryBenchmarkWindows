; Test scenario for DLL, variant 64-bit.

;--- Start code section ---
include 'win64a.inc'
format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
;--- Initializing context ---
sub rsp,8*5                 ; Create and align stack frame


;--- Pre-blank registers ---
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
xor esi,esi
xor edi,edi
xor ebp,ebp
xor r8d,r8d
xor r9d,r9d
xor r10d,r10d
xor r11d,r11d
xor r12d,r12d
xor r13d,r13d
xor r14d,r14d
xor r15d,r15d

;--- Test call,return EAX = status ---
call [CheckCpuid] 


;--- Start build report text, push registers ---
push r15 r14 r13 r12 r11 r10 r9 r8
push rdi rsi rbp
lea r8,[rsp+8*13]           ; R8 as RSP
push r8 rdx rcx rbx rax     
;--- Pop registers and built ASCII strings ---
cld
mov ecx,16
lea rsi,[Message]
lea rdi,[Buffer]
DumpRegs:
movsw
movsb
mov eax,' =  '
stosd
pop rax
call HexPrint64
mov ax,0D0Ah
stosw
loop DumpRegs
mov al,0
stosb
;--- Visualization ---
push rbp                    ; Save RBP
mov rbp,rsp                 ; Save RSP 
sub rsp,32                  ; Create parameters shadow
and rsp,0FFFFFFFFFFFFFFF0h  ; Align RSP required for API Call
xor ecx,ecx                 ; RCX = Parm #1 = Parent window
lea rdx,[Buffer]            ; RDX = Parm #2 = Message
lea r8,[Caption]            ; R8  = Parm #3 = Caption (upper message)
xor r9,r9                   ; R9  = Parm #4 = Message flags
call [MessageBoxA]          ; Call target function - show window
mov rsp,rbp                 ; Restore RSP
pop rbp                     ; Restore RBP
;--- Exit ---
xor ecx,ecx                   ; RCX=0, exit code
call [ExitProcess]            ; Exit from application

;--- Subroutines ---
;---------- Print 64-bit Hex Number ---------;
; INPUT:  RAX = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint64:
push rax
ror rax,32
call HexPrint32
pop rax
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------;
; INPUT:  EAX = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint32:
push rax
ror eax,16
call HexPrint16
pop rax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------;
; INPUT:  AX  = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint16:
push rax
xchg al,ah
call HexPrint8
pop rax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------;
; INPUT:  AL  = Number                       ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint8:
push rax
ror al,4
call HexPrint4
pop rax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------;
; INPUT:  AL  = Number (bits 0-3)            ;
;         RDI = Destination Pointer          ;
; OUTPUT: RDI = Modify                       ;
;--------------------------------------------;
HexPrint4:
cld
push rax
and al,0Fh
cmp al,9
ja HP4AF
add al,'0'
jmp HP4Store
HP4AF:
add al,'A'-10
HP4Store:
stosb
pop rax
ret

;--- Data section ---
section '.data' data readable writeable
;--- Dump support ---
Caption  DB  '  GPR dump',0
Message  DB  'RAXRBXRCXRDXRSPRBPRSIRDI'
         DB  'R8 R9 R10R11R12R13R14R15'
Buffer   DB  1024 DUP (?)
Data1    DB  16384 DUP (?)
Data2    DB  16384 DUP (?)

;--- Import section ---
section '.idata' import data readable writeable
library kernel32,'KERNEL32.DLL',user32,'USER32.DLL',mpe_w_64,'MPE64.DLL'
;--- OS functions ---
include 'api\kernel32.inc'    ; KERNEL32.DLL required because ExitProcess used
include 'api\user32.inc'      ; USER32.DLL required because MessageBoxA used
;--- MPE library functions ---
import mpe_w_64 ,\
GetDllStrings   , 'GetDllStrings'   ,\
CheckCpuid      , 'CheckCpuid'      ,\
ExecuteCpuid    , 'ExecuteCpuid'    ,\
ExecuteRdtsc    , 'ExecuteRdtsc'    ,\
ExecuteXgetbv   , 'ExecuteXgetbv'   ,\
MeasureTsc      , 'MeasureTsc'      ,\
PerformanceGate , 'PerformanceGate'  

