/*
 *  SSUtility.c
 *  seeSaw
 *
 *  Created by Daniel Heckenberg.
 *  Copyright (c) 2004 Daniel Heckenberg. All rights reserved.
 *  (danielh.seeSaw<at>cse<dot>unsw<dot>edu<dot>au)
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the right to use, copy, modify, merge, publish, communicate, sublicence, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * TO THE EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "SSUtility.h"

void
printError(const char* format, ...)
{
    va_list arglist;
	fprintf(stderr, "seeSaw: ");
    va_start( arglist, format );
    vfprintf( stderr, format, arglist );
    va_end( arglist );
}

long inline
min (long l1, long l2)
{
	return (l1 < l2) ? l1 : l2;
}

int inline
ceilpow2i(int i)
{
	return 1 << (int) ceil(log2(i));
}

float inline
ceilpow2f(float f)
{
	return (float) (1L << (long) ceil(log2(f)));
}

#pragma mark ---- Window Management data ----

#define HASHTABLE_SIZE 16

typedef struct tagIDPair
{
	int		ID;
	void*   pData;
	struct tagIDPair* pNext;
} IDPair;

typedef struct tagHashTable
{
	int			size;
	IDPair**	ppEntries;
} HashTable;

static HashTable* gpHT = NULL;

#pragma mark ---- Hash table forward declarations ----

/* Forward declaration of Hash table functions */
HashTable* 
hashNew(int size);

void
hashDelete(HashTable* pHT);

int
hashInsert(HashTable* pHT, int ID, void* pData);

void*
hashRemove(HashTable* pHT, int ID);

void* 
hashRecall(HashTable* pHT, int ID);

#pragma mark ---- Window Management functions ----

static HashTable*
getHT(void)
{
	if (!gpHT)
	{
		gpHT = hashNew(HASHTABLE_SIZE);
	}
	return gpHT;
}

void*
getDataForID(int ID)
{
	void* pData;
	if ((pData = hashRecall(getHT(), ID)))
		return pData;
	else
	{
		printError("getDataForID: ID not found\n");
		return NULL;
	}
}

void*
addDataforID(int ID, void* pData)
{
	if ((hashInsert(getHT(), ID, pData)))
	{
		return pData;
	} else 
	{
		printError("addDataforID: no more windows available\n");	
		return NULL;
	}	
}

void
removeDataforID(int ID)
{
	if (!hashRemove(getHT(), ID))
	{
		printError("removeDataforID: ID not found\n");
	}
}

#pragma mark ---- Hash table functions ----

int hashFunction(HashTable* pHT, int ID) 
{
   return ID % pHT->size;
}

HashTable* hashNew(int size)
{
	int i;
	HashTable* pHT;
	
	pHT = (HashTable*) malloc(sizeof(HashTable));
	if (pHT)
	{
		if ((pHT->ppEntries = (IDPair**)malloc(sizeof(IDPair*)*size)))
		{
			pHT->size = size;
			for (i = 0; i < size; ++i)
			{
				pHT->ppEntries[i] = NULL;
			}
		} else
		{
			free(pHT);
			pHT = NULL;
		}
	}
    return pHT;
}

void
hashDelete(HashTable* pHT)
{
 	int h;
 	IDPair* pEntry = NULL; 
	IDPair* pEntryNext = NULL;
 
 	for (h = 0; h < pHT->size; ++h)
 	{
		for (pEntry = pHT->ppEntries[h]; pEntry; )
 		{
			pEntryNext = pEntry->pNext;
			free(pEntry);
			pEntry = pEntryNext;
 		}
 	}
}

IDPair*
hashLocate(HashTable* pHT, int ID, int createFlag)
{
	int h;
	IDPair* pEntry = NULL;
	
	h = hashFunction(pHT, ID);
	for (pEntry = pHT->ppEntries[h]; pEntry; pEntry=pEntry->pNext)
	{
		if ((pEntry->ID) == ID)
			return pEntry;
	}
	if (createFlag)
	{   
	     pEntry = (IDPair*) malloc(sizeof(IDPair));
         pEntry->ID = ID;
         pEntry->pData = NULL;
         pEntry->pNext = pHT->ppEntries[h];
         pHT->ppEntries[h] = pEntry;
    }
	return pEntry;
}

void*
hashRemove(HashTable* pHT, int ID)
{
	int h;
	IDPair* pEntry = NULL;
	IDPair* pLastEntry = NULL;
	void* pData = NULL;
	
	h = hashFunction(pHT, ID);
	for (pEntry = pHT->ppEntries[h]; pEntry; pLastEntry=pEntry, pEntry=pEntry->pNext)
	{
		if ((pEntry->ID) == ID)
		{
			if (pEntry == pHT->ppEntries[h])
			{
				pHT->ppEntries[h] = pEntry->pNext;
			} else if (pLastEntry)
			{
				pLastEntry->pNext = pEntry->pNext;
			}
			pData = pEntry->pData;
			free(pEntry);
			break;
		}
	}
	return pData;
}

int
hashInsert(HashTable* pHT, int ID, void* pData)
{
	IDPair* pEntry = NULL;
	
	// Fail on duplicate ID
	if (hashLocate(pHT, ID, 0))
		return 0;
		
	pEntry = hashLocate(pHT, ID, 1);
	pEntry->pData = pData;
	return 1;
}

void* 
hashRecall(HashTable* pHT, int ID)
{
	IDPair* pEntry = NULL;
	
	if((pEntry = hashLocate(pHT, ID, 0)))
	{
		return pEntry->pData;
	}
	
	return NULL;
}
