include 'win64a.inc'
format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
sub rsp,8*5

lea rsi,[Data1]
lea rdi,[Data2]
xor r8d,r8d
mov rcx, 16  ;  16384 / 8
mov ebp, 2   ;  100000
; call Pattern_Read_MMX64
; call Pattern_Write_MMX64
call Pattern_Copy_MMX64

xor ecx,ecx
call [ExitProcess]

include 'read_mmx64.inc'
include 'write_mmx64.inc'
include 'copy_mmx64.inc'

section '.data' data readable writeable
align 64
; Data1    DB  16384 DUP (?)
; Data2    DB  16384 DUP (?)
Data1    DB  256 DUP (055h)
Data2    DB  256 DUP (011h)

section '.idata' import data readable writeable
library kernel32,'KERNEL32.DLL'
include 'api\kernel32.inc'


