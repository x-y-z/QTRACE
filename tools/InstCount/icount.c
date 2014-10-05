#include "qtrace-client-header.h"
#include <stdio.h>

#define R_EAX 0
#define R_ECX 1
#define R_EDX 2
#define R_EBX 3
#define R_ESP 4
#define R_EBP 5
#define R_ESI 6
#define R_EDI 7


void CacheSim(void *vma, unsigned msize)
{
//    printf("vma is 0x%lx msize is %d\n", (unsigned long)vma, msize);
}

void JmpSim(void* target, unsigned long upid)
{
  // 	printf("target is 0x%lx upid is %ld\n", target, upid);
}

void InstructionCallBack(unsigned type)
{
    if (QTRACE_TEST_FETCH(type))
    {
#if 0
       	Module_INS_InsertCall(5, 
                              QTRACE_IPOINT_BEFORE, 
                              QTRACE_IFUN, CacheSim, 
                              QTRACE_MEMTRACE_VMA,
                              QTRACE_MEMTRACE_MSIZE);
#else
       	Module_INS_InsertCall(5, 
                              QTRACE_IPOINT_BEFORE, 
                              QTRACE_IFUN, CacheSim, 
                              QTRACE_REGTRACE_VALUE,
                              R_EAX);
#endif
    }
    return;
}

void StatsReset() { printf("Count StatsReset\n"); }
void StatsPrint() { printf("Count StatsPrint\n"); }

static int main(void) __attribute__((constructor));
int main(void)
{
	AddInstructionCallBack(InstructionCallBack);
    AddStatsResetCallBack(StatsReset);
    AddStatsPrintCallBack(StatsPrint);
	return 0;
}
