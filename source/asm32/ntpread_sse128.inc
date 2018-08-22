;---------- Memory performance patterns ---------------------------------------;
; INPUT:   ESI = Block #1 pointer (32-bit flat)                                ;
;          EDI = Block #2 pointer (32-bit flat)                                ;
;                For Read, Write, Modify use ESI as Source or Destination      ;
;                For Copy use ESI = Source , EDI = Destination                 ;
;          ECX = Block length, units = instructions                            ;
;          EBP = Number of measurement repeats                                 ;
; OUTPUT:  None                                                                ;
;          All registers corrupted                                             ;
;------------------------------------------------------------------------------;

Pattern_NtpRead_SSE128:
Measurement_NtpRead_SSE128:
; Prepare big cycle
; Set pointer to middle of 256-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea eax,[esi+128]            ; EAX = Reload source address
mov edx,ecx                  ; EDX = Reload length
shr edx,4                    ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_NtpRead_SSE128      ; Go if Length < 16 instructions
mov ebx,256                  ; EBX = Addend
; Big cycle                  ; 256 bytes per cycle with 16 128-bit SSE registers
DumpStart_NtpRead_SSE128:
Block_NtpRead_SSE128:
; patch v0.98.1
cmp edx,1
je @f
prefetchnta [eax-16*08+ebx]
prefetchnta [eax-16*04+ebx]
prefetchnta [eax+16*00+ebx]
prefetchnta [eax+16*04+ebx]
@@:
; end of patch v0.98.1
movapd xmm0,[eax-16*08]
movapd xmm1,[eax-16*07]
movapd xmm2,[eax-16*06]
movapd xmm3,[eax-16*05]
movapd xmm4,[eax-16*04]
movapd xmm5,[eax-16*03]
movapd xmm6,[eax-16*02]
movapd xmm7,[eax-16*01]
movapd xmm0,[eax+16*00]
movapd xmm1,[eax+16*01]
movapd xmm2,[eax+16*02]
movapd xmm3,[eax+16*03]
movapd xmm4,[eax+16*04]
movapd xmm5,[eax+16*05]
movapd xmm6,[eax+16*06]
movapd xmm7,[eax+16*07]
add eax,ebx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Block_NtpRead_SSE128     ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_NtpRead_SSE128:
; Prepare tail cycle
Small_NtpRead_SSE128:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_NtpRead_SSE128
mov ebx,16                   ; 16 bytes per cycle with one 128-bit SSE register
; Tail cycle
Tail_NtpRead_SSE128:
movapd xmm0,[eax-16*08]
add eax,ebx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Tail_NtpRead_SSE128      ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_NtpRead_SSE128:
dec ebp
jnz Measurement_NtpRead_SSE128  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret
