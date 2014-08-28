#include "qtrace.h"
#include "qtrace_header.h"
#include <stdio.h>

void CacheSim(void* vma, void *pma, unsigned msize, unsigned value, void *pc)
{
//	printf("vma is 0x%lx pma is 0x%lx msize is %d value is 0x%lx pc is 0x%lx\n", vma, pma, msize, value, pc);
}

void JmpSim(void* target, unsigned long upid)
{
//   	printf("target is 0x%lx upid is %ld\n", target, upid);
}

void InstructionCallBack(unsigned type)
{
#if 1 
    	if (QTRACE_TEST_FETCH(type))
    	{
       	Module_INS_InsertCall(8, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, CacheSim, 
                             QTRACE_MEMTRACE_VMA,
			     QTRACE_MEMTRACE_MSIZE,
			     QTRACE_MEMTRACE_PMA,
			     QTRACE_MEMTRACE_VALUE,
			     QTRACE_PCTRACE_VMA);
 
#if 0
                             QTRACE_MEMTRACE_PMA, 
                             QTRACE_MEMTRACE_MSIZE,
                             QTRACE_MEMTRACE_VALUE,
                             QTRACE_PCTRACE_VMA);
#endif
    	}
#else
    	//if (QTRACE_TEST_JMP(type))
    	if (QTRACE_TEST_BRANCH(type) && QTRACE_TEST_INDIRECT(type))
    	{
       	Module_INS_InsertCall(5, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, JmpSim, 
                             QTRACE_BRANCH_TARGET, 
			     QTRACE_PROCESS_UPID);
 
    	}
#endif 
}

void StatsReset() { printf("StatsReset\n"); }

static int main(void) __attribute__((constructor));
int main(void)
{
	AddInstructionCallBack(InstructionCallBack);
        AddStatsReset(StatsReset);
        //AddStatsPrint(StatsPrint);
	return 0;
}
