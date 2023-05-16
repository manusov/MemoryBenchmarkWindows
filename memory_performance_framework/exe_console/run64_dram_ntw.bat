mpe64 test=memory asm=ntwritesse128 repeats=200 threads=4 start=32M end=40M step=1M
copy output.txt reports\dram_nt_write_sse.txt
pause