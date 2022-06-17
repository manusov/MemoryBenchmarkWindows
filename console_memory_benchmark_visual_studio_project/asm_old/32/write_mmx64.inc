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

Pattern_Write_MMX64:
mov eax,128
Measurement_Write_MMX64:
; Prepare big cycle
mov ebx,esi                  ; EBX = Reload source address
mov edx,ecx                  ; EDX = Reload length
shr edx,4                    ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_Write_MMX64         ; Go if Length < 16 instructions
; Big cycle
DumpStart_Write_MMX64:
Block_Write_MMX64:
movq [ebx+08*00],mm0
movq [ebx+08*01],mm1
movq [ebx+08*02],mm2
movq [ebx+08*03],mm3
movq [ebx+08*04],mm4
movq [ebx+08*05],mm5
movq [ebx+08*06],mm6
movq [ebx+08*07],mm7
movq [ebx+08*08],mm0
movq [ebx+08*09],mm1
movq [ebx+08*10],mm2
movq [ebx+08*11],mm3
movq [ebx+08*12],mm4
movq [ebx+08*13],mm5
movq [ebx+08*14],mm6
movq [ebx+08*15],mm7
add ebx,eax                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Block_Write_MMX64        ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_Write_MMX64:
; Prepare tail cycle
Small_Write_MMX64:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_Write_MMX64
; Tail cycle
Tail_Write_MMX64:
movq [ebx+08*00],mm0
add ebx,8                    ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Tail_Write_MMX64         ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_Write_MMX64:
dec ebp
jnz Measurement_Write_MMX64   ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
; Required empty MMX state before return
emms
ret
