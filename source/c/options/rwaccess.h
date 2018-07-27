/*
 *   Non-temporal access definitions.
 */

// Enumeration of speculative (cacheable) memory access modes
// temporal means cacheable, non-temporal means non cacheable
typedef enum {
	TEMPORAL, NON_TEMPORAL, DEFAULT_CACHING
} RW_ACCESS;


