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
static InstrumentContext *ictxhead = NULL;

InstructionRtn* instruction_list = NULL;
IBasicBlockRtn* ibasicblock_list = NULL;
MResetStatsRtn* resetstats_list  = NULL;
MPrintStatsRtn* printstats_list  = NULL;

/* the current and last unique instruction id */
unsigned long long uiid = 0;
unsigned long long last_uiid = 0;

#define MAX_ICONTEXT_ROOT 256
static unsigned rootcount;
static InstrumentContext* rootarray[MAX_ICONTEXT_ROOT];

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
	memcpy(head->mname, name, strlen(name)+1);
   	head->next  = NULL;

	/* done */
	return;
}

/// @ register_stats_reset - register a function to resetstats_list list.
static void register_stats_reset(RESET_STATS rs, const char *name)  
{ 
	add_function_to_list(rs, name, &resetstats_list);
}
/// @ register_stats_print - register a function to printstats_list list.
static void register_stats_print(PRINT_STATS ps, const char *name)  
{ 
	add_function_to_list(ps, name, &printstats_list);
}
/// @ register_instruction_cb - register a function to instruction_list list.
static void register_instruction_cb(INSTRUCTION_CALLBACK cb, const char* name)
{
	add_function_to_list(cb, name, &instruction_list);
}
/// @ register_ibasicblock_cb - register a function to basicblock_list list.
static void register_ibasicblock_cb(IBASICBLOCK_CALLBACK cb, const char* name)
{
	add_function_to_list(cb, name, &ibasicblock_list);
}

/// @ qtrace_allocate_icontext_root - allocate a icontext root pointer.
inline void qtrace_allocate_icontext_root(void) { rootcount++;  assert(rootcount < MAX_ICONTEXT_ROOT); }
/// @ qtrace_increment_uiid - increment the uiid. one more instruction has passed.
inline void qtrace_increment_uiid(void) { uiid ++; }
/// @ qtrace_get_current_icontext_list - return the current icontext head.
inline InstrumentContext* qtrace_get_current_icontext_list(void)  {  return ictxhead; }
/// @ qtrace_set_current_icontext_list - set the current icontext head.
inline void qtrace_set_current_icontext_list(InstrumentContext *ictx) { ictxhead = ictx; }

/// @ qtrace_allocate_new_icontext - allocate a new instrumentation context
/// @ pointed by root.
void qtrace_allocate_new_icontext(InstrumentContext **root)
{	
	InstrumentContext* newhead = malloc(sizeof(InstrumentContext));
        memset(newhead, 0x0, sizeof(InstrumentContext));
	newhead->next = *root;
	*root = newhead;
	ictxhead = newhead;
}

/// @ qtrace_free_all_icontexts - free all allocated instrumentation context
/// @ pointed by root.
void qtrace_free_root_icontext(InstrumentContext *root) 
{
	InstrumentContext *next = NULL;
	while(root) 
	{
		next = root->next;  free(root);
		root = next;
	}
	return;
}

/// @ qtrace_free_all_icontexts - free all the allocated instrumentation context.
void qtrace_free_all_icontexts(void)
{
	unsigned idx;
	for(idx = 0; idx < rootcount; ++idx)
	{
		qtrace_free_root_icontext(rootarray[idx]);
		rootarray[idx] = NULL;
	}	
	rootcount = 0;
	ictxhead  = NULL;
	return;
}

/// @ qtrace_verify_last_icontext - make sure the icontext makes sense before 
/// @ generating instrumetation.
void qtrace_verify_last_icontext(InstrumentContext *ictx)
{
	if (!ictx->ifun) QTRACE_ERROR("icontext ifun is NULL\n"); 
}

/// @ qtrace_instrument - this function is called by the instrumentatiom module.
/// @ it parses what is requested by the instrumentation module into a icontext
/// @ structure.
void qtrace_instrument_parser(unsigned pos, ...) 
{
  	va_list arguments;                  
  	va_start(arguments, pos);         

	/* every instruction will call qtrace_instrument_parser only once.
	   and 1 icontext list will be allocated */
	if (last_uiid != uiid)
	{
		qtrace_allocate_icontext_root();
	}

        /* allocate a icontext in the current rootarray */
	assert(rootcount);
	qtrace_allocate_new_icontext(&rootarray[rootcount-1]);

	unsigned idx=0;
  	for (idx=0;idx<pos;idx++) 
  	{
     		unsigned arg = va_arg(arguments, unsigned);
     		switch (arg)
     		{
		case QTRACE_IPOINT_BEFORE:
		case QTRACE_IPOINT_AFTER:
			ictxhead->ipoint = arg; 
			break;
		/* instrumentation function address */
     		case QTRACE_IFUN:
        		ictxhead->ifun = va_arg(arguments, uintptr_t);
        		++idx;
        		break;
		/* memory address instrumentation */
		case QTRACE_MEMTRACE_MSIZE:
		case QTRACE_MEMTRACE_VMA:
		case QTRACE_MEMTRACE_PMA:
		case QTRACE_MEMTRACE_VPMA:
                        ictxhead->iargs[ictxhead->ciarg++] = arg;
			ictxhead->memfext |= arg;
			break;
		case QTRACE_PCTRACE_VMA:
                        ictxhead->iargs[ictxhead->ciarg++] = arg;
			ictxhead->pcfext |= arg;
			break;
		/* branch instrumentation */
     		case QTRACE_BRANCH_TARGET:
                        ictxhead->iargs[ictxhead->ciarg++] = arg;
        		ictxhead->btarget = true;
			break;
		/* process unique id instrumentation */
		case QTRACE_PROCESS_UPID:
                        ictxhead->iargs[ictxhead->ciarg++] = arg;
			break;
		/* expand memory value to preop and pstop value */
		case QTRACE_MEMTRACE_VALUE:
			arg = (QTRACE_IPOINT_BEFORE == ictxhead->ipoint) ? 
                               QTRACE_MEMTRACE_PREOP_VALUE : 
			       QTRACE_MEMTRACE_PSTOP_VALUE ;
			ictxhead->iargs[ictxhead->ciarg++]  = arg; 
			ictxhead->memfext |= arg;
			break;
     		default:
        		break;
     		}
  	}

  	va_end (arguments);          

	last_uiid = uiid;

	/* briefly verify the validity of the instrumentations */
	qtrace_verify_last_icontext(ictxhead);

	/* done */
	return ;
}

/// @ qtrace_instrument_parse - this function takes the name of the module and setup
/// @ the module. it loads the module first and then passes the module the function to 
/// @ call to request instrumentation. 
void qtrace_instrument_setup(const char *module)
{
	unsigned idx = 0;
        void* handle = NULL;

	const char *ResetStatsName = "ResetStats";
	const char *PrintStatsName = "PrintStats";
	const char *InstructionCBArrayName = "InstructionCallBackArray";
	const char *InstructionCBNumName   = "InstructionCallBackNum";
	const char *BasicBlockCBArrayName  = "BasicBlockCallBackArray";
	const char *BasicBlockCBNumName    = "BasicBlockCallBackNum";
	const char *InitInsInstrumentName  = "InitializeInstructionInstrument";

   	/* load the instrumentation module */
   	if (!(handle=dlopen(module, RTLD_LAZY))) handle_unable_load_module(module);

	/* register all instruction callbacks */
	void **InstructionCallBackArray = (void **)
					  dlsym(handle, 
				          InstructionCBArrayName); 
        unsigned InstructionCallBackNum = *(unsigned*) 
                                          dlsym(handle, 
                                          InstructionCBNumName);
	assert(InstructionCallBackArray);
	for(idx=0; idx<InstructionCallBackNum; ++idx) 
	{
		register_instruction_cb(((INSTRUCTION_CALLBACK)
				       InstructionCallBackArray[idx]), module);
	}

	/* register all basicblock callbacks */
	void **BasicBlockCallBackArray = (void **)
                                         dlsym(handle, BasicBlockCBArrayName); 
        unsigned BasicBlockCallBackNum = *(unsigned*) 
                                         dlsym(handle, BasicBlockCBNumName);
	assert(BasicBlockCallBackArray);
	for(idx=0; idx<BasicBlockCallBackNum; ++idx) 
	{
   		register_ibasicblock_cb((IBASICBLOCK_CALLBACK)
				       BasicBlockCallBackArray[idx], module);
	}

	/* register module reset and print stats */
	register_stats_reset(*(void**) dlsym(handle, ResetStatsName), module);
	register_stats_print(*(void**) dlsym(handle, PrintStatsName), module);

   	/* register runtime functions with the instrumentation module */
   	QTRACE_MODULE_FINIT module_finit;

   	module_finit = (QTRACE_MODULE_FINIT)
                       dlsym(handle, InitInsInstrumentName);
	assert(module_finit);
   	module_finit(qtrace_instrument_parser);

   	/* done */
	return;
}

/// @ qtrace_invoke_instruction_callback - this function goes through the list 
/// @ of all instruction callbacks and call them one by one. the callbacks then
/// @ tells QTRACE what to instrument. 
#define qtrace_invoke_callback(unit, ulist)			\
void qtrace_invoke_##unit##_callback(unsigned arg)		\
{								\
	GenericRtn* curr_icc = ulist;				\
    	while(curr_icc) 					\
    	{							\
       		assert(curr_icc->rtn);				\
       		((INSTRUCTION_CALLBACK)curr_icc->rtn)(arg);	\
       		curr_icc = curr_icc->next;			\
    	} 							\
    	return;							\
}	

qtrace_invoke_callback(instruction, instruction_list);
qtrace_invoke_callback(ibasicblock, ibasicblock_list);
#undef qtrace_invoke_callback

/// @ qtrace_invoke_client - call client defined reset/print functions.
#define qtrace_invoke_client(rtnm, flist)			\
void qtrace_invoke_client_##rtnm(const char* name)		\
{								\
	GenericRtn* curr_rtn = (GenericRtn*) flist;		\
	bool func_done = false;					\
    	while(curr_rtn)						\
    	{							\
       		assert(curr_rtn->rtn);				\
		if (name) 					\
		{						\
			if (!strcmp(name, curr_rtn->mname))	\
			{					\
				((RESET_STATS)curr_rtn->rtn)();	\
				func_done = true;		\
				break;				\
			} 					\
		}						\
		else 						\
		{						\
			((RESET_STATS)curr_rtn->rtn)();		\
		}						\
       		curr_rtn = curr_rtn->next;			\
    	} 							\
								\
	if (name && !func_done) 				\
	{							\
		QTRACE_ERROR("call %s failed\n", name);		\
	}							\
	return;							\
}							

qtrace_invoke_client(reset_stats, resetstats_list);
qtrace_invoke_client(print_stats, printstats_list);
#undef qtrace_invoke_client


#define qtrace_instrumentation_sum(var, x)  			\
unsigned qtrace_sum_##var(InstrumentContext *root) 		\
{								\
	unsigned ext = 0;					\
	InstrumentContext *head = root;				\
	while(head) 						\
	{							\
		ext |= head->x;					\
		head = head->next;				\
	}							\
	return ext;						\
} 

qtrace_instrumentation_sum(ipoint, ipoint);
qtrace_instrumentation_sum(memfext, memfext);
#undef qtrace_instrumentation_sum

unsigned qtrace_has_call(InstrumentContext *root, unsigned flag)
{
    	return qtrace_sum_ipoint(root) & flag;
}


