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

#ifndef QTRACE_H
#define QTRACE_H

#include <stdint.h>

/// ------------------------------------------------ ///
/// instruction types. 
/// ------------------------------------------------ ///
#define QTRACE_IS_FETCH           (1<<0)
#define QTRACE_IS_STORE           (1<<1)
#define QTRACE_IS_JMP             (1<<2)
#define QTRACE_IS_CALL            (1<<3)
#define QTRACE_IS_BRANCH          (1<<4)
#define QTRACE_IS_COND            (1<<5)
#define QTRACE_IS_USER            (1<<6)
#define QTRACE_IS_KERN            (1<<7)
#define QTRACE_IS_SYSENTER        (1<<8)
#define QTRACE_IS_SYSEXIT         (1<<9)
#define QTRACE_IS_SYSCALL         (1<<10)
#define QTRACE_IS_SYSRET          (1<<11)
#define QTRACE_IS_CPUID           (1<<12)
#define QTRACE_IS_HLT             (1<<13)
#define QTRACE_IS_INVLPGA         (1<<14)
#define QTRACE_IS_MONITOR         (1<<15)
#define QTRACE_IS_MWAIT           (1<<16)
#define QTRACE_IS_INVD            (1<<17)
#define QTRACE_IS_PREFETCH        (1<<18)
#define QTRACE_IS_NOP             (1<<19)
#define QTRACE_IS_MEMFENCE        (1<<20)
#define QTRACE_IS_CLFLUSH         (1<<21)
#define QTRACE_IS_VIRT            (1<<22)
#define QTRACE_IS_RDTSC           (1<<23)
#define QTRACE_IS_ARITHLOGIC      (1<<24)
#define QTRACE_IS_INDIRECT        (1<<25)
#define QTRACE_IS_RETURN          (1<<26)

#define QTRACE_TEST_FETCH(X)      ((X&QTRACE_IS_FETCH)>0)
#define QTRACE_TEST_STORE(X)      ((X&QTRACE_IS_STORE)>0)
#define QTRACE_TEST_JMP(X)        ((X&QTRACE_IS_JMP)>0)
#define QTRACE_TEST_CALL(X)       ((X&QTRACE_IS_CALL)>0)
#define QTRACE_TEST_COND(X)       ((X&QTRACE_IS_COND)>0)
#define QTRACE_TEST_BRANCH(X)     ((X&QTRACE_IS_BRANCH)>0)
#define QTRACE_TEST_USER(X)       ((X&QTRACE_IS_USER)>0)
#define QTRACE_TEST_KERN(X)       ((X&QTRACE_IS_KERN)>0)
#define QTRACE_TEST_SYSENTER(X)   ((X&QTRACE_IS_SYSENTER)>0)
#define QTRACE_TEST_SYSEXIT(X)    ((X&QTRACE_IS_SYSEXIT)>0)
#define QTRACE_TEST_SYSCALL(X)    ((X&QTRACE_IS_SYSCALL)>0)
#define QTRACE_TEST_SYSRET(X)     ((X&QTRACE_IS_SYSRET)>0)
#define QTRACE_TEST_CPUID(X)      ((X&QTRACE_IS_CPUID)>0)
#define QTRACE_TEST_HLT(X)        ((X&QTRACE_IS_HLT)>0)
#define QTRACE_TEST_INVLPGA(X)    ((X&QTRACE_IS_INVLPGA)>0)
#define QTRACE_TEST_MONITOR(X)    ((X&QTRACE_IS_MONITOR)>0)
#define QTRACE_TEST_MWAIT(X)      ((X&QTRACE_IS_MWAIT)>0)
#define QTRACE_TEST_INVD(X)       ((X&QTRACE_IS_INVD)>0)
#define QTRACE_TEST_PREFETCH(X)   ((X&QTRACE_IS_PREFETCH)>0)
#define QTRACE_TEST_NOP(X)        ((X&QTRACE_IS_NOP)>0)
#define QTRACE_TEST_MEMFENCE(X)   ((X&QTRACE_IS_MEMFENCE)>0)
#define QTRACE_TEST_CLFLUSH(X)    ((X&QTRACE_IS_CLFLUSH)>0)
#define QTRACE_TEST_VIRT(X)       ((X&QTRACE_IS_VIRT)>0)
#define QTRACE_TEST_RDTSC(X)      ((X&QTRACE_IS_RDTSC)>0) 
#define QTRACE_TEST_ARITHLOGIC(X) ((X&QTRACE_IS_ARITHLOGIC)>0) 
#define QTRACE_TEST_INDIRECT(X)   ((X&QTRACE_IS_INDIRECT)>0) 
#define QTRACE_TEST_RETURN(X)	  ((X&QTRACE_IS_RETURN)>0) 


/// ------------------------------------------------ ///
/// instrumentation prototypes. 
/// ------------------------------------------------ ///
typedef 
void (*INSTRUCTION_CALLBACK)(unsigned);

typedef 
void (*IBASICBLOCK_CALLBACK)(unsigned);

typedef 
void (*RESET_STATS)(void);

typedef 
void (*PRINT_STATS)(void);

typedef 
void 
(*QTRACE_INSERT_INSTRUMENT)(unsigned, ...);

typedef 
void 
(*QTRACE_MODULE_FINIT)(QTRACE_INSERT_INSTRUMENT);

/// ------------------------------------------------ ///
/// instrumentation module metadata 
/// ------------------------------------------------ ///
struct genericRtn { void *rtn; const char* mname; struct genericRtn *next; };
typedef struct genericRtn GenericRtn;
typedef struct genericRtn InstructionRtn;
typedef struct genericRtn IBasicBlockRtn;
typedef struct genericRtn MResetStatsRtn;
typedef struct genericRtn MPrintStatsRtn;

/// ------------------------------------------------ ///
/// instrumentation function list
/// ------------------------------------------------ ///
void qtrace_instrument_parser(unsigned pos, ...); 
void qtrace_instrument_setup(const char*);
void qtrace_invoke_instruction_callback(unsigned arg);
void qtrace_invoke_client_reset_stats(void);
void qtrace_invoke_client_print_stats(void);


/// ------------------------------------------------ ///
/// miscellaneous 
/// ------------------------------------------------ ///
#define QTRACE_EXIT  exit
#define QTRACE_ERROR printf
#define QTRACE_WAIT_COMMAND_HANDLED(X)  while(!X);
#define QTRACE_LOCAL_FUN  static
#define QTRACE_MAX_IARGS  16

/* instrumentations */
typedef struct InstrumentContext  {
    	uintptr_t ifun;     /* instrumentation function */
	unsigned  ipoint;   
    	unsigned  ciarg;    /* current iarg */
    	unsigned  iargs[QTRACE_MAX_IARGS]; /* instrumentation args */
    	unsigned  memfext;  /* this is the flag representing the instrumentation by the client */
    	unsigned  pcfext;   /* this is the flag representing the instrmentation for program counter */
    	unsigned  btarget;  /* this is the flag to indicate instrumenting the branch target */
	struct InstrumentContext *next;
} InstrumentContext;

#define QTRACE_ADD_FLAG(s,flag) 	s->qtrace_insnflags|=(flag);
#define QTRACE_ADD_COND_FLAG(s,flag,c) 	{if(c) s->qtrace_insnflags|=(flag);}
#define QTRACE_RESET_FLAG(s)    	s->qtrace_insnflags=0;
   
/* qtrace memory tracing related stuff */
#define QTRACE_MEMTRACE_BITS  		(4)
#define QTRACE_MEMTRACE_NONE  		(0)
#define QTRACE_MEMTRACE_VMA   		(1<<0)
#define QTRACE_MEMTRACE_PMA   		(1<<1) 
#define QTRACE_MEMTRACE_VPMA  		(QTRACE_MEMTRACE_VMA | QTRACE_MEMTRACE_PMA)
#define QTRACE_MEMTRACE_MSIZE 		(1<<2)     
#define QTRACE_MEMTRACE_VALUE 		(1<<3)     
#define QTRACE_MEMTRACE_PREOP_VALUE 	(1<<4)   
#define QTRACE_MEMTRACE_PSTOP_VALUE 	(1<<5)   

#define QTRACE_ADD_MEMTRACE(index,x)    (((index) | (x << QTRACE_MEMTRACE_BITS))) 
#define QTRACE_EXT_MEMTRACE(index)      ((index >> QTRACE_MEMTRACE_BITS)) 
#define QTRACE_EXT_MEMINDEX(index)      ((index & ((1<<QTRACE_MEMTRACE_BITS)-1))) 

#define RESET_QTRACE_MEMTRACE_ADDRS(x)	(x &= (~QTRACE_MEMTRACE_VPMA)) 
#define RESET_QTRACE_MEMTRACE_MSIZE(x)  (x &= (~QTRACE_MEMTRACE_MSIZE))
#define RESET_QTRACE_MEMTRACE_PREOP_VALUE(x)  (x &= (~QTRACE_MEMTRACE_PREOP_VALUE))
#define RESET_QTRACE_MEMTRACE_PSTOP_VALUE(x)  (x &= (~QTRACE_MEMTRACE_PSTOP_VALUE))

#define QTRACE_MEMTRACE_EXT_ADDRS(x) 	({unsigned i = (x & QTRACE_MEMTRACE_VPMA);  RESET_QTRACE_MEMTRACE_ADDRS(x); i;})
#define QTRACE_MEMTRACE_EXT_MSIZE(x) 	({unsigned i = (x & QTRACE_MEMTRACE_MSIZE); RESET_QTRACE_MEMTRACE_MSIZE(x); i;})
#define QTRACE_MEMTRACE_EXT_PREOP_VALUE(x) 	({unsigned i = (x & QTRACE_MEMTRACE_PREOP_VALUE); RESET_QTRACE_MEMTRACE_PREOP_VALUE(x); i;})
#define QTRACE_MEMTRACE_EXT_PSTOP_VALUE(x) 	({unsigned i = (x & QTRACE_MEMTRACE_PSTOP_VALUE); RESET_QTRACE_MEMTRACE_PSTOP_VALUE(x); i;})

/* qtrace program counter */
#define QTRACE_PCTRACE_VMA		(1<<6)

/* QTRACE branch related */
#define QTRACE_BRANCH_TARGET  		(1<<7)   
#define QTRACE_BRANCH_TAKEN  		(1<<8)   
#define QTRACE_BRANCH_NOTTAKEN  	(1<<9)   
#define QTRACE_PROCESS_UPID		(1<<10)

/* QTRACE instrumentation function */
#define QTRACE_IFUN  	      		(1<<15)
#define QTRACE_IPOINT_BEFORE  		(1<<16)
#define QTRACE_IPOINT_AFTER   		(1<<17)




#endif /* QTRACE_H */

