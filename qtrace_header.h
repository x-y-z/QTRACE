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

#include <stdio.h>
#include <stdint.h> 

#define QTRACE_MAX_CALLBACK_NUM 32
short InstructionCallBackNum = 0, BasicBlockCallBackNum = 0;
void* ResetStats = NULL, *PrintStats = NULL;
void* InstructionCallBackArray[QTRACE_MAX_CALLBACK_NUM] = {0};
void* BasicBlockCallBackArray[QTRACE_MAX_CALLBACK_NUM]  = {0};

/* functions to ask QTRACE to insert instrumentation */
static QTRACE_INSERT_INSTRUMENT Module_INS_InsertCall;
static QTRACE_INSERT_INSTRUMENT Module_IBB_InsertCall;

/* AddStatsReset/AddStatsPrint - register stats reset/print */
void AddStatsReset(void *reset) { ResetStats = reset; }
void AddStatsPrint(void *print) { PrintStats = print; }

/* AddInstructionCallBack - register an instruction callback */
void AddInstructionCallBack(void *cb)
{
  	InstructionCallBackArray[InstructionCallBackNum++] = cb;
}

/* AddBasicBlockCallBack - register an basicblock callback */
void AddBasicBlockCallBack(void *cb)
{
  	BasicBlockCallBackArray[BasicBlockCallBackNum++] = cb;
}

/* hook up the QTRACE function to insert instrumentation */
void InitializeInstructionInstrument(QTRACE_INSERT_INSTRUMENT cb)
{
    	Module_INS_InsertCall = cb;
}

/* hook up the QTRACE function to insert instrumentation */
void InitializeBasicBlockInstrument(QTRACE_INSERT_INSTRUMENT cb)
{
    	Module_IBB_InsertCall = cb;
}

#endif /* QTRACE_HEADER_H */
