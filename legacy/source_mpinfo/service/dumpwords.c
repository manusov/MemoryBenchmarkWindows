/*
 *   Memory dump helpers for 16-bit words.
 */

// pointer = data array pointer
// offset = value for print address only
// size = print block size, bytes
void dumpWords( PWORD pointer, SIZE_T offset, SIZE_T size, WORD defaultColor )
{
	// Unaligned begin
	DWORD_PTR a = offset;
	a = a & 0xE;
	DWORD_PTR b = 16 - a;
	DWORD_PTR c = offset & 0xFFFFFFF0;
	if ( b > size )
	{
		b = size;
	}
	lineWords( pointer, c, a/2, b/2, defaultColor );
	pointer += b/2;
	c += 16;
	size -= b;
	// Aligned middle
	int i = 0;
	int n = size / 16;
	for(i=0; i<n; i++)
	{
		lineWords( pointer, c, 0, 8, defaultColor );
		pointer += 8;
		c += 16;
		size -= 16;
	}
	// Unaligned end
	n = size % 16;
	if ( n > 0 )
	{
		lineWords( pointer, c, 0, n/2, defaultColor );
	}
}

// Helper routine
// pointer = data array pointer
// offset = value for print address only
// blank = number of 16-bit words to skip at left, for unaligned begin
// print = number of 16-bit words to print, for unaligned end
void lineWords( PWORD pointer, SIZE_T offset, int blank, int print, WORD defaultColor )
{
	// Local variables
	CHAR addressString[81];
	CHAR dataString[81];
	CHAR* pa = addressString;
	CHAR* pd = dataString;
	WORD x;
	int i;
	PWORD p1;
	int tail = 8 - blank - print;
	// Build address string
	snprintf( pa, 81, "\n %08X  ", offset );
	// Build data string
	p1 = pointer;
	for(i=0; i<blank; i++)
	{
		pd += snprintf( pd, 6, "     " );
	}
	for(i=0; i<print; i++)
	{
		x = *pointer++;
		pd += snprintf( pd, 6, "%04X ", x );
	}
	for(i=0; i<tail; i++)
	{
		pd += snprintf( pd, 6, "     " );
	}
	// Print all strings
	WORD color1 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	WORD color2 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	CSTR cs1[] = { { color1, addressString },
	               { color2, dataString },
				   { 0, NULL } };
	colorPrint(cs1);
}

