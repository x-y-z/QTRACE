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

/// ------------------------------------------------ ///
/// instrumentation function list
/// ------------------------------------------------ ///
void qtrace_instrument_parse(const char*);
void qtrace_invoke_instruction_callback(unsigned arg0);
typedef void (*INSTRUCTIONCB)(unsigned);
typedef void (*BASICBLOCKCB)(unsigned);

typedef struct InstructionRtn {
   INSTRUCTIONCB rtn;
   struct InstructionRtn *next;
} InstructionRtn;

typedef struct IBasicBlockRtn {
   BASICBLOCKCB rtn;
   struct IBasicBlockRtn *next;
} IBasicBlockRtn;

extern InstructionRtn* icb_head;
extern IBasicBlockRtn* ibb_head;

/// ------------------------------------------------ ///
/// miscellaneous 
/// ------------------------------------------------ ///
#define QTRACE_EXIT  exit
#define QTRACE_ERROR printf
#define QTRACE_WAIT_COMMAND_HANDLED(X)  while(!X);

/// ------------------------------------------------ ///
/// instruction types. 
/// ------------------------------------------------ ///
#define QTRACE_IS_FETCH    (1<<0)
#define QTRACE_IS_STORE    (1<<1)
#define QTRACE_IS_JMP      (1<<2)
#define QTRACE_IS_CALL     (1<<3)
#define QTRACE_IS_BRANCH   (1<<4)
#define QTRACE_IS_COND     (1<<5)
#define QTRACE_IS_USER     (1<<6)
#define QTRACE_IS_KERN     (1<<7)
#define QTRACE_IS_SYSENTER (1<<8)
#define QTRACE_IS_SYSEXIT  (1<<9)
#define QTRACE_IS_SYSCALL  (1<<10)
#define QTRACE_IS_SYSRET   (1<<11)
#define QTRACE_IS_CPUID    (1<<12)
#define QTRACE_IS_HLT      (1<<13)
#define QTRACE_IS_INVLPGA  (1<<14)
#define QTRACE_IS_MONITOR  (1<<15)
#define QTRACE_IS_MWAIT    (1<<16)
#define QTRACE_IS_INVD     (1<<17)
#define QTRACE_IS_PREFETCH (1<<18)
#define QTRACE_IS_NOP      (1<<19)
#define QTRACE_IS_MEMFENCE (1<<20)
#define QTRACE_IS_CLFLUSH  (1<<21)
#define QTRACE_IS_VIRT     (1<<22)
#define QTRACE_IS_RDTSC    (1<<23)
#define QTRACE_IS_ARITHLOGIC (1<<24)

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

#endif /* QTRACE_H */

