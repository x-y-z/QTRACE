#include "qtrace.h"
#include <stdio.h>

#define PRINT_FREQ 100000

static int usercount = 0;
static int kerncount = 0;
static int totlcount = 0;

void InstructionCallBack(unsigned type)
{
    printf("hello\n");
    totlcount ++;
    usercount += QTRACE_TEST_INVLPGA(type);
    if (totlcount % PRINT_FREQ == 0) printf("totalcount is %d QTRACE_TEST_INVLPGA:%d kern:%d\n", totlcount, usercount);
}
