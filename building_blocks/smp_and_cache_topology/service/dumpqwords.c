/*
 *   Memory dump helpers for 64-bit quad words.
 */

// pointer = data array pointer
// offset = value for print address only
// size = print block size, bytes
void dumpQwords( PDWORD64 pointer, SIZE_T offset, SIZE_T size, WORD defaultColor )
{
	// Unaligned begin
	DWORD_PTR a = offset;
	a = a & 0x8;
	DWORD_PTR b = 16 - a;
	DWORD_PTR c = offset & 0xFFFFFFF0;
	if ( b > size )
	{
		b = size;
	}
	lineQwords( pointer, c, a/8, b/8, defaultColor );
	pointer += b/8;
	c += 16;
	size -= b;
	// Aligned middle
	int i = 0;
	int n = size / 16;
	for(i=0; i<n; i++)
	{
		lineQwords( pointer, c, 0, 2, defaultColor );
		pointer += 2;
		c += 16;
		size -= 16;
	}
	// Unaligned end
	n = size % 16;
	if ( n > 0 )
	{
		lineQwords( pointer, c, 0, n/8, defaultColor );
	}
}

// Helper routine
// pointer = data array pointer
// offset = value for print address only
// blank = number of 64-bit quad words to skip at left, for unaligned begin
// print = number of 64-bit quad words to print, for unaligned end
void lineQwords( PDWORD64 pointer, SIZE_T offset, int blank, int print, WORD defaultColor )
{
	// Local variables
	CHAR addressString[81];
	CHAR dataString[81];
	CHAR* pa = addressString;
	CHAR* pd = dataString;
	DWORD64 x;
	int i;
	PDWORD64 p1;
	int tail = 2 - blank - print;
	// Build address string
	snprintf( pa, 81, "\n %08X  ", offset );
	// Build data string
	p1 = pointer;
	for(i=0; i<blank; i++)
	{
		pd += snprintf( pd, 18, "                 " );
	}
	for(i=0; i<print; i++)
	{
		x = *pointer++;
		DWORD32 xh = (x>>31)>>1;  // note shift count > 31 don't work at 32-bit mode
		DWORD32 xl = x;
		pd += snprintf( pd, 18, "%08X%08X ", xh, xl );
	}
	for(i=0; i<tail; i++)
	{
		pd += snprintf( pd, 18, "                 " );
	}
	// Print all strings
	WORD color1 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	WORD color2 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	CSTR cs1[] = { { color1, addressString },
	               { color2, dataString },
				   { 0, NULL } };
	colorPrint(cs1);
}

