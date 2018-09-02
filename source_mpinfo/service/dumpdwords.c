/*
 *   Memory dump helpers for 32-bit double words.
 */

// pointer = data array pointer
// offset = value for print address only
// size = print block size, bytes
void dumpDwords( PDWORD32 pointer, SIZE_T offset, SIZE_T size, WORD defaultColor )
{
	// Unaligned begin
	DWORD_PTR a = offset;
	a = a & 0xC;
	DWORD_PTR b = 16 - a;
	DWORD_PTR c = offset & 0xFFFFFFF0;
	if ( b > size )
	{
		b = size;
	}
	lineDwords( pointer, c, a/4, b/4, defaultColor );
	pointer += b/4;
	c += 16;
	size -= b;
	// Aligned middle
	int i = 0;
	int n = size / 16;
	for(i=0; i<n; i++)
	{
		lineDwords( pointer, c, 0, 4, defaultColor );
		pointer += 4;
		c += 16;
		size -= 16;
	}
	// Unaligned end
	n = size % 16;
	if ( n > 0 )
	{
		lineDwords( pointer, c, 0, n/4, defaultColor );
	}
}

// Helper routine
// pointer = data array pointer
// offset = value for print address only
// blank = number of 32-bit double words to skip at left, for unaligned begin
// print = number of 32-bit double words to print, for unaligned end
void lineDwords( PDWORD32 pointer, SIZE_T offset, int blank, int print, WORD defaultColor )
{
	// Local variables
	CHAR addressString[81];
	CHAR dataString[81];
	CHAR* pa = addressString;
	CHAR* pd = dataString;
	DWORD32 x;
	int i;
	PDWORD32 p1;
	int tail = 4 - blank - print;
	// Build address string
	snprintf( pa, 81, "\n %08X  ", offset );
	// Build data string
	p1 = pointer;
	for(i=0; i<blank; i++)
	{
		pd += snprintf( pd, 10, "         " );
	}
	for(i=0; i<print; i++)
	{
		x = *pointer++;
		pd += snprintf( pd, 10, "%08X ", x );
	}
	for(i=0; i<tail; i++)
	{
		pd += snprintf( pd, 10, "         " );
	}
	// Print all strings
	WORD color1 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	WORD color2 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	CSTR cs1[] = { { color1, addressString },
	               { color2, dataString },
				   { 0, NULL } };
	colorPrint(cs1);
}

