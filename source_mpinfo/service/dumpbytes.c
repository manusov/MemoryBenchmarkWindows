/*
 *   Memory dump helpers for bytes.
 */

// pointer = data array pointer
// offset = value for print address only
// size = print block size, bytes
void dumpBytes( PBYTE pointer, SIZE_T offset, SIZE_T size, WORD defaultColor )
{
	// Unaligned begin
	DWORD_PTR a = offset;
	a = a & 0xF;
	DWORD_PTR b = 16 - a;
	DWORD_PTR c = offset & 0xFFFFFFF0;
	if ( b > size )
	{
		b = size;
	}
	lineBytes( pointer, c, a, b, defaultColor );
	pointer += b;
	c += 16;
	size -= b;
	// Aligned middle
	int i = 0;
	int n = size / 16;
	for(i=0; i<n; i++)
	{
		lineBytes( pointer, c, 0, 16, defaultColor );
		pointer += 16;
		c += 16;
		size -= 16;
	}
	// Unaligned end
	n = size % 16;
	if ( n > 0 )
	{
		lineBytes( pointer, c, 0, n, defaultColor );
	}
}

// Helper routine
// pointer = data array pointer
// offset = value for print address only
// blank = number of bytes to skip at left, for unaligned begin
// print = number of bytes to print, for unaligned end
void lineBytes( PBYTE pointer, SIZE_T offset, int blank, int print, WORD defaultColor )
{
	// Local variables
	CHAR addressString[81];
	CHAR dataString[81];
	CHAR textString[81];
	CHAR* pa = addressString;
	CHAR* pd = dataString;
	CHAR* pt = textString;
	BYTE x;
	int i;
	PBYTE p1;
	CHAR c;
	int tail = 16 - blank - print;
	// Build address string
	snprintf( pa, 81, "\n %08X  ", offset );
	// Build data string
	p1 = pointer;
	for(i=0; i<blank; i++)
	{
		pd += snprintf( pd, 4, "   " );
	}
	for(i=0; i<print; i++)
	{
		x = *pointer++;
		pd += snprintf( pd, 4, "%02X ", x );
	}
	for(i=0; i<tail; i++)
	{
		pd += snprintf( pd, 4, "   " );
	}
	// Build text string
	pt += snprintf( pt, 3, " " );
	for(i=0; i<blank; i++)
	{
		pt += snprintf( pt, 2, " " );
	}
	for(i=0; i<print; i++)
	{
		c = *p1++;
		if ( ( c < ' ' ) || ( c > 'z' ) )
		{
			c = '.';
		}
		pt += snprintf( pt, 2, "%c", c );
	}
	for(i=0; i<tail; i++)
	{
		pt += snprintf( pt, 2, " " );
	}
	// Print all strings
	WORD color1 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	WORD color2 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD color3 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	CSTR cs1[] = { { color1, addressString },
	               { color2, dataString },
	               { color3, textString },
				   { 0, NULL } };
	colorPrint(cs1);
}

