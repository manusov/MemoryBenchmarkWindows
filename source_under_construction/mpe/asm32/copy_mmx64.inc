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

Pattern_Copy_MMX64:
Measurement_Copy_MMX64:
; Prepare big cycle
mov eax,esi                  ; EAX = Reload source address
mov ebx,edi                  ; EBX = Reload destination address
mov edx,ecx                  ; EDX = Reload length
shr edx,4                    ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_Copy_MMX64          ; Go if Length < 16 instructions
; Big cycle
DumpStart_Copy_MMX64:
Block_Copy_MMX64:
movq mm0,[eax+08*00]
movq [ebx+08*00],mm0
movq mm1,[eax+08*01]
movq [ebx+08*01],mm1
movq mm2,[eax+08*02]
movq [ebx+08*02],mm2
movq mm3,[eax+08*03]
movq [ebx+08*03],mm3
movq mm4,[eax+08*04]
movq [ebx+08*04],mm4
movq mm5,[eax+08*05]
movq [ebx+08*05],mm5
movq mm6,[eax+08*06]
movq [ebx+08*06],mm6
movq mm7,[eax+08*07]
movq [ebx+08*07],mm7
movq mm0,[eax+08*08]
movq [ebx+08*08],mm0
movq mm1,[eax+08*09]
movq [ebx+08*09],mm1
movq mm2,[eax+08*10]
movq [ebx+08*10],mm2
movq mm3,[eax+08*11]
movq [ebx+08*11],mm3
movq mm4,[eax+08*12]
movq [ebx+08*12],mm4
movq mm5,[eax+08*13]
movq [ebx+08*13],mm5
movq mm6,[eax+08*14]
movq [ebx+08*14],mm6
movq mm7,[eax+08*15]
movq [ebx+08*15],mm7
add eax,128                  ; Modify source address
add ebx,128                  ; Modify destination address
dec edx
jnz Block_Copy_MMX64         ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_Copy_MMX64:
; Prepare tail cycle
Small_Copy_MMX64:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_Copy_MMX64
; Tail cycle
Tail_Copy_MMX64:
movq mm0,[eax+08*00]         ; [eax+32*00]  BUG FIX v0.92. don't care because *0 
movq [ebx+08*00],mm0         ; [ebx+32*00]  BUG FIX v0.92. don't care because *0  
add eax,8                    ; Modify source address
add ebx,8                    ; Modify destination address
dec edx
jnz Tail_Copy_MMX64          ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_Copy_MMX64:
dec ebp
jnz Measurement_Copy_MMX64   ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
; Required empty MMX state before return
emms
ret
