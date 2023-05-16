mpe64 test=memory asm=ntreadsse128 repeats=50 threads=2 start=16M end=128M step=1M
copy output.txt reports\dram_nt_read_sse.txt
pause