;---------- Memory performance patterns ---------------------------------------;
; INPUT:   RSI = Block #1 pointer (64-bit flat)                                ;
;          RDI = Block #2 pointer (64-bit flat)                                ;
;          R8  = Block #3 pointer (64-bit flat) , reserved and not used yet    ;
;                For Read, Write, Modify use RSI as Source or Destination      ;
;                For Copy use RSI = Source , RDI = Destination                 ;
;          RCX = Block length, units = instructions                            ;
;          RBP = Number of measurement repeats                                 ;
; OUTPUT:  None                                                                ;
;          Registers corrupted, but must save R14, R15                         ;
;------------------------------------------------------------------------------;

Pattern_Write_SSE128:
Measurement_Write_SSE128:
; Prepare big cycle
; Set pointer to middle of 256-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea rax,[rsi+128]            ; RAX = Reload source address
mov rdx,rcx                  ; RDX = Reload length
shr rdx,4                    ; RDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION iterations, required future make 1 instr. iteration (!)
jz Small_Write_SSE128        ; Go if Length < 16 instructions
mov ebx,256                  ; RBX = Addend, this operation also clear bits RBX[63-32]
; Big cycle
DumpStart_Write_SSE128:
Block_Write_SSE128:
movaps [rax-16*08],xmm0
movaps [rax-16*07],xmm1
movaps [rax-16*06],xmm2
movaps [rax-16*05],xmm3
movaps [rax-16*04],xmm4
movaps [rax-16*03],xmm5
movaps [rax-16*02],xmm6
movaps [rax-16*01],xmm7
movaps [rax+16*00],xmm8
movaps [rax+16*01],xmm9
movaps [rax+16*02],xmm10
movaps [rax+16*03],xmm11
movaps [rax+16*04],xmm12
movaps [rax+16*05],xmm13
movaps [rax+16*06],xmm14
movaps [rax+16*07],xmm15
add rax,rbx                  ; Modify address, addend=register (not constant) for optimization!
dec rdx
jnz Block_Write_SSE128       ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_Write_SSE128:
; Prepare tail cycle
Small_Write_SSE128:
mov edx,ecx
and edx,00001111b            ; ECX = Extract tail length
jz Measure_Write_SSE128
mov ebx,16
; Tail cycle
Tail_Write_SSE128:
movaps [rax-16*08],xmm0
add rax,rbx                  ; Modify address, addend=register (not constant) for optimization!
dec edx
jnz Tail_Write_SSE128        ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
; Measurement cycle
Measure_Write_SSE128:
dec rbp
jnz Measurement_Write_SSE128  ; Cycle for measurement, repeat same operation, DEC/JNZ is faster than LOOP!
ret

