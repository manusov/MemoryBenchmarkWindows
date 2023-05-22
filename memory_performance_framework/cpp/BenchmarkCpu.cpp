/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class realization for CPU benchmark.

*/

#include "BenchmarkCpu.h"

BenchmarkCpu::BenchmarkCpu(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp) :
	USM(op, cs, pp)
{
}
BenchmarkCpu::~BenchmarkCpu()
{
}
void BenchmarkCpu::execute()
{
}
