/*
					   MEMORY PERFORMANCE ENGINE FRAMEWORK.
					  Header for NUMA domains detector class.
	For platforms without NUMA API builds one virtual domain, for regular methods.
*/

#pragma once
#ifndef DOMAINSBUILDER_H
#define DOMAINSBUILDER_H

#include <windows.h>
#include <cstdio>
#include "MainDefinitions.h"
#include "AppLib.h"

class DomainsBuilder
{
public:
	DomainsBuilder(FUNCTIONS_LIST* functions);
	~DomainsBuilder();
	SYSTEM_NUMA_DATA* getNumaList();
	SYSTEM_NUMA_DATA* getSimpleList();
	void getNumaText(LPSTR& dst, size_t& max);
	void getSimpleText(LPSTR& dst, size_t& max);
	char* getStatusString();
	// Memory allocation methods, NUMA-aware
	BOOL allocateNodesList(size_t xs, DWORD pgMode, DWORD64 pgSize, BOOL swapFlag);
	BOOL freeNodesList();
	// Memory allocation methods, NUMA-unaware, but same format for regularity
	BOOL allocateSimpleList(size_t xs, DWORD pgMode, DWORD64 pgSize);
	BOOL freeSimpleList();
private:
	static FUNCTIONS_LIST* f;
	static SYSTEM_NUMA_DATA numaData;
	static SYSTEM_NUMA_DATA simpleData;
	static NUMA_NODE_ENTRY simpleEntry;
	static const char* tableUpNuma;
	static const char* tableUpSimple;
	static char s[];
	// Helpers methods
	void blankNode(NUMA_NODE_ENTRY* p);
	DWORD64 alignByFactor(DWORD64 value, DWORD64 factor);
};

#endif  // DOMAINSBUILDER_H

