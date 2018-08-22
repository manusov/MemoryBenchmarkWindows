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

Pattern_NtWrite_SSE128:
Measurement_NtWrite_SSE128:
; Prepare big cycle
; Set pointer to middle of 256-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea eax,[esi+128]               ; EAX = Reload source address
mov edx,ecx                     ; EDX = Reload length
shr edx,4                       ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_NtWrite_SSE128         ; Go if Length < 16 instructions
mov ebx,256                     ; EBX = Addend
; Big cycle
DumpStart_NtWrite_SSE128:
Block_NtWrite_SSE128:
movntps [eax-16*08],xmm0
movntps [eax-16*07],xmm1
movntps [eax-16*06],xmm2
movntps [eax-16*05],xmm3
movntps [eax-16*04],xmm4
movntps [eax-16*03],xmm5
movntps [eax-16*02],xmm6
movntps [eax-16*01],xmm7
movntps [eax+16*00],xmm0
movntps [eax+16*01],xmm1
movntps [eax+16*02],xmm2
movntps [eax+16*03],xmm3
movntps [eax+16*04],xmm4
movntps [eax+16*05],xmm5
movntps [eax+16*06],xmm6
movntps [eax+16*07],xmm7
add eax,ebx                     ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Block_NtWrite_SSE128        ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_NtWrite_SSE128:
; Prepare tail cycle
Small_NtWrite_SSE128:
mov edx,ecx
and edx,00001111b               ; ECX = Extract tail length
jz Measure_NtWrite_SSE128
mov ebx,16
; Tail cycle
Tail_NtWrite_SSE128:
movntps [eax-16*08],xmm0
add eax,ebx                     ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Tail_NtWrite_SSE128         ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_NtWrite_SSE128:
dec ebp
jnz Measurement_NtWrite_SSE128  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret

