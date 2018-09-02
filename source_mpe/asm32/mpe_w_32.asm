format PE GUI 4.0 DLL
entry  DllMain
include 'win32a.inc'

; Code section
section '.text' code readable executable

; System subroutine
align 64
DllMain:
mov eax,1        ; EAX=1 means call when start process
ret

; Get strings
; INPUT:
; Parm#1 = [ESP+04] = Pointer to variable = pointer to product string
; Parm#2 = [ESP+08] = Pointer to variable = pointer to version string
; Parm#3 = [ESP+12]  = Pointer to variable = pointer to copyright string
; OUTPUT:
; void
GetDllStrings:
mov ecx,[esp+04]
jecxz @f                  ; check for NULL pointer to product string
lea eax,[StringProduct]
mov [ecx],eax
@@:
mov ecx,[esp+08] 
jecxz @f                  ; check for NULL pointer to version string                
lea eax,[StringVersion]
mov [ecx],eax
@@:
mov ecx,[esp+12]
jecxz @f                  ; check for NULL pointer to copyright string
lea eax,[StringCopyright]
mov [ecx],eax
@@:
xor eax,eax
ret

; Check CPUID support, note may be locked by virtualization
; INPUT:
; void
; OUTPUT:
; EAX = Status: 0 = CPUID not supported or locked, 
;               otherwise supported and functional
CheckCpuid:
; Check for ID bit writeable for "1"
mov ecx,21
pushf          ; EFLAGS for ia32, but note RFLAGS for x64
pop eax
bts eax,ecx    ; Set EAX.21=1
push eax         
popf           ; Load RFLAGS with RFLAGS.21=1
pushf          ; Store RFLAGS
pop eax        ; Load RFLAGS to RAX
btr eax,ecx    ; Check EAX.21=1, Set EAX.21=0
jnc @f         ; Go error branch if cannot set EFLAGS.21=1
; Check for ID bit writeable for "0"
push eax
popf           ; Load RFLAGS with RFLAGS.21=0
pushf          ; Store RFLAGS
pop eax        ; Load RFLAGS to RAX
bt eax,ecx     ; Check EAX.21=0
jc @f          ; Go if cannot set EFLAGS.21=0
; Exit points
mov eax,-1     ; 0FFFFFFFFh means TRUE
ret
@@:
xor eax,eax    ; 0 means FALSE
ret

; Execute CPUID instruction, assume support already verified
; INPUT:
; Parm#1 = [ESP+04] = CPUID function code (means EAX for CPUID instruction)
; Parm#2 = [ESP+08] = CPUID subfunction code (means ECX for CPUID instruction) 
; Parm#3 = [ESP+12] = Pointer to output DWORD variable = CPUID output EAX  
; Parm#4 = [ESP+16] = Pointer to output DWORD variable = CPUID output EBX 
; Parm#5 = [ESP+20] = Pointer to output DWORD variable = CPUID output ECX 
; Parm#6 = [ESP+24] = Pointer to output DWORD variable = CPUID output EDX
; OUTPUT:
; void 
ExecuteCpuid:
mov eax,[esp+04]
mov ecx,[esp+08]
push ebx esi
cpuid
mov esi,[esp+12+08]
mov [esi],eax
mov esi,[esp+16+08]
mov [esi],ebx
mov esi,[esp+20+08]
mov [esi],ecx
mov esi,[esp+24+08]
mov [esi],edx
pop esi ebx
xor eax,eax
ret

; Execute RDTSC instruction, assume support already verified
; INPUT:
; Parm#1 = [ESP+04] = Pointer to output QWORD variable = 64-bit TSC value
; OUTPUT:
; void
ExecuteRdtsc:
mov ecx,[esp+04]
rdtsc
mov [ecx+00],eax
mov [ecx+04],edx
xor eax,eax
ret 

; Execute XGETBV instruction, assume support already verified
; INPUT:
; Parm#1 = [ESP+04] = Pointer to output QWORD variable = XCR0 bitmap
; OUTPUT:
; void
ExecuteXgetbv:
push ebx
mov ebx,[esp+04+04]
xor ecx,ecx
xgetbv
mov [ebx+00],eax
mov [ebx+04],edx
pop ebx
xor eax,eax
ret 

; Measure CPU TSC clock, assume support already verified
; INPUT:
; Parm#1 = [ESP+04] = Pointer for update output variable, frequency at Hz
; OUTPUT:
; EAX = Reserved for status, return TRUE
MeasureTsc:
xor eax,eax
push esi edi ebp eax eax        ; Last EAX = Variable
; Start measure frequency, wait toggle
mov ebp,esp
push ebp
call [GetSystemTimeAsFileTime]  ; Get current count
mov esi,[ebp]
@@:
push ebp
call [GetSystemTimeAsFileTime]  ; Get next count for wait 100 ns
cmp esi,[ebp]
je @b
; Start time point
mov esi,[ebp+0]
mov edi,[ebp+4]
add esi,10000000                
adc edi,0                       ; EDI:ESI = 10^7 * 100ns = 1 second
rdtsc
push eax edx                    ; Stack qword = 64-bit TSC at operation start
; Delay 1 second
@@:
push ebp
call [GetSystemTimeAsFileTime]  ; Get count for wait 1 second
cmp edi,[ebp+4]                 ; Compare high: target=edi and returned=[ebp+4] 
ja @b                           ; Go wait if target > returned, must 1 second
jb @f
cmp esi,[ebp+0]                 ; Compare low: target=esi and returned=[ebp+0] 
ja @b                           ; Go wait if target > returned, must 1 second
@@:
; Stop time point
rdtsc                           ; EDX:EAX = 64-bit TSC at operation end
pop ecx ebx
sub eax,ebx
sbb edx,ecx
; Remove stack frame, restore registers non-volatile by IA32 calling convention
pop ebp ebp ebp edi esi          ; First POP EBP for RSP-8 only 
; Update output variable
mov ecx,[esp+04]
mov [ecx+00],eax
mov [ecx+04],edx
; Exit with status = TRUE
mov eax,-1
ret

; Performance patterns caller gate
; Parm#1 = [ESP+04] = Pattern selector 
; Parm#2 = [ESP+08] = Block #1 pointer
; Parm#3 = [ESP+12] = Block #2 pointer
; Parm#4 = [ESP+16] = Block length, instructions
; Parm#5 = [ESP+20] = Measurement repeats
; Parm#6 = [ESP+24] = Pointer for update output variable = dTSC
; OUTPUT:
; EAX = Reserved for status, return TRUE
PerformanceGate:
push ebx esi edi ebp
; Load input parameters, part 1
mov esi,[esp+08+16]                    ; ESI = Block #1 pointer (32-bit flat)
mov edi,[esp+12+16]                    ; EDI = Block #2 pointer (32-bit flat)
mov ebp,[esp+20+16]                    ; EBP = Number of measurement repeats 
; This CPUID for events serialization only, results ignored
xor eax,eax                            ; EAX=0 means function 0 for CPUID
cpuid                                  ; This CPUID for serialization only
; Load input parameters, part 2
movzx ebx,word [esp+04+16]             ; EBX = Pattern selector
mov ebx,[PerformancePatterns + ebx*4]  ; EBX = Target routine entry point
mov ecx,[esp+16+16]                    ; ECX = Block length, units=instructions 
; Get start time
rdtsc
push eax edx
; Call target routine
call ebx
; Get end time, calculate delta
rdtsc
pop ecx ebx
sub eax,ebx        ; EAX = Delta-TSC, low bits
sbb edx,ecx        ; EDX = Delta-TSC, high bits
; This CPUID for events serialization only, results ignored
push eax edx
xor eax,eax        ; EAX=0 means function 0 for CPUID
cpuid
pop edx eax
; Restore registers non-volatile by ia32 calling convention
pop ebp edi esi ebx
; Return with update output
mov ecx,[esp+24]
mov [ecx+00],eax
mov [ecx+04],edx
mov eax,-1
ret

; Include files connect for performance patterns
; Temporal (cacheable read and write) mode
include 'read_mov32.inc'
include 'write_mov32.inc'
include 'copy_mov32.inc'
include 'modify_not32.inc'
include 'write_stosd.inc'
include 'copy_movsd.inc'
include 'read_sse128.inc'
include 'write_sse128.inc'
include 'copy_sse128.inc'
include 'read_avx256.inc'
include 'write_avx256.inc'
include 'copy_avx256.inc'
include 'read_avx512.inc'
include 'write_avx512.inc'
include 'copy_avx512.inc'
include 'dot_fma256.inc'
include 'dot_fma512.inc'
; Non-temporal (non-cacheable write) mode
include 'ntwrite_sse128.inc'
include 'ntcopy_sse128.inc'
include 'ntwrite_avx256.inc'
include 'ntcopy_avx256.inc'
include 'ntwrite_avx512.inc'
include 'ntcopy_avx512.inc'
; Non-temporal (non-cacheable read and write) mode
include 'ntread_sse128.inc'
include 'ntrcopy_sse128.inc'
include 'ntread_avx256.inc'
include 'ntrcopy_avx256.inc'
include 'ntread_avx512.inc'
include 'ntrcopy_avx512.inc'
; Non-temporal (non-cacheable read-by-prefetch and write) mode
include 'ntpread_sse128.inc'
include 'ntpread_avx256.inc'

; Data section
section '.data' data readable writeable
StringProduct    DB 'MPE native library.',0
StringVersion    DB 'v0.20.01 for Windows ia32.',0
StringCopyright  DB '(C)2018 IC Book Labs.',0

; Pointers to performance patterns
align 8
PerformancePatterns:
; Temporal (cacheable read and write) mode
DD  Pattern_Read_MOV32
DD  Pattern_Write_MOV32
DD  Pattern_Copy_MOV32
DD  Pattern_Modify_NOT32
DD  Pattern_Write_STOSD
DD  Pattern_Copy_MOVSD
DD  Pattern_Read_SSE128
DD  Pattern_Write_SSE128
DD  Pattern_Copy_SSE128
DD  Pattern_Read_AVX256
DD  Pattern_Write_AVX256
DD  Pattern_Copy_AVX256
DD  Pattern_Read_AVX512
DD  Pattern_Write_AVX512
DD  Pattern_Copy_AVX512
DD  Pattern_Dot_FMA256  
DD  Pattern_Dot_FMA512  
; Non-temporal (non-cacheable write) mode 
DD  Pattern_NtWrite_SSE128
DD  Pattern_NtCopy_SSE128   ; This duplicated 1
DD  Pattern_NtWrite_AVX256
DD  Pattern_NtCopy_AVX256
DD  Pattern_NtWrite_AVX512
DD  Pattern_NtCopy_AVX512
; Non-temporal (non-cacheable read and write) mode
DD  Pattern_NtRead_SSE128
DD  Pattern_NtRCopy_SSE128
DD  Pattern_NtRead_AVX256
DD  Pattern_NtRCopy_AVX256
DD  Pattern_NtRead_AVX512   
DD  Pattern_NtRCopy_AVX512
; Non-temporal (non-cacheable read-by-prefetch and write) mode
DD  Pattern_NtpRead_SSE128
DD  Pattern_NtCopy_SSE128   ; This duplicated 1
DD  Pattern_NtpRead_AVX256
; Reserved for same 256-512 bit operations
; FMA with non-temporal store
; Reserved 

; Export section
section '.edata' export data readable
export  'test1.dll'                 ,\
GetDllStrings   , 'GetDllStrings'   ,\
CheckCpuid      , 'CheckCpuid'      ,\
ExecuteCpuid    , 'ExecuteCpuid'    ,\
ExecuteRdtsc    , 'ExecuteRdtsc'    ,\
ExecuteXgetbv   , 'ExecuteXgetbv'   ,\
MeasureTsc      , 'MeasureTsc'      ,\
PerformanceGate , 'PerformanceGate'  

; Import section
section '.idata' import data readable
library kernel32 , 'KERNEL32.DLL'
include 'api\kernel32.inc'

; Fixups section
data fixups
end data

