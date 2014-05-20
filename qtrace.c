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
#include <stdarg.h>
#include "cpu.h"
#include "qtrace.h"


InstructionRtn* icb_head = NULL;
IBasicBlockRtn* ibb_head = NULL;

static const char* icb = "InstructionCallBack"; 
static const char* bcb = "BasicBlockCallBack"; 
static const char* ims = "INS_InsertCall";
static const char* ibb = "IBB_InsertCall";
static void* handle = NULL;

static inline void handle_unable_load_module(const char *optarg)
{
   QTRACE_ERROR("unable to load module %s\n", optarg);
   QTRACE_EXIT(-1);
}

static void register_instruction_cb(INSTRUCTIONCB cb)
{
   /* it is possible that this module does not define a instruction callback */
   if (!cb) return;

   printf("register InstructionCallBack\n");
   InstructionRtn *head = icb_head;
   if (!icb_head) head = icb_head = malloc(sizeof(InstructionRtn));
   else 
   {
     /* get to the end of the linkedlist */
     while(head->next) head = head->next;
     head->next = malloc(sizeof(InstructionRtn));
     head = head->next;
   }
   /* register the callback */
   head->rtn  = cb;
   head->next = NULL;
}

static void register_ibasicblock_cb(BASICBLOCKCB cb)
{
   /* it is possible that this module does not define a basicblock callback */
   if (!cb) return;

   IBasicBlockRtn *head = ibb_head;
   if (!ibb_head) head = ibb_head = malloc(sizeof(IBasicBlockRtn));
   else 
   {
     /* get to the end of the linkedlist */
     while(head->next) head = head->next;
     head->next = malloc(sizeof(IBasicBlockRtn));
     head = head->next;
   }
   /* register the callback */
   head->rtn  = cb;
   head->next = NULL;
}


static void qtrace_instrument(unsigned pos, ...) 
{
  int idx=0;
  va_list arguments;                  
  va_start (arguments, pos);         
  for (idx=0;idx<pos;idx++);        
  va_end ( arguments );          
}

void qtrace_instrument_parse(const char *module)
{
   /* load the instrumentation module */
   if (!(handle=dlopen(module, RTLD_LAZY))) handle_unable_load_module(module);

   /* register the instrumentation module functions with runtime */
   register_instruction_cb((INSTRUCTIONCB)dlsym(handle, icb));
   register_ibasicblock_cb((BASICBLOCKCB)dlsym(handle, bcb));

   /* register runtime functions with the instrumentation module */
   MODULE_FUNC_INIT module_function_init;
   module_function_init = dlsym(handle, ims);
   module_function_init(qtrace_instrument);
   /* done */
}

/* invoke all the instruction callbacks */
void qtrace_invoke_instruction_callback(unsigned arg0)
{
    InstructionRtn* curr_icc = icb_head;
    while(curr_icc) 
    {
       assert(curr_icc->rtn);
       curr_icc->rtn(arg0);
       curr_icc = curr_icc->next;
    } 
    return;
}







