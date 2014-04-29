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
#include "cpu.h"
#include "qtrace.h"

static const char* icb = "InstructionCallBack"; 
static const char* bcb = "BasicBlockCallBack"; 
static void* handle = NULL;

static inline void handle_unable_load_module(const char *optarg)
{
   QTRACE_ERROR("unable to load module %s\n", optarg);
   QTRACE_EXIT(-1);
}

static void register_instruction_cb(INSTRUCTIONCB cb)
{
   printf("cb at address 0x%lx\n", (long int)cb);
}

static void register_ibasicblock_cb(BASICBLOCKCB cb)
{
   printf("cb at address 0x%lx\n", (long int)cb);
}

int qtrace_instrument_parse(const char *module)
{
   /* load the instrumentation module */
   if (!(handle=dlopen(optarg, RTLD_LAZY))) handle_unable_load_module(optarg);

   /* register the runtime instrumentation functions */
   register_instruction_cb((INSTRUCTIONCB)dlsym(handle, icb));
   register_ibasicblock_cb((INSTRUCTIONCB)dlsym(handle, bcb));
   /* done */
   return 0;
}



