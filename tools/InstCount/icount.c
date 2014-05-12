#include "qtrace.h"
#include <stdio.h>

static int usercount = 0;
static int kerncount = 0;
static int totlcount = 0;

void InstructionCallBack(unsigned type)
{
    totlcount ++;
    usercount += QTRACE_TEST_USER(type);
    kerncount += QTRACE_TEST_KERN(type);

    if (totlcount % 1000000 == 0) printf("user:%d kern:%d\n", usercount, kerncount);
}
