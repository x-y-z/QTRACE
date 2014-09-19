#include "qtrace.h"
#include "qtrace_header.h"
#include <stdio.h>

void CacheSim(void* vma, unsigned msize1, unsigned msize2, unsigned msize3, unsigned msize4, unsigned msize5, unsigned msize6)
{
//	printf("vma is 0x%lx pma is 0x%lx msize is %d value is 0x%lx pc is 0x%lx\n", vma, pma, msize, value, pc);
//	printf("%d %d %d %d %d %d\n", msize1, msize2, msize3, msize4, msize5, msize6);
}

void JmpSim(void* target, unsigned long upid)
{
  // 	printf("target is 0x%lx upid is %ld\n", target, upid);
}

void InstructionCallBack(unsigned type)
{
    	if (QTRACE_TEST_FETCH(type))
    	{
       	Module_INS_InsertCall(7, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, CacheSim, 
                             QTRACE_MEMTRACE_VMA, QTRACE_MEMTRACE_MSIZE, QTRACE_MEMTRACE_MSIZE, QTRACE_MEMTRACE_MSIZE);

#if 0
			     QTRACE_MEMTRACE_MSIZE,
			     QTRACE_MEMTRACE_MSIZE,
			     QTRACE_MEMTRACE_MSIZE,
			     QTRACE_MEMTRACE_MSIZE,
                             QTRACE_MEMTRACE_MSIZE);
#endif
    	}

#if 0
    	if (QTRACE_TEST_FETCH(type))
    	{
       	Module_INS_InsertCall(5, 
                             QTRACE_IPOINT_BEFORE, 
                             QTRACE_IFUN, CacheSim, 
                             QTRACE_MEMTRACE_VMA,
			     QTRACE_MEMTRACE_MSIZE);
    	}
#endif


#if 0
    	//if (QTRACE_TEST_JMP(type))
    	if (QTRACE_TEST_BRANCH(type) && QTRACE_TEST_INDIRECT(type))
    	{
       	Module_INS_InsertCall(5, 
                             QTRACE_IPOINT_AFTER, 
                             QTRACE_IFUN, JmpSim, 
                             QTRACE_BRANCH_TARGET, 
			     QTRACE_PROCESS_UPID);
 
    	}
#endif
}

void StatsReset() { printf("Count StatsReset\n"); }

static int main(void) __attribute__((constructor));
int main(void)
{
	AddInstructionCallBack(InstructionCallBack);
        AddStatsReset(StatsReset);
        //AddStatsPrint(StatsPrint);
	return 0;
}
