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
#define QTRACE_IS_FETCH   (1<<0)
#define QTRACE_IS_STORE   (1<<1)
#define QTRACE_IS_JMP     (1<<2)
#define QTRACE_IS_CALL    (1<<3)
#define QTRACE_IS_BRANCH  (1<<4)
#define QTRACE_IS_COND    (1<<5)
#define QTRACE_IS_USER    (1<<6)
#define QTRACE_IS_KERN    (1<<7)

#define QTRACE_TEST_FETCH(X)   (X&QTRACE_IS_FETCH)
#define QTRACE_TEST_STORE(X)   (X&QTRACE_IS_STORE)
#define QTRACE_TEST_JMP(X)     (X&QTRACE_IS_JMP)
#define QTRACE_TEST_CALL(X)    (X&QTRACE_IS_CALL)
#define QTRACE_TEST_COND(X)    (X&QTRACE_IS_COND)
#define QTRACE_TEST_BRANCH(X)  (X&QTRACE_IS_BRANCH)
#define QTRACE_TEST_USER(X)    (X&QTRACE_IS_USER)
#define QTRACE_TEST_KERN(X)    (X&QTRACE_IS_KERN)

#endif /* QTRACE_H */

