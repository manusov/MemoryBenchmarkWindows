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

Pattern_Latency_Walk:
mov ecx,[esi]         ; qword [esi] = walk list entry point, Block #1
.WalkRepeat:
mov eax,ecx
.WalkLinked:          ; Walk linked list, use fetched data as next address
mov edx,[eax+0]
mov eax,[eax+4]       ; Get duplicated from high dword
test eax,eax
jnz .WalkLinked
dec ebp
jnz .WalkRepeat
ret
