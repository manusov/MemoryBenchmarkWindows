/*
 *   Regular detector for optional functionality depend by CPU features
 */

BOOLEAN getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask );
BOOLEAN getXgetbvFeature( DWORD bitmask );
DWORDLONG buildCpuidBitmap( CPUID_CONDITION x[] );
DWORDLONG buildXgetbvBitmap( XGETBV_CONDITION x[] );


