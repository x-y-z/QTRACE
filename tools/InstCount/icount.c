#include "qtrace.h"
#include <stdio.h>

#define PRINT_FREQ 100000

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

QTRACE_LOCAL_FUN void CacheSim(void* vma, void *pma, unsigned msize)
{
}

void InstructionCallBack(unsigned type)
{
    if (QTRACE_TEST_FETCH(type))
    {
       Module_INS_InsertCall(5, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, 0, 
                             QTRACE_MEMORY_VMA, 
                             QTRACE_MEMORY_PMA, 
                             QTRACE_MEMORY_SIZE);
    }
}
