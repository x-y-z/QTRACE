#include "qtrace.h"
#include <stdio.h>

#define PRINT_FREQ 100000

#define __cdecl __attribute__((__cdecl__))

static int usercount = 0;
static int kerncount = 0;
static int totlcount = 0;

static QTRACE_INSERT_INSTRUMENT_CALLBACK Module_INS_InsertCall = NULL;
static QTRACE_INSERT_INSTRUMENT_CALLBACK Module_IBB_InsertCall = NULL;

void INS_InsertCall(QTRACE_INSERT_INSTRUMENT_CALLBACK ins_insertcall_func)
{
    Module_INS_InsertCall = ins_insertcall_func;
}

void IBB_InsertCall(QTRACE_INSERT_INSTRUMENT_CALLBACK ibb_insertcall_func)
{
    Module_IBB_InsertCall = ibb_insertcall_func;
}

void CacheSim(void* vma, void *pma, unsigned msize)  __attribute__((cdecl));

void __attribute__((cdecl)) CacheSim(void* vma, void *pma, unsigned msize)
{
   printf("vma is 0x%lx pma is 0x%lx msize is %d\n", vma, pma, msize);
}

void InstructionCallBack(unsigned type)
{
#if 0
    if (QTRACE_TEST_FETCH(type))
    {
       Module_INS_InsertCall(5, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, 0, 
                             QTRACE_MEMORY_VMA, 
                             QTRACE_MEMORY_PMA, 
                             QTRACE_MEMORY_SIZE);
    }
#endif 

    if (QTRACE_TEST_BRANCH(type) && QTRACE_TEST_INDIRECT(type))
    {
       Module_INS_InsertCall(4, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, CacheSim, 
                             QTRACE_BRANCH_TARGET);
 
    }
}
