include 'win32a.inc'
format PE DLL
entry  DllMain

section '.text' code readable executable

; System subroutine
DllMain:
mov eax,1        ; EAX = 1 means call when start process
ret 12

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
ret 12

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
ret 24

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
ret 4

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
ret 4

; Measure CPU TSC clock, assume support already verified
; INPUT:
; Parm#1 = [ESP+04] = Pointer for update output variable, frequency at Hz
; OUTPUT:
; EAX = Status, return TRUE if OK, FALSE if error.
MeasureTsc:
push ebx esi edi ebp ebp ebp   ; Last 2 pushes = reserve space for local variables
mov ebp,esp                    ; EBP = pointer to local variables
mov edi,[esp + 24 + 4]         ; EDI = first parameter, pointer to output
; Start measure frequency, get current change
push ebp                          ; Parm#1 = pointer to output 64-bit variable
call [GetSystemTimeAsFileTime]    ; Get current count
mov esi,[ebp]
@@:                               ; Wait for start 1 second interval
push ebp
call [GetSystemTimeAsFileTime]    ; Get next count for wait 100 ns
cmp esi,[ebp]
je @b
mov esi,[ebp + 0]                 ; Set time interval = 1 second
mov ebx,[ebp + 4]
add esi,10000000                  ; 10^7 * 100ns = 1 second
adc ebx,0
rdtsc                             ; Get start TSC
push edx eax
@@:                               ; Wait for end 1 second interval
push ebp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp [ebp + 4],ebx
jb @b
ja @f
cmp [ebp + 0],esi
jb @b
@@:
rdtsc           ; Get end TSC, calculate delta-TSC
pop esi ebx
sub eax,esi
sbb edx,ebx     ; EDX:EAX = Delta TSC per 1 second = frequency, Hz
jb .error       ; Go error if frequency < 0               
mov ecx,eax
or ecx,edx
jz .error       ; Go error if frequency = 0
; Store Frequency, as 64-bit integer value, Hz, delta-TSC per second
cld
stosd           ; Store EAX = Frequency, low dword
xchg eax,edx
stosd           ; Store EAX = Frequency, high dword
; Pop extra registers, exit
mov eax,1     ; EAX = 1 means CPU clock measured OK
.exit:
pop ebp ebp ebp edi esi ebx
ret 4
.error:
xor eax,eax   ; RAX = 0 means CPU clock measured ERROR
jmp .exit

; Measure CPU TSC clock by OS performance counter,
; assume TSC support already verified
; INPUT:
; Parm#1   = dword [ESP+04] =
;            Pointer for update output variable, frequency at Hz
; Parm#2   = dword [ESP+08] =
;            Pointer to dyn. imported function QueryPerformanceCounter
; Parm#3,4 = dword [ESP+12], dword [ESP+16] =
;            Number of counter ticks for wait 1 second
; OUTPUT:
; EAX = Status, return TRUE if OK, FALSE if error.
MeasureTscByPcounter:
push ebx esi edi ebp ebp ebp   ; Last 2 pushes = reserve space for local variables
mov ebp,esp                    ; EBP = pointer to local variables
mov edi,[esp + 24 + 4]         ; EDI = first parameter, pointer to output
; Start measure frequency, get current change
push ebp                          ; Parm#1 = pointer to output 64-bit variable
call dword [esp + 24 + 8 + 4]     ; Get current count
mov esi,[ebp]
@@:                               ; Wait for start 1 second interval
push ebp
call dword [esp + 24 + 8 + 4]     ; Get next count for wait Tqp. TODO. Check status.
cmp esi,[ebp]
je @b
mov esi,[ebp + 0]                 ; Set time interval = 1 second
mov ebx,[ebp + 4]
add esi,[esp + 24 + 12]           ; N * Tqp = 1 second
adc ebx,[esp + 24 + 16]
rdtsc                             ; Get start TSC
push edx eax
@@:                               ; Wait for end 1 second interval
push ebp
call dword [esp + 24 + 8 + 12]    ; Get count for wait 1 second. TODO. Check status.
cmp [ebp + 4],ebx
jb @b
ja @f
cmp [ebp + 0],esi
jb @b
@@:
rdtsc           ; Get end TSC, calculate delta-TSC
pop esi ebx
sub eax,esi
sbb edx,ebx     ; EDX:EAX = Delta TSC per 1 second = frequency, Hz
jb .error       ; Go error if frequency < 0               
mov ecx,eax
or ecx,edx
jz .error       ; Go error if frequency = 0
; Store Frequency, as 64-bit integer value, Hz, delta-TSC per second
cld
stosd           ; Store EAX = Frequency, low dword
xchg eax,edx
stosd           ; Store EAX = Frequency, high dword
; Pop extra registers, exit
mov eax,1     ; EAX = 1 means CPU clock measured OK
.exit:
pop ebp ebp ebp edi esi ebx
ret 16
.error:
xor eax,eax   ; EAX = 0 means CPU clock measured ERROR
jmp .exit

; Performance patterns caller gate
; Parm#1 = dword [ESP+04] = Pattern selector 
; Parm#2 = dword [ESP+08] = Block #1 pointer
; Parm#3 = dword [ESP+12] = Block #2 pointer
; Parm#4 = dword [ESP+16] = Block length, instructions
; Parm#5 = dword [ESP+20] = Measurement repeats, Low 32 bits
; Parm#5 = dword [ESP+24] = Measurement repeats, High 32 bits
; Parm#6 = dword [ESP+28] = Pointer for update output variable = dTSC
; OUTPUT:
; EAX = Reserved for status, return TRUE
PerformanceGate:
push ebx esi edi ebp
; Load input parameters, part 1
mov esi,[esp + 08 + 16]                ; ESI = Block #1 pointer (32-bit flat)
mov edi,[esp + 12 + 16]                ; EDI = Block #2 pointer (32-bit flat)
mov ebp,[esp + 20 + 16]                ; EBP = Measurement repeats, Low 32 bits 
; This CPUID for events serialization only, results ignored
xor eax,eax                            ; EAX=0 means function 0 for CPUID
cpuid                                  ; This CPUID for serialization only
; Load input parameters, part 2
movzx ebx,word [esp + 04 + 16]         ; EBX = Pattern selector
mov ebx,[PerformancePatterns + ebx*4]  ; EBX = Target routine entry point
mov ecx,[esp + 16 + 16]                ; ECX = Block length, units=instructions 
; Get start time
rdtsc
push eax edx
; Call target routine
mov eax,[esp + 24 + 16 + 08]         ; EAX = Measurement repeats, High 32 bits
xchg eax,ebx                         ; EAX = Call address, EBX = Repeats, High
call eax
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
mov ecx,[esp + 28]
mov [ecx + 00],eax
mov [ecx + 04],edx
mov eax,-1
ret 28

; Performance patterns
READ_MOV_32:
include '..\asm\ncrb32\memory_bandwidth_temporal\read_mov32.inc'     ; Read by MOV (32 bit)
WRITE_MOV_32:
include '..\asm\ncrb32\memory_bandwidth_temporal\write_mov32.inc'    ; Write by MOV (32 bit)
COPY_MOV_32:
include '..\asm\ncrb32\memory_bandwidth_temporal\copy_mov32.inc'     ; Copy by MOV + MOV (32 bit)
MODIFY_NOT_32:
include '..\asm\ncrb32\memory_bandwidth_temporal\modify_not32.inc'   ; Modify = Read-Modify-Write by NOT (32 bit)
WRITE_STRINGS_32:
include '..\asm\ncrb32\memory_bandwidth_temporal\write_stosd.inc'    ; Write by REP STOSQ (32 bit)
READ_STRINGS_32:
include '..\asm\ncrb32\memory_bandwidth_temporal\copy_movsd.inc'     ; Copy by REP MOVSQ (32 bit)
READ_MMX_64:
include '..\asm\ncrb32\memory_bandwidth_temporal\read_mmx64.inc'     ; Read by MOVQ (64 bit)
WRITE_MMX_64:
include '..\asm\ncrb32\memory_bandwidth_temporal\write_mmx64.inc'    ; Write by MOVQ (64 bit)
COPY_MMX_64:
include '..\asm\ncrb32\memory_bandwidth_temporal\copy_mmx64.inc'     ; Copy by MOVQ + MOVQ (64 bit)
READ_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_temporal\read_sse128.inc'    ; Read by MOVAPS (128 bit)
WRITE_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_temporal\write_sse128.inc'   ; Write by MOVAPS (128 bit)
COPY_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_temporal\copy_sse128.inc'    ; Copy by MOVAPS + MOVAPS (128 bit)
READ_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_temporal\read_avx256.inc'    ; Read by VMOVAPD (256 bit)
WRITE_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_temporal\write_avx256.inc'   ; Write by VMOVAPD (256 bit)
COPY_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_temporal\copy_avx256.inc'    ; Copy by VMOVAPD + VMOVAPD (256 bit)
READ_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_temporal\read_avx512.inc'    ; Read by VMOVAPD (512 bit)
WRITE_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_temporal\write_avx512.inc'   ; Write by VMOVAPD (512 bit)
COPY_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_temporal\copy_avx512.inc'    ; Copy by VMOVAPD + VMOVAPD (512 bit)
DOT_FMA_256:
include '..\asm\ncrb32\memory_bandwidth_temporal\dot_fma256.inc'     ; Fused multiply and add, VFMADD231PD (256 bit)
DOT_FMA_512:
include '..\asm\ncrb32\memory_bandwidth_temporal\dot_fma512.inc'     ; Fused multiply and add, VFMADD231PD (512 bit)
GATHER_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_temporal\gather_avx256.inc'  ; Gather Read by VGATHERQPD (256 bit)
GATHER_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_temporal\gather_avx512.inc'  ; Gather Read by VGATHERQPD (512 bit)
SCATTER_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_temporal\scatter_avx512.inc' ; Scatter Write by VSCATTERQPD (512 bit)
WRITE_CLZERO:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntwrite_clzero.inc' ; Write by CLZERO (AMD)

;========== DEBUG PATCH =======================================================;
;
LATENCY_BUILD_LCM:
include '..\asm\ncrb32\memory_latency\latency_lcm_32.inc'       ; Build walk list, use linear congruental method
LATENCY_BUILD_LCM_32X2:
include '..\asm\ncrb32\memory_latency\latency_lcm_32x2.inc'     ; Build walk list, use linear congruental method
LATENCY_BUILD_RDRAND: 
include '..\asm\ncrb32\memory_latency\latency_rdrand_32.inc'    ; Build walk list, use RDRAND instruction
LATENCY_BUILD_RDRAND_32X2:
include '..\asm\ncrb32\memory_latency\latency_rdrand_32x2.inc'  ; Build walk list, use RDRAND instruction
LATENCY_WALK:
include '..\asm\ncrb32\memory_latency\latency_walk_32.inc'      ; Walk on pre-builded walk list
LATENCY_WALK_32X2:
include '..\asm\ncrb32\memory_latency\latency_walk_32x2.inc'    ; Walk on pre-builded walk list
;
;========== DEBUG PATCH =======================================================;
;
;LATENCY_BUILD_LCM:
;LATENCY_BUILD_LCM_32X2:
;include '..\asm_old\32\latency_lcm.inc'     ; Build walk list, use linear congruental method
;
;LATENCY_BUILD_RDRAND: 
;LATENCY_BUILD_RDRAND_32X2:
;include '..\asm_old\32\latency_rdrand.inc'  ; Build walk list, use RDRAND instruction
;
;LATENCY_WALK:
;LATENCY_WALK_32X2:
;include '..\asm_old\32\latency_walk.inc'    ; Walk on pre-builded walk list
;
;========== DEBUG PATCH =======================================================;

NT_READ_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntread_sse128.inc'    ; Read by MOVNTDQA (128 bit)
NTP_READ_SSE_128_DEFAULT_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_sse128.inc'   ; Read by PREFETCHNTA + MOVAPS (128 bit)
NTP_READ_SSE_128_MEDIUM_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_medium_sse128.inc'   ; Same read with prefetch distance + 512
NTP_READ_SSE_128_LONG_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_long_sse128.inc'     ; Same read with prefetch distance + 2048
NT_WRITE_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntwrite_sse128.inc'   ; Write by MOVNTPS (128 bit)
NT_COPY_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntcopy_sse128.inc'    ; Copy by PREFETCHNTA + MOVAPS + MOVNTPS (128 bit)
NTR_COPY_SSE_128:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntrcopy_sse128.inc'   ; Copy by MOVNTDQA + MOVNTPD (128 bit)
NT_READ_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntread_avx256.inc'    ; Read by VMOVNTDQA (256 bit)
NTP_READ_AVX_256_DEFAULT_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_avx256.inc'   ; Read by PREFETCHNTA + VMOVAPD (256 bit)
NTP_READ_AVX_256_MEDIUM_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_medium_avx256.inc'   ; Same read with prefetch distance + 512
NTP_READ_AVX_256_LONG_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_long_avx256.inc'     ; Same read with prefetch distance + 2048
NT_WRITE_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntwrite_avx256.inc'   ; Write by VMOVNTPD (256 bit)
NT_COPY_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntcopy_avx256.inc'    ; Copy by MOVAPD + MOVNTPD (256 bit)
NTR_COPY_AVX_256:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntrcopy_avx256.inc'   ; Copy by VMOVNTDQA + VMOVNTPD (256 bit)
NT_READ_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntread_avx512.inc'    ; Read by VMOVNTDQA (512 bit)
NTP_READ_AVX_512_DEFAULT_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_avx512.inc'   ; Read by PREFETCHNTA + VMOVAPD (512 bit)
NTP_READ_AVX_512_MEDIUM_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_medium_avx512.inc'   ; Same read with prefetch distance + 512
NTP_READ_AVX_512_LONG_DISTANCE:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntpread_long_avx512.inc'     ; Same read with prefetch distance + 2048
NT_WRITE_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntwrite_avx512.inc'   ; Write by VMOVNTPD (512 bit) 
NT_COPY_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntcopy_avx512.inc'    ; Copy by MOVAPD + MOVNTPD (512 bit)
NTR_COPY_AVX_512:
include '..\asm\ncrb32\memory_bandwidth_non_temporal\ntrcopy_avx512.inc'   ; Copy by VMOVNTDQA + VMOVNTPD (512 bit)

section '.data' data readable writeable
align 8
PerformancePatterns:
DD  READ_MOV_32
DD  WRITE_MOV_32
DD  COPY_MOV_32
DD  MODIFY_NOT_32
DD  WRITE_STRINGS_32
DD  READ_STRINGS_32
DD  READ_MMX_64
DD  WRITE_MMX_64
DD  COPY_MMX_64
DD  READ_SSE_128
DD  WRITE_SSE_128
DD  COPY_SSE_128
DD  READ_AVX_256
DD  WRITE_AVX_256
DD  COPY_AVX_256
DD  READ_AVX_512
DD  WRITE_AVX_512
DD  COPY_AVX_512
DD  DOT_FMA_256
DD  DOT_FMA_512
DD  GATHER_AVX_256
DD  GATHER_AVX_512
DD  SCATTER_AVX_512
DD  WRITE_CLZERO
DD  LATENCY_BUILD_LCM
DD  LATENCY_BUILD_LCM_32X2
DD  LATENCY_BUILD_RDRAND
DD  LATENCY_BUILD_RDRAND_32X2 
DD  LATENCY_WALK
DD  LATENCY_WALK_32X2
DD  NT_READ_SSE_128
DD  NTP_READ_SSE_128_DEFAULT_DISTANCE
DD  NTP_READ_SSE_128_MEDIUM_DISTANCE
DD  NTP_READ_SSE_128_LONG_DISTANCE
DD  NT_WRITE_SSE_128
DD  NT_COPY_SSE_128
DD  NTR_COPY_SSE_128
DD  NT_READ_AVX_256
DD  NTP_READ_AVX_256_DEFAULT_DISTANCE
DD  NTP_READ_AVX_256_MEDIUM_DISTANCE
DD  NTP_READ_AVX_256_LONG_DISTANCE
DD  NT_WRITE_AVX_256
DD  NT_COPY_AVX_256
DD  NTR_COPY_AVX_256
DD  NT_READ_AVX_512
DD  NTP_READ_AVX_512_DEFAULT_DISTANCE
DD  NTP_READ_AVX_512_MEDIUM_DISTANCE
DD  NTP_READ_AVX_512_LONG_DISTANCE
DD  NT_WRITE_AVX_512
DD  NT_COPY_AVX_512
DD  NTR_COPY_AVX_512

StringProduct    DB 'NCRB performance library.',0
StringVersion    DB 'v0.02.01 for Windows ia32.',0
StringCopyright  DB '(C) 2023 Ilya Manusov.',0

section '.edata' export data readable
export  'MPE32.DLL'                           ,\
GetDllStrings        , 'GetDllStrings'        ,\
CheckCpuid           , 'CheckCpuid'           ,\
ExecuteCpuid         , 'ExecuteCpuid'         ,\
ExecuteRdtsc         , 'ExecuteRdtsc'         ,\
ExecuteXgetbv        , 'ExecuteXgetbv'        ,\
MeasureTsc           , 'MeasureTsc'           ,\
MeasureTscByPcounter , 'MeasureTscByPcounter' ,\
PerformanceGate      , 'PerformanceGate'  

section '.idata' import data readable
library kernel32 , 'KERNEL32.DLL'
include 'api\kernel32.inc'

data fixups
end data

