mpe64 test=memory asm=latencyrdrand  memory=l1    adaptive=128M
copy output.txt reports\report_latency_L1.txt
mpe64 test=memory asm=latencyrdrand  memory=l2    adaptive=128M
copy output.txt reports\report_latency_L2.txt
mpe64 test=memory asm=latencyrdrand  memory=l3    adaptive=16M
copy output.txt reports\report_latency_L3.txt
mpe64 test=memory asm=latencyrdrand  memory=dram  adaptive=2M
copy output.txt reports\report_latency_DRAM.txt
pause