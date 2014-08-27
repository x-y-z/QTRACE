/*
 *  QTRACE asynchronous debug tool 
 *
 *  Copyright (c) 2003-2005 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QTRACE_HEADER_H
#define QTRACE_HEADER_H

#include <stdint.h>
#include <stdio.h>

#define QTRACE_MAX_CALLBACK_NUM 32
unsigned InstructionCallBackNum = 0, BasicBlockCallBackNum = 0;
void* InstructionCallBackArray[QTRACE_MAX_CALLBACK_NUM] = {0};
void* BasicBlockCallBackArray[QTRACE_MAX_CALLBACK_NUM]  = {0};
void* ResetStats = NULL, *PrintStats = NULL;

void AddInstructionCallBack(void *callback)
{
  	InstructionCallBackArray[InstructionCallBackNum++] = callback;
}

void AddBasicBlockCallBack(void *callback)
{
  	BasicBlockCallBackArray[BasicBlockCallBackNum++] = callback;
}

void AddStatsReset(void *reset) { ResetStats = reset; }
void AddStatsPrint(void *print) { PrintStats = print; }

static QTRACE_INSERT_INSTRUMENT_CALLBACK Module_INS_InsertCall = NULL;
static QTRACE_INSERT_INSTRUMENT_CALLBACK Module_IBB_InsertCall = NULL;
static QTRACE_INSERT_INSTRUMENT_CALLBACK Module_STS_Reset = NULL;
static QTRACE_INSERT_INSTRUMENT_CALLBACK Module_STS_Print = NULL;

void INS_InsertCall(QTRACE_INSERT_INSTRUMENT_CALLBACK ins_insertcall_func)
{
    Module_INS_InsertCall = ins_insertcall_func;
}

void IBB_InsertCall(QTRACE_INSERT_INSTRUMENT_CALLBACK ibb_insertcall_func)
{
    Module_IBB_InsertCall = ibb_insertcall_func;
}

#endif /* QTRACE_HEADER_H */
