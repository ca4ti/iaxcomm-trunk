/*
 *  SSUtility.h
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

#ifndef SSUTILITY_H
#define SSUTILITY_H

void
printError(const char* format, ...);

long inline
min (long l1, long l2);

int inline
ceilpow2i(int i);

float inline
ceilpow2f(float f);

#pragma mark ---- Window Management functions ----

void*
getDataForID(int ID);

void*
addDataforID(int ID, void* pData);

void
removeDataforID(int ID);

#endif //SSUTILITY_H