mpe64 test=memory memory=l1    adaptive=512M  
copy output.txt reports\report_adaptive_L1.txt
mpe64 test=memory memory=l2    adaptive=512M
copy output.txt reports\report_adaptive_L2.txt
mpe64 test=memory memory=l3    adaptive=256M
copy output.txt reports\report_adaptive_L3.txt
mpe64 test=memory memory=dram  adaptive=8M
copy output.txt reports\report_adaptive_DRAM.txt
