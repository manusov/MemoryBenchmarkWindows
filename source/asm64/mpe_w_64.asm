format PE64 DLL
entry  DllMain
include 'win64a.inc'

; Code section
section '.text' code readable executable

; System subroutine
align 64
DllMain:
mov eax,1        ; RAX=1 means call when start process
ret              ; Note operation with EAX clear bits RAX.[63-32]

; Get strings
; INPUT:
; Parm#1 = RCX = Pointer to variable = pointer to product string
; Parm#2 = RDX = Pointer to variable = pointer to version string
; Parm#3 = R8  = Pointer to variable = pointer to copyright string
; OUTPUT:
; void
GetDllStrings:
test rcx,rcx  ; check for NULL pointer to product string
jz @f
lea rax,[StringProduct]
mov [rcx],rax
@@:
test rdx,rdx  ; check for NULL pointer to version string 
jz @f
lea rax,[StringVersion]
mov [rdx],rax
@@:
test r8,r8    ; check for NULL pointer to copyright string
jz @f
lea rax,[StringCopyright]
mov [r8],rax
@@:
xor eax,eax
ret

; Check CPUID support, note may be locked by virtualization
; INPUT:
; void
; OUTPUT:
; RAX = Status: 0 = CPUID not supported or locked, 
;               otherwise supported and functional
CheckCpuid:
; Check for ID bit writeable for "1"
mov ecx,21
pushf          ; In the 64-bit mode, push RFLAGS
pop rax
bts eax,ecx    ; Set EAX.21=1
push rax         
popf           ; Load RFLAGS with RFLAGS.21=1
pushf          ; Store RFLAGS
pop rax        ; Load RFLAGS to RAX
btr eax,ecx    ; Check EAX.21=1, Set EAX.21=0
jnc @f         ; Go error branch if cannot set EFLAGS.21=1
; Check for ID bit writeable for "0"
push rax
popf           ; Load RFLAGS with RFLAGS.21=0
pushf          ; Store RFLAGS
pop rax        ; Load RFLAGS to RAX
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
; Parm#1 = RCX = CPUID function code (means EAX for CPUID instruction)
; Parm#2 = RDX = CPUID subfunction code (means ECX for CPUID instruction) 
; Parm#3 = R8  = Pointer to output DWORD variable = CPUID output EAX  
; Parm#4 = R9  = Pointer to output DWORD variable = CPUID output EBX 
; Parm#5 = [rsp+32+08] = Pointer to output DWORD variable = CPUID output ECX 
; Parm#6 = [rsp+32+16] = Pointer to output DWORD variable = CPUID output EDX
; OUTPUT:
; void 
ExecuteCpuid:
mov eax,ecx
mov ecx,edx
push rbx
cpuid
mov [r8],eax
mov [r9],ebx
mov r8,[rsp+32+08+08]  ; parm.shadow=32, rip size=8, rbx size=8
mov r9,[rsp+32+16+08]  ; parm.shadow=32, rip+1 parm size=16, rbx size=8
mov [r8],ecx
mov [r9],edx
pop rbx
xor eax,eax
ret

; Execute RDTSC instruction, assume support already verified
; INPUT:
; Parm#1 = RCX = Pointer to output QWORD variable = 64-bit TSC value
; OUTPUT:
; void
ExecuteRdtsc:
rdtsc
mov [rcx+0],eax
mov [rcx+4],edx
xor eax,eax
ret 

; Execute XGETBV instruction, assume support already verified
; INPUT:
; Parm#1 = RCX = Pointer to output QWORD variable = XCR0 bitmap
; OUTPUT:
; void
ExecuteXgetbv:
mov r8,rcx
xor ecx,ecx
xgetbv
mov [r8+0],eax
mov [r8+4],edx
xor eax,eax
ret 

; Measure CPU TSC clock, assume support already verified
; INPUT:
; Parm#1 = RCX = Pointer for update output variable, frequency at Hz
; OUTPUT:
; RAX = Reserved for status, return TRUE
MeasureTsc:
push rcx rsi rdi rbp 0     ; 0 as variable storage
mov rbp,rsp                ; RBP used for restore RSP and addressing variables
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32                 ; Make parameters shadow
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get current count
mov rsi,[rbp]
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get next count for wait 100 ns
cmp rsi,[rbp]
je @b
mov rsi,[rbp]
add rsi,10000000                  ; 10^7 * 100ns = 1 second
rdtsc
shl rdx,32
lea rdi,[rax+rdx]                 ; RBX = 64-bit TSC at operation start
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp rsi,[rbp]
ja @b
rdtsc
shl rdx,32
or rax,rdx                        ; RAX = 64-bit TSC at operation end
sub rax,rdi                       ; RAX = Delta TSC
mov rsp,rbp
pop rbp rbp rdi rsi rcx
mov [rcx],rax
mov eax,-1
ret

; Performance patterns caller gate
; Parm#1 = RCX         = Pattern selector 
; Parm#2 = RDX         = Block #1 pointer
; Parm#3 = R8          = Block #2 pointer
; Parm#4 = R9          = Block length, instructions
; Parm#5 = [rsp+32+08] = Measurement repeats
; Parm#6 = [rsp+32+16] = Pointer for update output variable = dTSC
; OUTPUT:
; RAX = Reserved for status, return TRUE
PerformanceGate:
push rbx rsi rdi rbp r12 r13 r14 r15
; Load input parameters, part 1
mov rsi,rdx            ; RSI = Block #1 pointer (64-bit flat)
mov rdi,r8             ; RDI = Block #2 pointer (64-bit flat)
mov rbp,[rsp+32+8+64]  ; RBP = Number of measurement repeats
movzx rcx,cx
mov r14,[PerformancePatterns + rcx*8]
; Serialization, note EAX,EBX,ECX,EDX changed
xor eax,eax        ; EAX=0 means function 0 for CPUID
cpuid              ; This CPUID for serialization only, results ignored
; Load input parameters, part 1
mov rcx,r9         ; RCX = Block length, units = instructions 
mov rbx,r14        ; RBX = Target subroutine address
; Get start time
rdtsc              ; EDX:EAX = TSC value at start: EDX=High, EAX=Low
shl rdx,32         ; Positioning TSC high 32 bits to RDX.[63-32]
lea r14,[rax+rdx]  ; R14 = Interval start TSC value, integral 64 bits
; Call target routine (required not destroy R14, R15)
call rbx           ; Run target operation
; Get end time
rdtsc              ; EDX:EAX = TSC value at end: EDX=High, EAX=Low
shl rdx,32         ; Positioning TSC high 32 bits to RDX.[63-32]
or rax,rdx         ; RAX = Interval end TSC value, integral 64 bits
sub rax,r14        ; RAX = Delta TSC value
push rax
xor eax,eax        ; EAX=0 means function 0 for CPUID
cpuid              ; This CPUID for events serialization only, results ignored
pop rax
pop r15 r14 r13 r12 rbp rdi rsi rbx
; Return with update output
mov rcx,[rsp+32+16]
mov [rcx],rax
mov eax,-1
ret

; Include files connect for performance patterns
; Temporal (cacheable read and write) mode
include 'read_mov64.inc'
include 'write_mov64.inc'
include 'copy_mov64.inc'
include 'modify_not64.inc'
include 'write_stosq.inc'
include 'copy_movsq.inc'
include 'read_mmx64.inc'
include 'write_mmx64.inc'
include 'copy_mmx64.inc'
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
; Latency measurement
include 'latency_lcm.inc'
include 'latency_rdrand.inc'
include 'latency_walk.inc'

; Data section
section '.data' data readable writeable
StringProduct    DB 'MPE native library.',0
StringVersion    DB 'v0.22.00 for Windows x64.',0
StringCopyright  DB '(C)2018 IC Book Labs.',0

; Pointers to performance patterns
align 8
PerformancePatterns:
; Temporal (cacheable read and write) mode
DQ  Pattern_Read_MOV64
DQ  Pattern_Write_MOV64
DQ  Pattern_Copy_MOV64
DQ  Pattern_Modify_NOT64
DQ  Pattern_Write_STOSQ
DQ  Pattern_Copy_MOVSQ
;
DQ  Pattern_Read_MMX64
DQ  Pattern_Write_MMX64
DQ  Pattern_Copy_MMX64
;
DQ  Pattern_Read_SSE128
DQ  Pattern_Write_SSE128
DQ  Pattern_Copy_SSE128
DQ  Pattern_Read_AVX256
DQ  Pattern_Write_AVX256
DQ  Pattern_Copy_AVX256
DQ  Pattern_Read_AVX512
DQ  Pattern_Write_AVX512
DQ  Pattern_Copy_AVX512
DQ  Pattern_Dot_FMA256  
DQ  Pattern_Dot_FMA512  
; Non-temporal (non-cacheable write) mode 
DQ  Pattern_NtWrite_SSE128
DQ  Pattern_NtCopy_SSE128   ; This duplicated 1
DQ  Pattern_NtWrite_AVX256
DQ  Pattern_NtCopy_AVX256
DQ  Pattern_NtWrite_AVX512
DQ  Pattern_NtCopy_AVX512
; Non-temporal (non-cacheable read and write) mode
DQ  Pattern_NtRead_SSE128
DQ  Pattern_NtRCopy_SSE128
DQ  Pattern_NtRead_AVX256
DQ  Pattern_NtRCopy_AVX256
DQ  Pattern_NtRead_AVX512   
DQ  Pattern_NtRCopy_AVX512
; Non-temporal (non-cacheable read-by-prefetch and write) mode
DQ  Pattern_NtpRead_SSE128
DQ  Pattern_NtCopy_SSE128   ; This duplicated 1
DQ  Pattern_NtpRead_AVX256
; Reserved for same 256-512 bit operations
; FMA with non-temporal store
; ... reserved ...
; Latency measurement
DQ  Pattern_Latency_LCM
DQ  Pattern_Latency_RDRAND  
DQ  Pattern_Latency_Walk   

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
