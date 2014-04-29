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

#include <dlfcn.h>
#include <stdio.h>
#include "qtrace.h"
#include "cpu.h"

static const char* icb = "InstructionCallBack"; 
static const char* bcb = "BasicBlockCallBack"; 

static inline void handle_unable_load_module(const char *optarg)
{
   QTRACE_ERROR("unable to load module %s\n", optarg);
   QTRACE_EXIT(-1);
}

static void register_instruction_callback(INSTRUCTIONCB *cb)
{
   printf("cb at address 0x%lx\n", cb);
}

static void register_basicblock_callback(BASICBLOCKCB *cb)
{
   printf("cb at address 0x%lx\n", cb);
}

int qtrace_instrument_parse(QemuOptsList *opts_list, const char *optarg)
{
   void* handle = dlopen(optarg, RTLD_LAZY);
   if (!handle) 
        handle_unable_load_module(optarg);

   register_instruction_callback((INSTRUCTIONCB)dlsym(handle, icb));
   register_basicblock_callback((INSTRUCTIONCB)dlsym(handle, bcb));
}



