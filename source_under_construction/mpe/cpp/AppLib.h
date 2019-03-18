/*
           MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Header for common library, text strings builder class.
*/

#ifndef APPLIB_H
#define APPLIB_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"

class AppLib
{
	public:
		static BOOL buildExecPath( LPSTR &dst, size_t max, LPCSTR src );
		static void printString( LPSTR &dst, size_t &max, LPCSTR src );
		static void printCell( LPSTR &dst, LPCSTR src, size_t &max, size_t min );
		static void printLine( LPSTR &dst, size_t &max, size_t cnt );
		static void printCrLf( LPSTR &dst, size_t &max );
		static void printCellNum( LPSTR &dst, size_t &max, DWORD32 x, size_t min );
		static void print16( LPSTR &dst, size_t &max, DWORD32 x, BOOL h );
		static void print32( LPSTR &dst, size_t &max, DWORD32 x, BOOL h );
		static void print64( LPSTR &dst, size_t &max, DWORD64 x, BOOL h );
		static void printPointer( LPSTR &dst, size_t &max, LPVOID x, BOOL h );
		static void printBitsList( LPSTR &dst, size_t &max, DWORD64 x );
		static void printCellMemorySize( LPSTR &dst, size_t &max, DWORD64 x, size_t min );
		static void printBaseAndSize( LPSTR &dst, size_t &max, DWORD64 base, DWORD64 size );
		static void calculateStatistics
			( int length, double results[], double* min, double* max, double* average, double* median );
		static void printSystemError( DWORD osErrorCode );
	private:
};

#endif  // APPLIB_H
