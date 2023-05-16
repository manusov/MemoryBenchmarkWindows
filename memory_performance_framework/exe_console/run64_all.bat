mpe64 test=memory memory=l1
copy output.txt reports\report_L1.txt
mpe64 test=memory memory=l2
copy output.txt reports\report_L2.txt
mpe64 test=memory memory=l3
copy output.txt reports\report_L3.txt
mpe64 test=memory memory=dram
copy output.txt reports\report_DRAM.txt
pause