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

Pattern_Copy_AVX512:
Measurement_Copy_AVX512:
; Prepare big cycle
mov eax,esi                  ; EAX = Reload source address
mov ebx,edi                  ; EBX = Reload destination address
mov edx,ecx                  ; EDX = Reload length
shr edx,3                    ; EDX = Block length, convert from INSTRUCTIONS to 8xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_Copy_AVX512         ; Go if Length < 16 instructions
; Big cycle
DumpStart_Copy_AVX512:
Block_Copy_AVX512:
vmovapd zmm0,[eax+64*00]
vmovapd [ebx+64*00],zmm0
vmovapd zmm1,[eax+64*01]
vmovapd [ebx+64*01],zmm1
vmovapd zmm2,[eax+64*02]
vmovapd [ebx+64*02],zmm2
vmovapd zmm3,[eax+64*03]
vmovapd [ebx+64*03],zmm3
vmovapd zmm4,[eax+64*04]
vmovapd [ebx+64*04],zmm4
vmovapd zmm5,[eax+64*05]
vmovapd [ebx+64*05],zmm5
vmovapd zmm6,[eax+64*06]
vmovapd [ebx+64*06],zmm6
vmovapd zmm7,[eax+64*07]
vmovapd [ebx+64*07],zmm7
add eax,512                   ; Modify source address
add ebx,512                   ; Modify destination address
dec edx
jnz Block_Copy_AVX512        ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_Copy_AVX512:
; Prepare tail cycle
Small_Copy_AVX512:
mov edx,ecx
and edx,00000111b            ; ECX = Extract tail length
jz Measure_Copy_AVX512
; Tail cycle
Tail_Copy_AVX512:
vmovapd zmm0,[eax+64*00]
vmovapd [ebx+64*00],zmm0
add eax,64                   ; Modify source address
add ebx,64                   ; Modify destination address
dec edx
jnz Tail_Copy_AVX512         ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_Copy_AVX512:
dec ebp
jnz Measurement_Copy_AVX512  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret
