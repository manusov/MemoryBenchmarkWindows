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

Pattern_Latency_Walk:
mov rcx,[rsi]         ; qword [rsi] = walk list entry point, Block #1
.WalkRepeat:
mov rax,rcx
.WalkLinked:          ; Walk linked list, use fetched data as next address
mov rax,[rax]
test rax,rax
jnz .WalkLinked
dec ebp
jnz .WalkRepeat
ret