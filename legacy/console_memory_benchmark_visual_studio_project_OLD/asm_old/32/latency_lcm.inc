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

Pattern_Latency_LCM:
; Initializing parameters for build linear list, build linear list  
push edi ecx esi 
xor eax,eax
xor edx,edx
mov ebp,8
.BuildLinear:                ; This cycle for build sequental offsets list
mov [esi+0],eax
mov [esi+4],edx
add esi,ebp
add eax,ebp
dec ecx
jnz .BuildLinear
pop esi ecx
; Initializing parameters for random unsorting linear list, build it
RANDOM_SEED        EQU  3
RANDOM_MULTIPLIER  EQU  00DEECE66Dh  ;  05DEECE66Dh  
RANDOM_ADDEND      EQU  0Bh   
push ecx esi

xor edi,edi
bsr eax,ecx
inc eax
bts edi,eax
dec edi               ; EDI = mask
mov ebp,ecx           ; EBP = limit
mov ebx,RANDOM_SEED   ; EBX = Value

.BuildRandom:
mov eax,ebx
mov edx,RANDOM_MULTIPLIER
mul edx
add eax,RANDOM_ADDEND
mov ebx,eax
and eax,edi
cmp eax,ebp
jae .BuildRandom
shl eax,3
add eax,[esp+00]                   ; dword [esp+00] = unmodified base ESI
push dword [eax] dword [esi]
pop dword [eax] dword [esi]
add esi,8
dec ecx
jnz .BuildRandom

pop esi ecx edi
; Initializing parameters for build linked random offsets list, build it
push esi
xor eax,eax           ; EAX = Value
mov ebp,8             ; EBP = Addend
.BuildLinked:         ; This cycle for build linked random offsets list
mov ebx,[esi]
add ebx,edi
mov [ebx+0],eax
mov [ebx+4],eax       ; Duplicate at high dword
mov eax,ebx
add esi,ebp
dec ecx
jnz .BuildLinked 
pop esi
; Store EAX = walk list entry point to Block#1 (original pointer ESI)
mov [esi],eax
ret
