cls
mpe64 asm=latencyrdrand  memory=l1    adaptive=128M  > report_L1.txt
mpe64 asm=latencyrdrand  memory=l2    adaptive=128M  > report_L2.txt
mpe64 asm=latencyrdrand  memory=l3    adaptive=16M   > report_L3.txt
mpe64 asm=latencyrdrand  memory=dram  adaptive=2M    > report_DRAM.txt
