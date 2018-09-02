/*
 *    Multi-thread management routines.
 */

// Threads management per-step routines.
// Allocated memory resources for multithread context
void mtStepListAlloc( MT_DATA* mtd, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                      LIST_RELEASE_RESOURCES* xr );
// Create multithread context
void mtStepOpen( MT_DATA* mtd, LIST_DLL_FUNCTIONS* xf,
                 MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                 LIST_RELEASE_RESOURCES* xr );
// Allocate performance buffer memory
void mtStepMemoryAlloc( MT_DATA* mtd, MPE_INPUT_PARAMETERS_BLOCK* ipb, 
                        LIST_RELEASE_RESOURCES* xr );
// Benchmarks calibration, adjust measurement repeats count, for multi-thread
void mtStepCalibration( MT_DATA* mtd,
                        MPE_CALIBRATION* xc,
                        LIST_DLL_FUNCTIONS* xf,
                        MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                        LIST_RELEASE_RESOURCES* xr );
// Multithread sequence control per-step procedure.
void mtStepPerformance ( MT_DATA* mtd,
                         LIST_DLL_FUNCTIONS* xf, MPE_PLATFORM_INPUT* xp,
                         MPE_INPUT_PARAMETERS_BLOCK* ipb, MPE_OUTPUT_PARAMETERS_BLOCK* opb,
                         MPE_CALIBRATION* xc,  LIST_RELEASE_RESOURCES* xr );
// Destroy multithread context
void mtStepClose( MT_DATA* mtd, LIST_RELEASE_RESOURCES* xr );

// Additional routines.
// First run performance pattern
void threadsRun( MT_DATA* mtd, LIST_DLL_FUNCTIONS* xf, DWORD64* deltaTSC );
// Non-first (continued) run performance patterns
void threadsRestart( MT_DATA* mtd, LIST_DLL_FUNCTIONS* xf, DWORD64* deltaTSC );
// Performance pattern routine
DWORD WINAPI threadEntry( LPVOID threadControl );

