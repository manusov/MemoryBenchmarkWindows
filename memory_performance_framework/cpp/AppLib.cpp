/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System information and performance analysing application,
include console version of NCRB (NUMA CPU and RAM Benchmark).
See also memory benchmark projects:
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples:
https://github.com/manusov/Prototyping
Special thanks for C++ lessons:
https://ravesli.com/uroki-cpp/

Class realization for helpers library:
support display output and file report.
TODO.

*/

#include "Applib.h"

bool AppLib::screenMode;
bool AppLib::fileMode;
char* AppLib::outPointer;
size_t AppLib::maxOutSize;
size_t AppLib::currentOutSize;
HANDLE AppLib::hStdout;
CONSOLE_SCREEN_BUFFER_INFO AppLib::csbi;

BOOL AppLib::initConsole(bool screenMode, bool fileMode, char* outPointer, size_t maxOutSize)
{
    AppLib::screenMode = screenMode;
    AppLib::fileMode = fileMode;
    AppLib::outPointer = outPointer;
    AppLib::maxOutSize = maxOutSize;
    AppLib::currentOutSize = 0;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) return FALSE;
    return GetConsoleScreenBufferInfo(hStdout, &csbi);
}
size_t AppLib::getCurrentOutSize()
{
    return currentOutSize;
}
void AppLib::write(const char* string)
{
    if (screenMode)
    {
        using namespace std;
        cout << string;
    }
    if (fileMode)
    {
        size_t limit = maxOutSize - currentOutSize;
        size_t addend = snprintf(outPointer, limit, string);
        outPointer += addend;
        currentOutSize += addend;
    }
}
void AppLib::writeColor(const char* string, WORD color)
{
    colorHelper(color);
    write(string);
    colorRestoreHelper();
}
void AppLib::writeParmGroup(const char* string)
{
    char temp[APPCONST::MAX_TEXT_STRING];
    snprintf(temp, APPCONST::MAX_TEXT_STRING, "[%s]\r\n", string);
    colorHelper(APPCONST::GROUP_COLOR);
    write(temp);
    colorRestoreHelper();
}
void AppLib::writeParmError(const char* string)
{
    char temp[APPCONST::MAX_TEXT_STRING];
    snprintf(temp, APPCONST::MAX_TEXT_STRING, "ERROR: %s.\r\n", string);
    colorHelper(APPCONST::ERROR_COLOR);
    write(temp);
    colorRestoreHelper();
}
void AppLib::writeParmAndValue(const char* parmName, const char* parmValue, int nameWidth)
{
    char temp[APPCONST::MAX_TEXT_STRING];
    int n = snprintf(temp, APPCONST::MAX_TEXT_STRING, "%s", parmName);
    int m = nameWidth - n;
    int limit = APPCONST::MAX_TEXT_STRING - 1;
    if ((n + m) > limit)
    {
        m = limit - n;
    }
    memset(temp + n, ' ', m);
    temp[m + n] = 0;
    colorHelper(APPCONST::PARM_COLOR);
    write(temp);
    snprintf(temp, APPCONST::MAX_TEXT_STRING, "%s\r\n", parmValue);
    colorHelper(APPCONST::VALUE_COLOR);
    write(temp);
    colorRestoreHelper();
}
void AppLib::writeHexDump(byte* ptr, int size)
{
    // initialization
    char temp[APPCONST::MAX_TEXT_STRING];
    int address = 0;
    byte* ptr1;
    char* temp1;
    int n;
    int m;
    int k;
    // cycle for dump strings
    int lines = size / APPCONST::DUMP_BYTES_PER_LINE;
    if ((size % APPCONST::DUMP_BYTES_PER_LINE) > 0)
    {
        lines++;
    }
    for (int i = 0; i < lines; i++)
    {
        // print address part of dump string
        colorHelper(APPCONST::DUMP_ADDRESS_COLOR);
        temp1 = temp;
        temp1 += snprintf(temp, APPCONST::MAX_TEXT_STRING, " %08X  ", address);
        write(temp);
        // print data part of dump string
        colorHelper(APPCONST::DUMP_DATA_COLOR);
        if (size > APPCONST::DUMP_BYTES_PER_LINE)
        {
            n = APPCONST::DUMP_BYTES_PER_LINE;
        }
        else
        {
            n = size;
        }
        m = APPCONST::MAX_TEXT_STRING;
        ptr1 = ptr;
        temp1 = temp;
        for (int j = 0; j < n; j++)
        {
            k = snprintf(temp1, m, "%02X ", *ptr1++);
            temp1 += k;
            m -= k;
        }
        for (int j = 0; j < (APPCONST::DUMP_BYTES_PER_LINE - n); j++)
        {
            k = snprintf(temp1, m, "   ");
            temp1 += k;
            m -= k;
        }
        k = snprintf(temp1, m, " ");
        temp1 += k;
        m -= k;
        write(temp);
        // print text part of dump string
        colorHelper(APPCONST::DUMP_TEXT_COLOR);
        m = APPCONST::MAX_TEXT_STRING;
        ptr1 = ptr;
        temp1 = temp;
        for (int j = 0; j < n; j++)
        {
            char c = *ptr1++;
            if ((c < ' ') || (c > '}'))
            {
                c = '.';
            }
            k = snprintf(temp1, m, "%c", c);
            temp1 += k;
            m -= k;
        }
        for (int j = 0; j < (APPCONST::DUMP_BYTES_PER_LINE - n); j++)
        {
            k = snprintf(temp1, m, " ");
            temp1 += k;
            m -= k;
        }
        write(temp);
        write("\r\n");
        // cycle for dump strings
        address += APPCONST::DUMP_BYTES_PER_LINE;
        ptr += APPCONST::DUMP_BYTES_PER_LINE;
        size -= APPCONST::DUMP_BYTES_PER_LINE;
    }
    // restore color
    colorRestoreHelper();
}
void AppLib::writeHexDumpUp()
{
    writeLine(APPCONST::DUMP_WIDTH);
    writeColor(" Offset    x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF\r\n",
        APPCONST::TABLE_COLOR);
    writeLine(APPCONST::DUMP_WIDTH);
}
void AppLib::writeLine(int count)
{
    int limit = APPCONST::MAX_TEXT_STRING - 3;
    if (count > limit)
    {
        count = limit;
    }
    char temp[APPCONST::MAX_TEXT_STRING];
    memset(temp, '-', count);
    temp[count] = '\r';
    temp[count + 1] = '\n';
    temp[count + 2] = 0;
    write(temp);
}
void AppLib::writeColorLine(int count, WORD color)
{
    colorHelper(color);
    writeLine(count);
    colorRestoreHelper();
}
size_t AppLib::storeHex8(char* buffer, size_t limit, DWORD32 value, bool h)
{
    value &= 0xFF;
    if (h)
    {
        return snprintf(buffer, limit, "%02Xh", value);
    }
    else
    {
        return snprintf(buffer, limit, "%02X", value);
    }
}
size_t AppLib::storeHex16(char* buffer, size_t limit, DWORD32 value, bool h)
{
    value &= 0xFFFF;
    if (h)
    {
        return snprintf(buffer, limit, "%04Xh", value);
    }
    else
    {
        return snprintf(buffer, limit, "%04X", value);
    }
}
size_t AppLib::storeHex32(char* buffer, size_t limit, DWORD32 value, bool h)
{
    if (h)
    {
        return snprintf(buffer, limit, "%08Xh", value);
    }
    else
    {
        return snprintf(buffer, limit, "%08X", value);
    }
}
size_t AppLib::storeHex64(char* buffer, size_t limit, DWORD64 value, bool h)
{
    DWORD32 low = value & 0xFFFFFFFFL;
    DWORD32 high = (value >> 32) & 0xFFFFFFFFL;
    if (h)
    {
        return snprintf(buffer, limit, "%08X%08Xh", high, low);
    }
    else
    {
        return snprintf(buffer, limit, "%08X%08X", high, low);
    }
}
size_t AppLib::storeHexPointer(char* buffer, size_t limit, LPVOID ptr, bool h)
{
#ifdef NATIVE_WIDTH_64
    DWORD64 value = (DWORD64)ptr;
    return AppLib::storeHex64(buffer, limit, value, h);
#endif
#ifdef NATIVE_WIDTH_32
    DWORD32 value = (DWORD32)ptr;
    return AppLib::storeHex32(buffer, limit, value, h);
#endif
}
// Print list of "1" bits.
void AppLib::storeBitsList(char* buffer, size_t limit, DWORD64 x)
{
    int count = 0;
    int i = 0;
    int j = 0;
    DWORD64 mask = 1;
    BOOL flag = FALSE;
    // This required if no "1" bits
    *buffer = 0;
    // Print cycle
    while (i < 64)
    {
        // Detect group of set bits
        while ((!(x & mask)) && (i < 64))
        {
            i++;
            mask = (DWORD64)1 << i;
        }
        j = i;
        while ((x & mask) && (j < 64))
        {
            j++;
            mask = (DWORD64)1 << j;
        }
        j--;
        // Print group of set bits
        if (i < 64)
        {
            if (flag)
            {
                count = snprintf(buffer, limit, ", ");
                buffer += count;
                limit -= count;
            }
            if (limit <= 0) break;
            if (i == j)
            {
                count = snprintf(buffer, limit, "%d", i);
            }
            else
            {
                count = snprintf(buffer, limit, "%d-%d", i, j);
            }
            buffer += count;
            limit -= count;
            if (limit <= 0) break;
            j++;
            i = j;
            flag = TRUE;
        }
    }
    return;
}
// Print memory size string (Bytes, KB, MB, GB) to buffer.
void AppLib::storeCellMemorySize(char* buffer, size_t limit, DWORD64 x, size_t cell)
{
    // Print number at left of fixed size cell.
    double xd = static_cast<double>(x);
    int count = 0;
    if ( (x < APPCONST::KILO) || ((x < APPCONST::KILO) && (x % APPCONST::KILO)) )
    {
        int xi = (int)x;
        count = snprintf(buffer, limit, "%d bytes", xi);
    }
    else if (x < APPCONST::MEGA)
    {
        xd /= APPCONST::KILO;
        count = snprintf(buffer, limit, "%.3f KB", xd);
    }
    else if (x < APPCONST::GIGA)
    {
        xd /= APPCONST::MEGA;
        count = snprintf(buffer, limit, "%.3f MB", xd);
    }
    else
    {
        xd /= APPCONST::GIGA;
        count = snprintf(buffer, limit, "%.3f GB", xd);
    }
    buffer += count;
    limit -= count;
    // Print extra spaces for fill fixed size cell.
    cellPrintHelper(buffer, limit, cell, count);
}
void AppLib::storeCellMemorySizeInt(char* buffer, size_t limit, DWORD64 x, size_t cell, int mode)
{
    // Print number at left of fixed size cell.
    int count = 0;
    if ((mode == PRINT_SIZE_BYTES) || ((mode == PRINT_SIZE_AUTO) && ((x < APPCONST::KILO) || (x % APPCONST::KILO))))
    {
        count = snprintf(buffer, limit, "%llu", x);
    }
    else if ((mode == PRINT_SIZE_KB) || ((mode == PRINT_SIZE_AUTO) && ((x < APPCONST::MEGA) || (x % APPCONST::MEGA))))
    {
        count = snprintf(buffer, limit, "%lluK", x / APPCONST::KILO);
    }
    else if ((mode == PRINT_SIZE_MB) || ((mode == PRINT_SIZE_AUTO) && ((x < APPCONST::GIGA) || (x % APPCONST::GIGA))))
    {
        count = snprintf(buffer, limit, "%lluM", x / APPCONST::MEGA);
    }
    else if (mode == PRINT_SIZE_GB)
    {
        count = snprintf(buffer, limit, "%lluG", x / APPCONST::GIGA);
    }
    else
    {
        count = snprintf(buffer, limit, "?");
    }
    buffer += count;
    limit -= count;
    // Print extra spaces for fill fixed size cell.
    cellPrintHelper(buffer, limit, cell, count);
}
void AppLib::storeBaseAndSize(char* buffer, size_t limit, DWORD64 blockBase, DWORD64 blockSize)
{
    size_t count = 0;
    count = snprintf(buffer, limit, "base=");
    buffer += count;
    limit -= count;
    if (limit > 0)
    {
        count = storeHex64(buffer, limit, blockBase, TRUE);
        buffer += count;
        limit -= count;
        if (limit > 0)
        {
            count = snprintf(buffer, limit, ", size=");
            buffer += count;
            limit -= count;
            if (limit > 0)
            {
                storeHex64(buffer, limit, blockSize, TRUE);
            }
        }
    }
}
// Build executable path at destination buffer, use const file name.
// This function used for save text report in the executable file directory,
// not in the current directory.
BOOL AppLib::storeExecReportPath(char* buffer, size_t limit, const char* name)
{
    memset(buffer, 0, limit);                               // blank buffer
    int n = GetModuleFileName(NULL, buffer, (DWORD)limit);  // get this executable file path by WinAPI
    int i;
    LPSTR p = buffer + n - 1;
    for (i = 0; i < n; i++)     // cycle for remove this executable file name
    {
        if (*p == '\\') break;  // detect path separator
        *p = 0;
        p--;
    }
    if (i >= n) return FALSE;  // return with error if path separator '\' not found
    p++;
    while (*p++ = *name++);    // copy report file name instead just blanked executable file name
    return TRUE;
}
void AppLib::storeSystemErrorName(char* buffer, size_t limit, DWORD errorCode)
{
    // This required if no strings stored.
    *buffer = 0;
    // Reject 0 = no OS errors, -1 = error without OS API code.
    if ((errorCode > 0) && (errorCode < 0x7FFFFFFF))
    {
        // Local variables
        LPVOID lpvMessageBuffer;
        DWORD status;
        // Build message string = f (error code)
        status = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, 
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),  // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpvMessageBuffer, 0, NULL);
        if (status)
        {
            // This visualized if error string build OK.
            snprintf(buffer, limit, "OS API reports error %d = %s", errorCode, (char*)lpvMessageBuffer);
        }
        else
        {
            DWORD dwError = GetLastError();
            // This visualized if build error string FAILED.
            snprintf(buffer, limit, "Decode OS error failed, format message error %d", dwError);
        }
        // Free the buffer allocated by the system API function
        LocalFree(lpvMessageBuffer);
    }
}
void AppLib::calculateStatistics(std::vector<double>& data, double& min, double& max, double& average, double& median)
{
    size_t n = data.size();
    if (n)
    {
        std::sort(data.begin(), data.end());
        double sum = std::accumulate(data.begin(), data.end(), double(0));
        min = data[0];
        max = data[n - 1];
        average = sum / n;
        median = (n % 2) ? (data[n / 2]) : ((data[n / 2 - 1] + data[n / 2]) / 2.0);
    }
    else
    {
        min = 0.0;
        max = 0.0;
        average = 0.0;
        median = 0.0;
    }
}
void AppLib::colorHelper(WORD color)
{
    if (screenMode)
    {
        SetConsoleTextAttribute(hStdout, (csbi.wAttributes & 0xF0) | (color & 0x0F));
    }
}
void AppLib::colorRestoreHelper()
{
    if (screenMode)
    {
        SetConsoleTextAttribute(hStdout, csbi.wAttributes);
    }
}
void AppLib::cellPrintHelper(char* buffer, size_t limit, size_t cell, size_t count)
{
    int extra = (int)(cell - count);
    if ((extra > 0) && (limit > 0))
    {
        for (int i = 0; i < extra; i++)
        {
            count = snprintf(buffer, limit, " ");
            buffer += count;
            limit -= count;
            if (limit <= 0) break;
        }
    }
}

