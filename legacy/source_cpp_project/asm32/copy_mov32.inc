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

Pattern_Copy_MOV32:
Measurement_Copy_MOV32:
push ebp

; Prepare big cycle
mov eax,esi                  ; EAX = Reload source address
mov ebx,edi                  ; EBX = Reload destination address
mov edx,ecx                  ; EDX = Reload length
shr edx,4                    ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_Copy_MOV32          ; Go if Length < 16 instructions
; Big cycle
DumpStart_Copy_MOV32:
Block_Copy_MOV32:
mov ebp,[eax+08*00]
mov [ebx+08*00],ebp
mov ebp,[eax+08*01]
mov [ebx+08*01],ebp
mov ebp,[eax+08*02]
mov [ebx+08*02],ebp
mov ebp,[eax+08*03]
mov [ebx+08*03],ebp
mov ebp,[eax+08*04]
mov [ebx+08*04],ebp
mov ebp,[eax+08*05]
mov [ebx+08*05],ebp
mov ebp,[eax+08*06]
mov [ebx+08*06],ebp
mov ebp,[eax+08*07]
mov [ebx+08*07],ebp
mov ebp,[eax+08*08]
mov [ebx+08*08],ebp
mov ebp,[eax+08*09]
mov [ebx+08*09],ebp
mov ebp,[eax+08*10]
mov [ebx+08*10],ebp
mov ebp,[eax+08*11]
mov [ebx+08*11],ebp
mov ebp,[eax+08*12]
mov [ebx+08*12],ebp
mov ebp,[eax+08*13]
mov [ebx+08*13],ebp
mov ebp,[eax+08*14]
mov [ebx+08*14],ebp
mov ebp,[eax+08*15]
mov [ebx+08*15],ebp
add eax,64                   ; Modify source address
add ebx,64                   ; Modify destination address
dec edx
jnz Block_Copy_MOV32         ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_Copy_MOV32:
; Prepare tail cycle
Small_Copy_MOV32:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_Copy_MOV32
; Tail cycle
Tail_Copy_MOV32:
mov ebp,[eax+08*00]          ; [eax+32*00]  BUG FIX v0.92. don't care because *0 
mov [ebx+08*00],ebp          ; [ebx+32*00]  BUG FIX v0.92. don't care because *0  
add eax,4                    ; Modify source address
add ebx,4                    ; Modify destination address
dec edx
jnz Tail_Copy_MOV32          ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_Copy_MOV32:

pop ebp
dec ebp
jnz Measurement_Copy_MOV32   ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret
