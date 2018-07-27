/*
 *   Get and show CPU information.
 */

typedef enum {
	EAX, EBX, ECX, EDX
} CPUID_OUTPUT;

BOOL getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask );
BOOL getXgetbvFeature( DWORD bitmask );

BOOL detectCpu( CHAR *cpuVendorString, CHAR *cpuModelString );
int measureTsc( DWORDLONG *tscClk, double *tscHz, double *tscNs );

void printCpu( CHAR *cpuVendorString, CHAR *cpuModelString );
void printTsc( double tscHz, double tscNs );


