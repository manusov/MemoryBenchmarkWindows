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

Pattern_Write_AVX256:
Measurement_Write_AVX256:
; Prepare big cycle
; Set pointer to middle of 512-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea eax,[esi+256]            ; EAX = Reload source address
mov edx,ecx                  ; EDX = Reload length
shr edx,4                    ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_Write_AVX256        ; Go if Length < 16 instructions
mov ebx,512                  ; EBX = Addend
; Big cycle
DumpStart_Write_AVX256:
Block_Write_AVX256:
vmovapd [eax-32*08],ymm0
vmovapd [eax-32*07],ymm1
vmovapd [eax-32*06],ymm2
vmovapd [eax-32*05],ymm3
vmovapd [eax-32*04],ymm4
vmovapd [eax-32*03],ymm5
vmovapd [eax-32*02],ymm6
vmovapd [eax-32*01],ymm7
vmovapd [eax+32*00],ymm0
vmovapd [eax+32*01],ymm1
vmovapd [eax+32*02],ymm2
vmovapd [eax+32*03],ymm3
vmovapd [eax+32*04],ymm4
vmovapd [eax+32*05],ymm5
vmovapd [eax+32*06],ymm6
vmovapd [eax+32*07],ymm7
add eax,ebx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Block_Write_AVX256       ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_Write_AVX256:
; Prepare tail cycle
Small_Write_AVX256:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_Write_AVX256
mov ebx,32
; Tail cycle
Tail_Write_AVX256:
vmovapd [eax-32*08],ymm0
add eax,ebx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Tail_Write_AVX256        ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_Write_AVX256:
dec ebp
jnz Measurement_Write_AVX256  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret

