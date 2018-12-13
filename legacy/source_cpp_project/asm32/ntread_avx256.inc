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

Pattern_NtRead_AVX256:
Measurement_NtRead_AVX256:
; Prepare big cycle
; Set pointer to middle of 512-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea eax,[esi+256]            ; EAX = Reload source address
mov edx,ecx                  ; EDX = Reload length
shr edx,4                    ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_NtRead_AVX256       ; Go if Length < 16 instructions
mov ebx,512                  ; EBX = Addend, this operation also clear bits ebx[63-32]
; Big cycle
DumpStart_NtRead_AVX256:
Block_NtRead_AVX256:
vmovntdqa ymm0,[eax-32*08]
vmovntdqa ymm1,[eax-32*07]
vmovntdqa ymm2,[eax-32*06]
vmovntdqa ymm3,[eax-32*05]
vmovntdqa ymm4,[eax-32*04]
vmovntdqa ymm5,[eax-32*03]
vmovntdqa ymm6,[eax-32*02]
vmovntdqa ymm7,[eax-32*01]
vmovntdqa ymm0,[eax+32*00]
vmovntdqa ymm1,[eax+32*01]
vmovntdqa ymm2,[eax+32*02]
vmovntdqa ymm3,[eax+32*03]
vmovntdqa ymm4,[eax+32*04]
vmovntdqa ymm5,[eax+32*05]
vmovntdqa ymm6,[eax+32*06]
vmovntdqa ymm7,[eax+32*07]
add eax,ebx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Block_NtRead_AVX256      ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_NtRead_AVX256:
; Prepare tail cycle
Small_NtRead_AVX256:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_NtRead_AVX256
mov ebx,32
; Tail cycle
Tail_NtRead_AVX256:
vmovntdqa ymm0,[eax-32*08]
add eax,ebx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Tail_NtRead_AVX256       ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_NtRead_AVX256:
dec ebp
jnz Measurement_NtRead_AVX256  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret
