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

Pattern_Write_STOSD:
mov edx,ecx                   ; EDX = Save block size , received ESI = Destination block base
Measurement_Write_STOSD:
mov edi,esi                   ; EDI = Restore destination block base , restore - because measurement cycle
mov ecx,edx                   ; ECX = Restore block size , units = hardware cycle iterations
DumpStart_Write_STOSD:
rep stosd                     ; This is target operation - hardware cycle
DumpStop_Write_STOSD:
dec ebp                       ; EBP = Measurement cycle counter
jnz Measurement_Write_STOSD   ; Measurement cycle
ret

