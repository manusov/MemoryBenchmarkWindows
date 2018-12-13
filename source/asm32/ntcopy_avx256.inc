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

Pattern_NtCopy_AVX256:
Measurement_NtCopy_AVX256:
; Prepare big cycle
; Set pointer to middle of 512-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea eax,[esi+256]              ; EAX = Reload source address
lea ebx,[edi+256]              ; EBX = Reload destination address
mov edx,ecx                    ; EDX = Reload length
shr edx,4                      ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_NtCopy_AVX256         ; Go if Length < 16 instructions
; Big cycle
DumpStart_NtCopy_AVX256:
Block_NtCopy_AVX256:
vmovapd ymm0,[eax-32*08]
vmovntpd [ebx-32*08],ymm0
vmovapd ymm1,[eax-32*07]
vmovntpd [ebx-32*07],ymm1
vmovapd ymm2,[eax-32*06]
vmovntpd [ebx-32*06],ymm2
vmovapd ymm3,[eax-32*05]
vmovntpd [ebx-32*05],ymm3
vmovapd ymm4,[eax-32*04]
vmovntpd [ebx-32*04],ymm4
vmovapd ymm5,[eax-32*03]
vmovntpd [ebx-32*03],ymm5
vmovapd ymm6,[eax-32*02]
vmovntpd [ebx-32*02],ymm6
vmovapd ymm7,[eax-32*01]
vmovntpd [ebx-32*01],ymm7
vmovapd ymm0,[eax+32*00]
vmovntpd [ebx+32*00],ymm0
vmovapd ymm1,[eax+32*01]
vmovntpd [ebx+32*01],ymm1
vmovapd ymm2,[eax+32*02]
vmovntpd [ebx+32*02],ymm2
vmovapd ymm3,[eax+32*03]
vmovntpd [ebx+32*03],ymm3
vmovapd ymm4,[eax+32*04]
vmovntpd [ebx+32*04],ymm4
vmovapd ymm5,[eax+32*05]
vmovntpd [ebx+32*05],ymm5
vmovapd ymm6,[eax+32*06]
vmovntpd [ebx+32*06],ymm6
vmovapd ymm7,[eax+32*07]
vmovntpd [ebx+32*07],ymm7
add eax,512                    ; Modify source address, addend=register (not constant) for optimization!
add ebx,512                    ; Modify destination address, addend=register (not constant) for optimization!
dec edx
jnz Block_NtCopy_AVX256        ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_NtCopy_AVX256:
; Prepare tail cycle
Small_NtCopy_AVX256:
mov edx,ecx                    
and edx,00001111b              ; ECX = Extract tail length
jz Measure_NtCopy_AVX256
; Tail cycle
Tail_NtCopy_AVX256:
vmovapd ymm0,[eax-32*08]
vmovntpd [ebx-32*08],ymm0
add eax,32                     ; Modify source address, addend=register (not constant) for optimization!
add ebx,32                     ; Modify destination address, addend=register (not constant) for optimization!
dec edx
jnz Tail_NtCopy_AVX256         ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_NtCopy_AVX256:
dec ebp
jnz Measurement_NtCopy_AVX256  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret

