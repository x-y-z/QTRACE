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


/* the instrumentation context of the current instruction */
InstrumentContext icontext; 

InstructionRtn* instruction_list = NULL;
IBasicBlockRtn* basicblock_list = NULL;

static const char* icb = "InstructionCallBack"; 
static const char* bcb = "BasicBlockCallBack"; 
static const char* ims = "INS_InsertCall";
//static const char* ibb = "IBB_InsertCall";
static void* handle = NULL;

static inline void handle_unable_load_module(const char *optarg)
{
   	QTRACE_ERROR("unable to load instrumentation module %s\n", optarg);
   	QTRACE_EXIT(-1);
}

static void register_instruction_cb(INSTRUCTION_CALLBACK cb)
{
   	/* it is possible that this module does not define a instruction callback */
   	if (!cb) return;

   	InstructionRtn *head = instruction_list;
   	if (!instruction_list) 
	{
		head = instruction_list = (InstructionRtn*) malloc(sizeof(InstructionRtn));
	}
   	else 
   	{
     		/* get to the end of the linkedlist */
     		while(head->next) head = head->next;
     		head->next = (InstructionRtn*) malloc(sizeof(InstructionRtn));
     		head = head->next;
   	}

   	/* register the callback */
   	head->rtn  = cb;
   	head->next = NULL;

	/* done */
	return;
}

static void register_ibasicblock_cb(IBASICBLOCK_CALLBACK cb)
{
   	/* it is possible that this module does not define a basicblock callback */
   	if (!cb) return;

   	IBasicBlockRtn *head = basicblock_list;
   	if (!basicblock_list) 
	{
		head = basicblock_list = (IBasicBlockRtn*) malloc(sizeof(IBasicBlockRtn));
	}
   	else 
   	{
     		/* get to the end of the linkedlist */
     		while(head->next) head = head->next;
     		head->next = (IBasicBlockRtn*) malloc(sizeof(IBasicBlockRtn));
     		head = head->next;
   	}

   	/* register the callback */
   	head->rtn  = cb;
   	head->next = NULL;

	/* done */
	return;
}


/// @ qtrace_instrument - this function is called by the instrumentatiom module.
/// @ it parses what is requested by the instrumentation module into a icontext
/// @ structure.
void qtrace_instrument_parser(unsigned pos, ...) 
{
  	va_list arguments;                  
  	va_start(arguments, pos);         

        /* reset icontext */
        memset(&icontext, 0x0, sizeof(InstrumentContext));

	unsigned idx=0;
  	for (idx=0;idx<pos;idx++) 
  	{
     		unsigned arg = va_arg(arguments, unsigned);
     		switch (arg)
     		{
		/* instrumentation function address */
     		case QTRACE_IFUN:
        		icontext.ifun = va_arg(arguments, uintptr_t);
        		++idx;
        		break;
		/* memory address instrumentation */
		case QTRACE_MEMTRACE_MSIZE:
		case QTRACE_MEMTRACE_VMA:
		case QTRACE_MEMTRACE_PMA:
		case QTRACE_MEMTRACE_VPMA:
		case QTRACE_MEMTRACE_VALUE:
                        icontext.iargs[icontext.ciarg++] = arg;
			icontext.memfext |= arg;
			break;
		case QTRACE_PCTRACE_VMA:
                        icontext.iargs[icontext.ciarg++] = arg;
			icontext.pcfext |= arg;
			break;
		/* branch instrumentation */
     		case QTRACE_BRANCH_TARGET:
			printf("QTRACE_BRANCH_TARGET called\n");
                        icontext.iargs[icontext.ciarg++] = arg;
        		icontext.btarget = true;
			break;
     		default:
        		break;
     		}
  	}

  	va_end (arguments);          

	/* briefly verify the validity of the instrumentations */
	assert(icontext.ifun); /* must have valid inst function */

	/* done */
	return;
}

/// @ qtrace_instrument_parse - this function takes the name of the module and setup
/// @ the module. it loads the module first and then passes the module the function to 
/// @ call to request instrumentation. 
void qtrace_instrument_setup(const char *module)
{
   	/* load the instrumentation module */
   	if (!(handle=dlopen(module, RTLD_LAZY))) handle_unable_load_module(module);

   	/* register the instrumentation module functions with runtime */
   	register_instruction_cb((INSTRUCTION_CALLBACK)dlsym(handle, icb));
   	register_ibasicblock_cb((IBASICBLOCK_CALLBACK)dlsym(handle, bcb));

   	/* register runtime functions with the instrumentation module */
   	QTRACE_MODULE_FUNC_INIT module_function_init;
   	module_function_init = dlsym(handle, ims);
   	module_function_init(qtrace_instrument_parser);

   	/* done */
	return;
}

/// @ qtrace_invoke_instruction_callback - this function goes through the list 
/// @ of all instruction callbacks and call them one by one. the callbacks then
/// @ tells QTRACE what to instrument. 
void qtrace_invoke_instruction_callback(unsigned arg)
{
	InstructionRtn* curr_icc = instruction_list;

	/* call instruction callbacks one by one */
    	while(curr_icc) 
    	{
       		assert(curr_icc->rtn);
       		curr_icc->rtn(arg);
       		curr_icc = curr_icc->next;
    	} 
	
	/* done */
    	return;
}







