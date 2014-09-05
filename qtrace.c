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
InstrumentContext *ictxhead = NULL;

InstructionRtn* instruction_list = NULL;
IBasicBlockRtn* basicblock_list  = NULL;
MResetStatsRtn* resetstats_list  = NULL;
MPrintStatsRtn* printstats_list  = NULL;

static inline void handle_unable_load_module(const char *optarg)
{
   	QTRACE_ERROR("unable to load instrumentation module %s\n", optarg);
   	QTRACE_EXIT(-1);
}

static void add_function_to_list(void *cb, const char *name, GenericRtn **list)
{
   	/* it is possible that this module does not define a instruction callback */
   	if (!cb) return;

   	GenericRtn *head = *list;
   	if (!*list) 
	{
		head = *list = (GenericRtn*) malloc(sizeof(GenericRtn));
	}
   	else 
   	{
     		/* get to the end of the linkedlist */
     		while(head->next) head = head->next;
     		head->next = (GenericRtn*) malloc(sizeof(GenericRtn));
     		head = head->next;
   	}

   	/* register the callback */
   	head->rtn   = cb;
	head->mname = malloc(strlen(name)+1); 
	memcpy(head->mname, (char*)name, strlen(name)+1);
   	head->next  = NULL;

	/* done */
	return;

}

static void register_stats_reset(RESET_STATS rs, const char *name)  
{ 
	add_function_to_list(rs, name, &resetstats_list);
}

static void register_stats_print(PRINT_STATS ps, const char *name)  
{ 
	add_function_to_list(ps, name, &printstats_list);
}

static void register_ibasicblock_cb(IBASICBLOCK_CALLBACK cb, const char* name)
{
	add_function_to_list(cb, name, &basicblock_list);
}

static void register_instruction_cb(INSTRUCTION_CALLBACK cb, const char* name)
{
	add_function_to_list(cb, name, &instruction_list);
}


InstrumentContext *qtrace_allocate_new_icontext(void)
{	
	InstrumentContext* ct = malloc(sizeof(InstrumentContext));
        memset(ct, 0x0, sizeof(InstrumentContext));

	ct->next = ictxhead;
	ictxhead = ct;
	return ct;
}

void qtrace_free_all_icontexts(void)
{
	InstrumentContext *next = NULL;
	while(ictxhead) 
	{
		next = ictxhead->next;
		free(ictxhead);
		ictxhead = next;
	}
	ictxhead = NULL;
}

#if 0
#define QTRACE_SUM(var)  	\
unsigned qtrace_sum_##var(void) {	\
	unsigned ext = 0;	\
	InstrumentContext *head = ictxhead;		\
	while(head) 					\
	{					\
		ext |= head-> ##var;	\
		head = head->next;		\
	}			\
	return ext;	\
} 

QTRACE_SUM(memfext);
#endif 


unsigned qtrace_sum_memfext()
{
	unsigned memfext = 0;
	InstrumentContext *head = ictxhead;
	while(head) 
	{
		memfext |= ictxhead->memfext; 
		head = head->next;
	}
	return memfext;
}

unsigned qtrace_sum_ipoint()
{
	unsigned ipoint = 0;
	InstrumentContext *head = ictxhead;
	while(head) 
	{
		ipoint |= ictxhead->ipoint; 
		head = head->next;
	}
	return ipoint;
}


/// @ qtrace_instrument - this function is called by the instrumentatiom module.
/// @ it parses what is requested by the instrumentation module into a icontext
/// @ structure.
void qtrace_instrument_parser(unsigned pos, ...) 
{
  	va_list arguments;                  
  	va_start(arguments, pos);         

	InstrumentContext* icontext = qtrace_allocate_new_icontext();

	unsigned idx=0;
  	for (idx=0;idx<pos;idx++) 
  	{
     		unsigned arg = va_arg(arguments, unsigned);
     		switch (arg)
     		{
		case QTRACE_IPOINT_BEFORE:
		case QTRACE_IPOINT_AFTER:
			icontext->ipoint = arg; 
			break;
		/* instrumentation function address */
     		case QTRACE_IFUN:
        		icontext->ifun = va_arg(arguments, uintptr_t);
        		++idx;
        		break;
		/* memory address instrumentation */
		case QTRACE_MEMTRACE_MSIZE:
		case QTRACE_MEMTRACE_VMA:
		case QTRACE_MEMTRACE_PMA:
		case QTRACE_MEMTRACE_VPMA:
                        icontext->iargs[icontext->ciarg++] = arg;
			icontext->memfext |= arg;
			break;
		case QTRACE_PCTRACE_VMA:
                        icontext->iargs[icontext->ciarg++] = arg;
			icontext->pcfext |= arg;
			break;
		/* branch instrumentation */
     		case QTRACE_BRANCH_TARGET:
                        icontext->iargs[icontext->ciarg++] = arg;
        		icontext->btarget = true;
			break;
		/* process unique id instrumentation */
		case QTRACE_PROCESS_UPID:
                        icontext->iargs[icontext->ciarg++] = arg;
			break;
		/* expand memory value to preop and pstop value */
		case QTRACE_MEMTRACE_VALUE:
			arg = (QTRACE_IPOINT_BEFORE == icontext->ipoint) ? 
                               QTRACE_MEMTRACE_PREOP_VALUE : 
			       QTRACE_MEMTRACE_PSTOP_VALUE ;
			icontext->iargs[icontext->ciarg++]  = arg; 
			icontext->memfext |= arg;
			break;
     		default:
        		break;
     		}
  	}

  	va_end (arguments);          

	/* briefly verify the validity of the instrumentations */

	/* done */
	return;
}

/// @ qtrace_instrument_parse - this function takes the name of the module and setup
/// @ the module. it loads the module first and then passes the module the function to 
/// @ call to request instrumentation. 
void qtrace_instrument_setup(const char *module)
{
	unsigned idx = 0;
        void* handle = NULL;

   	/* load the instrumentation module */
   	if (!(handle=dlopen(module, RTLD_LAZY))) handle_unable_load_module(module);

	/* register all instruction callbacks */
	void **InstructionCallBackArray = (void **)
					  dlsym(handle, 
				          "InstructionCallBackArray"); 
        unsigned InstructionCallBackNum = *(unsigned*) 
                                          dlsym(handle, 
                                          "InstructionCallBackNum");
	assert(InstructionCallBackArray);
	for(idx=0; idx<InstructionCallBackNum; ++idx) 
	{
		register_instruction_cb(((INSTRUCTION_CALLBACK)InstructionCallBackArray[idx]), module);
	}

	/* register all basicblock callbacks */
	void **BasicBlockCallBackArray = (void **)
                                         dlsym(handle, 
                                         "BasicBlockCallBackArray"); 
        unsigned BasicBlockCallBackNum = *(unsigned*) 
                                         dlsym(handle, 
                                         "BasicBlockCallBackNum");
	assert(BasicBlockCallBackArray);
	for(idx=0; idx<BasicBlockCallBackNum; ++idx) 
	{
   		register_ibasicblock_cb((IBASICBLOCK_CALLBACK)BasicBlockCallBackArray[idx], module);
	}

	/* register module reset and print stats */
	register_stats_reset(*(void**) dlsym(handle, "ResetStats"), module);
	register_stats_print(*(void**) dlsym(handle, "PrintStats"), module);

   	/* register runtime functions with the instrumentation module */
   	QTRACE_MODULE_FINIT module_finit;

   	module_finit = (QTRACE_MODULE_FINIT)
                       dlsym(handle, 
                       "InitializeInstructionInstrument");
	assert(module_finit);
   	module_finit(qtrace_instrument_parser);


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
       		((INSTRUCTION_CALLBACK)curr_icc->rtn)(arg);
       		curr_icc = curr_icc->next;
    	} 
	
	/* done */
    	return;
}

void qtrace_invoke_client_reset_stats(void)
{
	MResetStatsRtn* curr_mrs = resetstats_list;

	/* call instruction callbacks one by one */
    	while(curr_mrs) 
    	{
       		assert(curr_mrs->rtn);
       		((RESET_STATS)curr_mrs->rtn)();
       		curr_mrs = curr_mrs->next;
    	} 
	
	/* done */
    	return;
}







