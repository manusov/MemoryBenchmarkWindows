/*
 *   Data structures for Memory Performance Engine command line options support.
 *   Details, see also associated definitions at options.c.
 */

// Methods for read-write memory by CPU instruction set
char* rwMethodsDetails[] = {
	"Read x86-64 (MOV)",
	"Write x86-64 (MOV)",
	"Copy x86-64 (MOV)",
	"Modify x86-64 (NOT)",
	"Write x86-64 strings (REP STOSQ)",
	"Copy x86-64 strings (REP MOVSQ)",
	"Read SSE-128 (MOVAPS)",
	"Write SSE-128 (MOVAPS)",
	"Copy SSE-128 (MOVAPS)",
	"Read AVX-256 (VMOVAPD)",
	"Write AVX-256 (VMOVAPD)",
	"Copy AVX-256 (VMOVAPD)",
	"Read AVX-512 (VMOVAPD)",
	"Write AVX-512 (VMOVAPD)",
	"Copy AVX-512 (VMOVAPD)",
	"Dot product FMA-256 (VFMADD231)",
	"Dot product FMA-512 (VFMADD231)",
	// Additions for non-temporal Write
	"Non-temporal write SSE-128 (MOVNTPS)",
	"Non-temporal copy SSE-128 (MOVAPS+MOVNTPS)",    // this duplicated 1
	"Non-temporal write AVX-256 (VMOVNTPD)",
	"Non-temporal copy AVX-256 (VMOVAPD+VMOVNTPD)",    // this duplicated 2, wrong
	"Non-temporal write AVX-512 (VMOVNTPD)",
	"Non-temporal copy AVX-512 (VMOVAPD+VMOVNTPD)",
	// Additions for non-temporal Read
	"Non-temporal read SSE-128 (MOVNTDQA)",
	"Non-temporal copy SSE-128 (MOVNTDQA+MOVNTPD)",
	"Non-temporal read AVX-256 (VMOVNTDQA)",
	"Non-temporal copy AVX-256 (VMOVNTDQA+VMOVNTPD)",
	"Non-temporal read AVX-512 (VMOVNTDQA)",
	"Non-temporal copy AVX-512 (VMOVNTDQA+VMOVNTPD)",
	// Additions for non-temporal Read if CPU not supports (V)MOVNTDQA
	// also if (V)MOVNTDQA is supported, but non-optimal
	// based on PREFETCHNTA hint
	"Non-temporal read SSE-128 (PREFETCHNTA+MOVAPS)",
	"Non-temporal copy SSE-128 (PREFETCHNTA+MOVAPS+MOVNTPS)",  // this duplicated 1
	"Non-temporal read AVX-256 (PREFETCHNTA+VMOVAPD)",
	// Reserved for extensions
	// "Non-temporal copy AVX-256 (PREFETCHNTA+VMOVAPD)"
	// "Non-temporal read AVX-512 (PREFETCHNTA+VMOVAPD)"
	// "Non-temporal copy AVX-512 (PREFETCHNTA+VMOVAPD)"
	// Reserved for FMA with non-temporal store
	NULL
};
// Target objects for benchmarks
char* rwTargetsDetails[] = {
	"Cache L1", "Cache L2", "Cache L3", "Cache L4", "System DRAM", "User-defined memory object",
	NULL
};
// Access methods by temporal/nontemporal classification
char* rwAccessDetails[] = {
	"Temporal data", "Non-temporal data", "Default caching mode",
	NULL
};
// Hyper-threading control
char* hyperThreadingDetails[] = {
	"Not supported by platform", "Not used but supported by platform", "Enabled and used"
};
// Page size control
char* pageSizeDetails[] = {
	"Default 4K", "Large 4M (ia32)", "Large 2M (x64)",
	NULL
};
// NUMA control
char* numaModeDetails[] = {
	"Non aware", "Force optimal", "Force non-optimal",
	NULL
};
// Precision control
char* precisionDetails[] = {
	"Slow and accurate", "Fast and approximation",
	NULL
};
// Machine readable option control
char* machineReadableDetails[] = {
	"Off", "On",
	NULL
};

