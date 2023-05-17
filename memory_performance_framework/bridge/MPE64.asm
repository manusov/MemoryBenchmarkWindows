include 'win64a.inc'
format PE64 DLL
entry  DllMain

section '.text' code readable executable

; System subroutine
DllMain:
mov eax,1        ; RAX = 1 means call when start process
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
mov r8,[rsp + 32 + 08 + 08]  ; parm.shadow=32, rip size=8, rbx size=8
mov r9,[rsp + 32 + 16 + 08]  ; parm.shadow=32, rip+1 parm size=16, rbx size=8
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
mov [rcx + 0],eax
mov [rcx + 4],edx
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
mov [r8 + 0],eax
mov [r8 + 4],edx
xor eax,eax
ret 

; Measure CPU TSC clock, assume support already verified
; INPUT:
; Parm#1 = RCX = Pointer for update output variable, frequency at Hz
; OUTPUT:
; RAX = Status, return TRUE if OK, FALSE if error.
MeasureTsc:
push rbx rsi rdi rbp rbp    ; Last push for reserve local variable space
mov rbp,rsp                 ; RBP used for restore RSP and addressing variables
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32                  ; Make parameters shadow
mov rdi,rcx                 ; Save output variable address
; Start measure frequency
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
lea rbx,[rax + rdx]               ; RBX = 64-bit TSC at operation start
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp rsi,[rbp]
ja @b
rdtsc
shl rdx,32
or rax,rdx    ; RAX = 64-bit TSC at operation end
sub rax,rbx   ; RAX = Delta TSC
jbe .error
; Store result 
cld
stosq   ; Store Frequency, as 64-bit integer value, Hz, delta-TSC per second
; Restore RSP, pop extra registers, exit
mov eax,1     ; RAX = 1 means CPU clock measured OK
.exit:
mov rsp,rbp
pop rbp rbp rdi rsi rbx
ret
.error:
xor eax,eax   ; RAX = 0 means CPU clock measured ERROR
jmp .exit

; Performance patterns caller gate
; Parm#1 = RCX                   = Pattern selector 
; Parm#2 = RDX                   = Block #1 pointer
; Parm#3 = R8                    = Block #2 pointer
; Parm#4 = R9                    = Block length, instructions
; Parm#5 = qword [rsp + 32 + 08] = Measurement repeats, for x64 here High:Low
; Parm#6 = qword [rsp + 32 + 16] = Measurement repeats, High, for x64 not used 
; Parm#7 = qword [rsp + 32 + 24] = Pointer for update output variable = dTSC
; OUTPUT:
; RAX = Reserved for status, return TRUE
PerformanceGate:
push rbx rsi rdi rbp r12 r13 r14 r15
; Load input parameters, part 1
mov rsi,rdx                  ; RSI = Block #1 pointer (64-bit flat)
mov rdi,r8                   ; RDI = Block #2 pointer (64-bit flat)
mov rbp,[rsp + 32 + 8 + 64]  ; RBP = Number of measurement repeats
movzx rcx,cx
mov r14,[PerformancePatterns + rcx*8]
; Serialization, note EAX,EBX,ECX,EDX changed
xor eax,eax          ; EAX=0 means function 0 for CPUID
cpuid                ; This CPUID for serialization only, results ignored
; Load input parameters, part 1
mov rcx,r9           ; RCX = Block length, units = instructions 
mov rbx,r14          ; RBX = Target subroutine address
; Get start time
rdtsc                ; EDX:EAX = TSC value at start: EDX=High, EAX=Low
shl rdx,32           ; Positioning TSC high 32 bits to RDX.[63-32]
lea r14,[rax + rdx]  ; R14 = Interval start TSC value, integral 64 bits
; Call target routine (required not destroy R14, R15)
call rbx             ; Run target operation
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
mov rcx,[rsp + 32 + 24]
mov [rcx],rax
mov eax,-1
ret

; Performance patterns
READ_MOV_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\read_mov64.inc'     ; Read by MOV (64 bit)
WRITE_MOV_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\write_mov64.inc'    ; Write by MOV (64 bit)
COPY_MOV_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\copy_mov64.inc'     ; Copy by MOV + MOV (64 bit)
MODIFY_NOT_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\modify_not64.inc'   ; Modify = Read-Modify-Write by NOT (64 bit)
WRITE_STRINGS_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\write_stosq.inc'    ; Write by REP STOSQ (64 bit)
READ_STRINGS_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\copy_movsq.inc'     ; Copy by REP MOVSQ (64 bit)
READ_MMX_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\read_mmx64.inc'     ; Read by MOVQ (64 bit)
WRITE_MMX_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\write_mmx64.inc'    ; Write by MOVQ (64 bit)
COPY_MMX_64:
include '..\asm\ncrb64\memory_bandwidth_temporal\copy_mmx64.inc'     ; Copy by MOVQ + MOVQ (64 bit)
READ_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_temporal\read_sse128.inc'    ; Read by MOVAPS (128 bit)
WRITE_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_temporal\write_sse128.inc'   ; Write by MOVAPS (128 bit)
COPY_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_temporal\copy_sse128.inc'    ; Copy by MOVAPS + MOVAPS (128 bit)
READ_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_temporal\read_avx256.inc'    ; Read by VMOVAPD (256 bit)
WRITE_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_temporal\write_avx256.inc'   ; Write by VMOVAPD (256 bit)
COPY_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_temporal\copy_avx256.inc'    ; Copy by VMOVAPD + VMOVAPD (256 bit)
READ_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_temporal\read_avx512.inc'    ; Read by VMOVAPD (512 bit)
WRITE_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_temporal\write_avx512.inc'   ; Write by VMOVAPD (512 bit)
COPY_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_temporal\copy_avx512.inc'    ; Copy by VMOVAPD + VMOVAPD (512 bit)
DOT_FMA_256:
include '..\asm\ncrb64\memory_bandwidth_temporal\dot_fma256.inc'     ; Fused multiply and add, VFMADD231PD (256 bit)
DOT_FMA_512:
include '..\asm\ncrb64\memory_bandwidth_temporal\dot_fma512.inc'     ; Fused multiply and add, VFMADD231PD (512 bit)
GATHER_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_temporal\gather_avx256.inc'  ; Gather Read by VGATHERQPD (256 bit)
GATHER_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_temporal\gather_avx512.inc'  ; Gather Read by VGATHERQPD (512 bit)
SCATTER_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_temporal\scatter_avx512.inc' ; Scatter Write by VSCATTERQPD (512 bit)
WRITE_CLZERO:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntwrite_clzero.inc' ; Write by CLZERO (AMD)
LATENCY_BUILD_LCM:
LATENCY_BUILD_LCM_32X2:                                     ; Same branch for x64 version
include '..\asm\ncrb64\memory_latency\latency_lcm.inc'      ; Build walk list, use linear congruental method
LATENCY_BUILD_RDRAND: 
LATENCY_BUILD_RDRAND_32X2:                                  ; Same branch for x64 version
include '..\asm\ncrb64\memory_latency\latency_rdrand.inc'   ; Build walk list, use RDRAND instruction
LATENCY_WALK:
LATENCY_WALK_32X2:                                          ; Same branch for x64 version
include '..\asm\ncrb64\memory_latency\latency_walk.inc'     ; Walk on pre-builded walk list
NT_READ_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntread_sse128.inc'    ; Read by MOVNTDQA (128 bit)
NTP_READ_SSE_128_DEFAULT_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_sse128.inc'   ; Read by PREFETCHNTA + MOVAPS (128 bit)
NTP_READ_SSE_128_MEDIUM_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_medium_sse128.inc'   ; Same read with prefetch distance + 512
NTP_READ_SSE_128_LONG_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_long_sse128.inc'     ; Same read with prefetch distance + 2048
NT_WRITE_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntwrite_sse128.inc'   ; Write by MOVNTPS (128 bit)
NT_COPY_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntcopy_sse128.inc'    ; Copy by PREFETCHNTA + MOVAPS + MOVNTPS (128 bit)
NTR_COPY_SSE_128:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntrcopy_sse128.inc'   ; Copy by MOVNTDQA + MOVNTPD (128 bit)
NT_READ_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntread_avx256.inc'    ; Read by VMOVNTDQA (256 bit)
NTP_READ_AVX_256_DEFAULT_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_avx256.inc'   ; Read by PREFETCHNTA + VMOVAPD (256 bit)
NTP_READ_AVX_256_MEDIUM_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_medium_avx256.inc'   ; Same read with prefetch distance + 512
NTP_READ_AVX_256_LONG_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_long_avx256.inc'     ; Same read with prefetch distance + 2048
NT_WRITE_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntwrite_avx256.inc'   ; Write by VMOVNTPD (256 bit)
NT_COPY_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntcopy_avx256.inc'    ; Copy by MOVAPD + MOVNTPD (256 bit)
NTR_COPY_AVX_256:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntrcopy_avx256.inc'   ; Copy by VMOVNTDQA + VMOVNTPD (256 bit)
NT_READ_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntread_avx512.inc'    ; Read by VMOVNTDQA (512 bit)
NTP_READ_AVX_512_DEFAULT_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_avx512.inc'   ; Read by PREFETCHNTA + VMOVAPD (512 bit)
NTP_READ_AVX_512_MEDIUM_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_medium_avx512.inc'   ; Same read with prefetch distance + 512
NTP_READ_AVX_512_LONG_DISTANCE:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntpread_long_avx512.inc'     ; Same read with prefetch distance + 2048
NT_WRITE_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntwrite_avx512.inc'   ; Write by VMOVNTPD (512 bit) 
NT_COPY_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntcopy_avx512.inc'    ; Copy by MOVAPD + MOVNTPD (512 bit)
NTR_COPY_AVX_512:
include '..\asm\ncrb64\memory_bandwidth_non_temporal\ntrcopy_avx512.inc'   ; Copy by VMOVNTDQA + VMOVNTPD (512 bit)

section '.data' data readable writeable
align 8
PerformancePatterns:
DQ  READ_MOV_64
DQ  WRITE_MOV_64
DQ  COPY_MOV_64
DQ  MODIFY_NOT_64
DQ  WRITE_STRINGS_64
DQ  READ_STRINGS_64
DQ  READ_MMX_64
DQ  WRITE_MMX_64
DQ  COPY_MMX_64
DQ  READ_SSE_128
DQ  WRITE_SSE_128
DQ  COPY_SSE_128
DQ  READ_AVX_256
DQ  WRITE_AVX_256
DQ  COPY_AVX_256
DQ  READ_AVX_512
DQ  WRITE_AVX_512
DQ  COPY_AVX_512
DQ  DOT_FMA_256
DQ  DOT_FMA_512
DQ  GATHER_AVX_256
DQ  GATHER_AVX_512
DQ  SCATTER_AVX_512
DQ  WRITE_CLZERO
DQ  LATENCY_BUILD_LCM
DQ  LATENCY_BUILD_LCM_32X2
DQ  LATENCY_BUILD_RDRAND
DQ  LATENCY_BUILD_RDRAND_32X2
DQ  LATENCY_WALK
DQ  LATENCY_WALK_32X2
DQ  NT_READ_SSE_128
DQ  NTP_READ_SSE_128_DEFAULT_DISTANCE
DQ  NTP_READ_SSE_128_MEDIUM_DISTANCE
DQ  NTP_READ_SSE_128_LONG_DISTANCE
DQ  NT_WRITE_SSE_128
DQ  NT_COPY_SSE_128
DQ  NTR_COPY_SSE_128
DQ  NT_READ_AVX_256
DQ  NTP_READ_AVX_256_DEFAULT_DISTANCE
DQ  NTP_READ_AVX_256_MEDIUM_DISTANCE
DQ  NTP_READ_AVX_256_LONG_DISTANCE
DQ  NT_WRITE_AVX_256
DQ  NT_COPY_AVX_256
DQ  NTR_COPY_AVX_256
DQ  NT_READ_AVX_512
DQ  NTP_READ_AVX_512_DEFAULT_DISTANCE
DQ  NTP_READ_AVX_512_MEDIUM_DISTANCE
DQ  NTP_READ_AVX_512_LONG_DISTANCE
DQ  NT_WRITE_AVX_512
DQ  NT_COPY_AVX_512
DQ  NTR_COPY_AVX_512

StringProduct    DB 'NCRB performance library.',0
StringVersion    DB 'v0.01.02 for Windows x64.',0
StringCopyright  DB '(C) 2023 Ilya Manusov.',0

section '.edata' export data readable
export  'test1.dll'                 ,\
GetDllStrings   , 'GetDllStrings'   ,\
CheckCpuid      , 'CheckCpuid'      ,\
ExecuteCpuid    , 'ExecuteCpuid'    ,\
ExecuteRdtsc    , 'ExecuteRdtsc'    ,\
ExecuteXgetbv   , 'ExecuteXgetbv'   ,\
MeasureTsc      , 'MeasureTsc'      ,\
PerformanceGate , 'PerformanceGate'  

section '.idata' import data readable
library kernel32 , 'KERNEL32.DLL'
include 'api\kernel32.inc'

data fixups
end data
