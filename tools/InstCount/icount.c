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

void InstructionCallBack(unsigned type)
{
    totlcount ++;
    usercount += QTRACE_TEST_INVLPGA(type);
    if (totlcount % PRINT_FREQ == 0) printf("totalcount is %d QTRACE_TEST_INVLPGA:%d kern:%d\n", totlcount, usercount);
}
